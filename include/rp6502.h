/*
 * Copyright (c) 2023 Rumbledethumps
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-License-Identifier: Unlicense
 */

#ifndef _RP6502_H_
#define _RP6502_H_

/* RP6502 VIA $FFD0-$FFDF */

#include <_6522.h>
#define VIA (*(volatile struct __6522 *)0xFFD0)

/* RP6502 RIA $FFE0-$FFF9 */

struct __RP6502
{
    const unsigned char ready;
    unsigned char tx;
    const unsigned char rx;
    const unsigned char vsync;
    unsigned char rw0;
    unsigned char step0;
    unsigned int addr0;
    unsigned char rw1;
    unsigned char step1;
    unsigned int addr1;
    unsigned char xstack;
    unsigned char errno_lo;
    unsigned char errno_hi;
    unsigned char op;
    const unsigned char nop;
    const unsigned char wait;
    const unsigned char busy;
    const unsigned char ldx;
    unsigned char x;
    const unsigned char lda;
    unsigned char a;
    const unsigned char rts;
    unsigned int sreg;
};
#define RIA (*(volatile struct __RP6502 *)0xFFE0)

#define RIA_READY_TX_BIT 0x80
#define RIA_READY_RX_BIT 0x40
#define RIA_BUSY_BIT 0x80

/* XSTACK helpers */

void __fastcall__ ria_push_long(unsigned long val);
void __fastcall__ ria_push_int(unsigned int val);
#define ria_push_char(v) RIA.xstack = v

long __fastcall__ ria_pop_long(void);
int __fastcall__ ria_pop_int(void);
#define ria_pop_char() RIA.xstack

/* Set the RIA fastcall register */

void __fastcall__ ria_set_axsreg(unsigned long axsreg);
void __fastcall__ ria_set_ax(unsigned int ax);
#define ria_set_a(v) RIA.a = v

/* Run an OS operation */

int __fastcall__ ria_call_int(unsigned char op);
long __fastcall__ ria_call_long(unsigned char op);

/* These run _mappederrno() on error */

int __fastcall__ ria_call_int_errno(unsigned char op);
long __fastcall__ ria_call_long_errno(unsigned char op);

/* OS operation numbers */

#define RIA_OP_EXIT 0xFF
#define RIA_OP_ZXSTACK 0x00
#define RIA_OP_OPEN 0x01
#define RIA_OP_CLOSE 0x04
#define RIA_OP_READ 0x05
#define RIA_OP_READX 0x06
#define RIA_OP_WRITE 0x08
#define RIA_OP_WRITEX 0x09
#define RIA_OP_LSEEK 0x0B
#define RIA_OP_XREG 0x10
#define RIA_OP_PHI2 0x11
#define RIA_OP_CODEPAGE 0x12
#define RIA_OP_RAND 0x13

/* C API for the operating system.
 * The commented functions are declared in CC65.
 * The CC65 functions are very close to POSIX and
 * we try to make our API work directly with them.
 * It can't be done in some cases. For example, read_() from
 * the OS can only transfer 256 bytes on the xstack so read()
 * is a wrapper that breaks up large transfers.
 */

// int __cdecl__ open(const char *name, int flags, ...);
// int __fastcall__ close(int fd);
// int __fastcall__ read_(void *buf, unsigned count, int fildes);
// int __fastcall__ write(int fd, const void *buf, unsigned count);
// off_t __fastcall__ lseek(int fd, off_t offset, int whence);

typedef unsigned xram_addr;

int __fastcall__ read_(void *buf, unsigned count, int fildes);
int __fastcall__ readx(xram_addr buf, unsigned count, int fildes);
int __fastcall__ write_(const void *buf, unsigned count, int fildes);
int __fastcall__ writex(xram_addr buf, unsigned count, int fildes);
long __fastcall__ lseek32(long offset, char whence, int fildes);
long __fastcall__ lseek16(int offset, char whence, int fildes);
void __fastcall__ xreg(unsigned value, unsigned reg, int devid);
unsigned __fastcall__ phi2(void);
unsigned __fastcall__ codepage(void);
unsigned long __fastcall__ rand32(void);
unsigned __fastcall__ rand16(void);

#endif /* _RP6502_H_ */
