/*
 * Copyright (c) 2023 Rumbledethumps
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-License-Identifier: Unlicense
 */

#ifndef _RP6502_H_
#define _RP6502_H_

#include <stdint.h>

// RP6502 VIA $FFD0-$FFDF
#include <_6522.h>
#define VIA (*(volatile struct __6522 *)0xFFD0)

// RP6502 RIA $FFE0-$FFF9
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

#define RIA_AX (RIA.a | (RIA.x << 8))
#define RIA_AXSREG (RIA_AX | ((uint32_t)RIA.sreg << 16))
// #define RIA_XSTACK (*(uint8_t *)0xFFEC)

// Call an OS function
#define RIA_CALL(OP) \
    {                \
        RIA.op = OP; \
    }
#define RIA_CALL_A(OP, A) \
    {                     \
        RIA.a = A;        \
        RIA.op = OP;      \
    }
#define RIA_CALL_AX(OP, AX) \
    {                       \
        RIA.a = AX & 0xFF;  \
        RIA.x = AX >> 8;    \
        RIA.op = OP;        \
    }
#define RIA_CALL_AXSREG(OP, AXSREG) \
    {                               \
        RIA.a = AXSREG & 0xFF;      \
        RIA.x = AXSREG >> 8;        \
        RIA.sreg = AXSREG >> 16;    \
        RIA.op = OP;                \
    }

// Block on completion of OS function.
#define RIA_BLOCK()                 \
    while (RIA.busy & RIA_BUSY_BIT) \
        ;

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

typedef uint16_t vram_ptr;

int __fastcall__ read_(void *buf, unsigned count, int fildes);
int __fastcall__ readx(vram_ptr buf, unsigned count, int fildes);
int __fastcall__ write_(const void *buf, unsigned count, int fildes);
int __fastcall__ writex(vram_ptr buf, unsigned count, int fildes);
long __fastcall__ lseek32(long offset, char whence, int fildes);
long __fastcall__ lseek16(int offset, char whence, int fildes);
void __fastcall__ xreg(unsigned value, unsigned reg, int devid);
unsigned __fastcall__ phi2(void);
unsigned __fastcall__ codepage(void);
unsigned long __fastcall__ rand32(void);
unsigned __fastcall__ rand16(void);

#endif /* _RP6502_H_ */
