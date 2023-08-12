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


def exec_args():
    parser = argparse.ArgumentParser(
        description='Control RP6502 RIA via UART.')
    parser.add_argument('command', choices=['run', 'upload'],
                        help='Run sends to memory. Upload writes a ROM file.')
    parser.add_argument('filename',
                        help='Raw binary file. e.g. build/hello')
    parser.add_argument('-o', dest='out', metavar='name',
                        help='Upload destination path/filename.')
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

    # Allow $ hex format.
    if (args.address):
        address = re.sub('^\$', '0x', args.address)
        if (re.match('^($|0x|)[0-9A-Fa-f]*$', address)):
            address = eval(address)
        else:
            parser.error(
                "argument -a/--address: invalid value: '%s'" % args.address)
        args.address = address

    mon = Monitor(args.device)
    mon.send_break()

    if (args.command == 'run'):
        mon.send_file_to_memory(args.filename, args.address)
        mon.send_reset_vector(args.reset)
        mon.reset()

    # TODO Support including a comment file
    if (args.command == 'upload'):
        rom = ROM()
        rom.binary_file(args.filename, args.address)
        rom.reset_vector(args.reset)
        mon.upload(args.out or os.path.basename(args.filename), rom)


# This file may be included or run like a program.
if __name__ == "__main__":
    exec_args()
