#!/usr/bin/env python3
#
# Copyright (c) 2023 Rumbledethumps
#
# SPDX-License-Identifier: BSD-3-Clause
# SPDX-License-Identifier: Unlicense

# Control RP6502 RIA via UART

import os
import io
import re
import time
import serial
import binascii
import argparse
import platform
from typing import Union


class Monitor:
    DEFAULT_TIMEOUT = 0.5
    UART_BAUDRATE = 115200
    serial = serial.Serial()

    def __init__(self, name, timeout=DEFAULT_TIMEOUT):
        self.serial.setPort(name)
        self.serial.timeout = timeout
        self.serial.baudrate = self.UART_BAUDRATE
        self.serial.open()

    def send_break(self, duration=0.01):
        ''' Stop the 6502 and return to monitor. '''
        self.serial.read_all()
        if platform.system() == "Darwin":
            self.serial.baudrate = 1200
            self.serial.write(b'\0')
            self.serial.baudrate = self.UART_BAUDRATE
        else:
            self.serial.send_break(duration)
        self.wait_for_prompt(']')

    def command(self, str, timeout=DEFAULT_TIMEOUT):
        ''' Send one command and wait for next monitor prompt '''
        self.serial.write(bytes(str, 'utf-8'))
        self.serial.write(b'\r')
        self.wait_for_prompt(']', timeout)

    def reset(self):
        ''' Start the 6502. '''
        self.serial.write(b'RESET\r')
        self.serial.read_until()

    def binary(self, addr: int, data):
        ''' Send data to memory using BINARY command. '''
        command = f'BINARY ${addr:04X} ${len(data):03X} ${binascii.crc32(data):08X}\r'
        self.serial.write(bytes(command, 'utf-8'))
        self.serial.write(data)
        self.wait_for_prompt(']')

    def upload(self, name, data):
        ''' Upload readable (file,rom,etc) to remote file "name" '''
        self.serial.write(bytes(f'UPLOAD {name}\r', 'utf-8'))
        self.wait_for_prompt('}')
        data.seek(0)
        while True:
            chunk = data.read(1024)
            if len(chunk) == 0:
                break
            command = f'${len(chunk):03X} ${binascii.crc32(chunk):08X}\r'
            self.serial.write(bytes(command, 'utf-8'))
            self.serial.write(chunk)
            self.wait_for_prompt('}')
        self.serial.write(b'END\r')
        self.wait_for_prompt(']')

    def send_reset_vector(self, addr: Union[int, None] = None):
        ''' Set reset vector. Use start address of last file as default. '''
        if addr == None:
            addr = self.reset_vector_guess
        if addr == None:
            raise RuntimeError("Reset vector not set")
        self.binary(0xFFFC, bytearray([addr & 0xFF, addr >> 8]))

    def send_file_to_memory(self, name, addr: Union[int, None] = None):
        ''' Send binary file. addr=None uses first two bytes as address.'''
        with open(name, 'rb') as f:
            if addr == None:
                data = f.read(2)
                addr = data[0] + data[1] * 256
            self.reset_vector_guess = addr
            while True:
                data = f.read(1024)
                if len(data) == 0:
                    break
                self.binary(addr, data)
                addr += len(data)

    def wait_for_prompt(self, prompt, timeout=DEFAULT_TIMEOUT):
        ''' Wait for prompt. '''
        prompt = bytes(prompt, 'utf-8')
        start = time.monotonic()
        while True:
            if len(prompt) == 1:
                data = self.serial.read()
            else:
                data = self.serial.read_until()
            if data[0:1] == b'?':
                monitor_result = data.decode('utf-8')
                monitor_result += self.serial.read_until().decode('utf-8').strip()
                raise RuntimeError(monitor_result)
            if data == prompt:
                break
            if len(data) == 0:
                if time.monotonic() - start > timeout:
                    raise TimeoutError()

    def basic_command(self, str, timeout=DEFAULT_TIMEOUT):
        ''' Send one line using "0\b" faux flow control. '''
        self.serial.write(b'0')
        start = time.monotonic()
        while True:
            r = self.serial.read()
            if r == b'\x00':  # huh, zeros?
                continue
            if r == b'0':
                break
            if time.monotonic() - start > timeout:
                raise TimeoutError()
        self.serial.write(b'\b')
        self.serial.write(bytes(str, 'utf-8'))
        self.serial.write(b'\r')
        self.serial.read_until()

    def basic_wait_for_ready(self, timeout=DEFAULT_TIMEOUT):
        ''' Wait for BASIC Ready. '''
        self.wait_for_prompt('Ready\r\n', timeout)


class ROM:
    def __init__(self):
        self.out = io.BytesIO(b'')
        # Shebang.
        self.out.write(b"#!RP6502\n")

    def seek(self, pos: int) -> int:
        return self.out.seek(pos)

    def read(self, size: Union[int, None]) -> bytes:
        return self.out.read(size)

    def comment(self, str):
        ''' Comments before binary data are used for help and info. '''
        self.out.write(b'# ')
        self.out.write(bytes(str, 'utf-8'))
        self.out.write(b'\n')

    def binary(self, addr, data):
        ''' Binary memory data. '''
        command = f'${addr:04X} ${len(data):03X} ${binascii.crc32(data):08X}\n'
        self.out.write(bytes(command, 'utf-8'))
        self.out.write(data)

    def binary_file(self, name, addr: Union[int, None] = None):
        ''' Binary memory data from file. addr=None uses first two bytes as address.'''
        with open(name, 'rb') as f:
            data = f.read()
        pos = 0
        if addr == None:
            pos += 2
            addr = data[0] + data[1] * 256
        self.reset_vector_guess = addr
        while pos < len(data):
            size = len(data) - pos
            if size > 1024:
                size = 1024
            self.binary(addr, data[pos:pos+size])
            addr += size
            pos += size

    def reset_vector(self, addr: Union[int, None] = None):
        ''' Set reset vector. Use start address of last file as default. '''
        if addr == None:
            addr = self.reset_vector_guess
        if addr == None:
            raise RuntimeError("Reset vector not set")
        self.binary(0xFFFC, bytearray([addr & 0xFF, addr >> 8]))


class RP6502:
    def __init__(self):
        self.help = []
        self.data = [0 for i in range(0x20000)]
        self.alloc = [0 for i in range(0x20000)]

    def add_help(self, string):
        if len(string) > 80:
            raise RuntimeError("Help line too long")
        self.help.append(string)
        if len(self.help) > 24:
            raise RuntimeError("Help lines > 24")

    def add_binary(self, data, addr: Union[int, None] = None):
        ''' Binary memory data. addr=None uses first two bytes as address.'''
        offset = 0
        length = len(data)
        if addr == None:
            if length < 2:
                raise ""
            offset += 2
            length -= 2
            addr = data[0] + data[1] * 256
        if (addr < 0x10000 and addr+length > 0x10000) or addr+length > 0x20000:
            raise RuntimeError("RP6502 data crossed memory boundary")
        for i in range(length):
            if self.alloc[addr + i]:
                raise RuntimeError(f"RP6502 data conflict ${addr+i:04X}")
            self.data[addr + i] = data[offset + i]
            self.alloc[addr + i] = 1

    def load_binary(self, name, addr: Union[int, None] = None):
        ''' Binary memory data from file. addr=None uses first two bytes as address.'''
        with open(name, 'rb') as f:
            data = f.read()
        self.add_binary(data, addr)

    def load_rp6502(self, name):
        with open(name, 'rb') as f:
            while(True):
                command = f.readline()
                if len(command)==0:
                    break
                se = re.search("[ ]*(# )", command)
                if (se):
                    self.add_help(command[se.start(1)+2:].rstrip())
                    continue
        print(f"Loaded ${name}")


# foo = RP6502()
# raise RuntimeError(f"Crossed memory boundary ${12:04X}")


def exec_args():
    parser = argparse.ArgumentParser(
        description='Control RP6502 RIA via UART.')
    parser.add_argument('command', choices=['run', 'create', 'upload'],
                        help='Run program. Create an RP6502 ROM. Upload a file.')
    parser.add_argument('filename',
                        help='Raw binary file. e.g. build/hello')
    parser.add_argument('-o', dest='out', metavar='name',
                        help='Destination path/filename.')
    parser.add_argument('-D', '--device', dest='device', metavar='dev',
                        default='/dev/ttyACM0',
                        help='Serial device name. Default=/dev/ttyACM0')
    parser.add_argument('-a', '--address', dest='address', metavar='addr',
                        help='Starting address of file. If not provided, '
                        'the first two bytes of the file are used.')
    parser.add_argument('-r', '--reset', dest='reset', metavar='addr',
                        help='Reset vector. If not provided, '
                        'the file starting address is used.')
    args = parser.parse_args()

    # Allow $ hex format
    def str_to_address(parser, str, errmsg):
        ''' Set reset vector. Use start address of last file as default. '''
        if (str):
            str = re.sub('^\$', '0x', str)
            if (re.match('^(0x|)[0-9A-Fa-f]*$', str)):
                return eval(str)
            else:
                parser.error("argument %s: invalid address: '%s'" %
                             (errmsg, str))
    args.address = str_to_address(parser, args.address, "-a/--address")
    args.reset = str_to_address(parser, args.reset, "-r/--reset")

    # Create an executable in RP6502 ROM format
    if (args.command == 'create'):
        rom = ROM()
        rom.binary_file(args.filename, args.address)
        rom.reset_vector(args.reset)
        with open(args.out, "wb") as o:
            rom.seek(0)
            while True:
                chunk = rom.read(1024)
                if len(chunk) == 0:
                    break
                o.write(chunk)
        exit()

    # Establish monitor session
    mon = Monitor(args.device)
    mon.send_break()

    # TODO make this work from a ROM
    if (args.command == 'run'):
        mon.send_file_to_memory(args.filename, args.address)
        mon.send_reset_vector(args.reset)
        mon.reset()

    # TODO make this a generic upload
    if (args.command == 'upload'):
        rom = ROM()
        rom.binary_file(args.filename, args.address)
        rom.reset_vector(args.reset)
        mon.upload(args.out or os.path.basename(args.filename), rom)


# This file may be included or run like a program. e.g.
#   import importlib
#   rp6502 = importlib.import_module("rp6502-sdk.rp6502")
if __name__ == "__main__":
    exec_args()
