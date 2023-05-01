/*
 * Copyright (c) 2023 Rumbledethumps
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-License-Identifier: Unlicense
 */

#ifndef _RP6502_H_
#define _RP6502_H_

// RP6502 RIA $FFE0-$FFF9

#include <stdint.h>

// UART
#define RIA_TX_READY (*(uint8_t *)0xFFE0 & 0x80)
#define RIA_TX (*(uint8_t *)0xFFE1)
#define RIA_RX_READY (*(uint8_t *)0xFFE0 & 0x40)
#define RIA_RX (*(uint8_t *)0xFFE2)

// VRAM portal 0
#define RIA_RW0 (*(uint8_t *)0xFFE4)
#define RIA_STEP0 (*(int8_t *)0xFFE5)
#define RIA_ADDR0 (*(uint16_t *)0xFFE6)

// VRAM portal 1
#define RIA_RW1 (*(uint8_t *)0xFFE8)
#define RIA_STEP1 (*(int8_t *)0xFFE9)
#define RIA_ADDR1 (*(uint16_t *)0xFFEA)

// OS fastcall
#define RIA_ERRNO (*(uint16_t *)0xFFED)
#define RIA_ERRNO_LO (*(uint8_t *)0xFFED)
#define RIA_A (*(uint8_t *)0xFFF4)
#define RIA_X (*(uint8_t *)0xFFF6)
#define RIA_SREG (*(uint16_t *)0xFFF8)
#define RIA_AX (RIA_A | (RIA_X << 8))
#define RIA_AXSREG (RIA_AX | ((uint32_t)RIA_SREG << 16))
#define RIA_VSTACK (*(uint8_t *)0xFFEC)
#define RIA_OP (*(uint8_t *)0xFFEF)
#define RIA_BUSY (*(uint8_t *)0xFFF2 & 0x80)

// Call an OS function
#define RIA_CALL(op) \
    {                \
        RIA_OP = op; \
    }
#define RIA_CALL_A(op, a) \
    {                     \
        RIA_A = a;        \
        RIA_OP = op;      \
    }
#define RIA_CALL_AX(op, ax) \
    {                       \
        RIA_A = ax & 0xFF;  \
        RIA_X = ax >> 8;    \
        RIA_OP = op;        \
    }
#define RIA_CALL_AXSREG(op, axss) \
    {                             \
        RIA_A = axss & 0xFF;      \
        RIA_X = axss >> 8;        \
        RIA_SREG = axss >> 16;    \
        RIA_OP = op;              \
    }

// Block on completion of OS function.
#define RIA_BLOCK()  \
    while (RIA_BUSY) \
        ;

#define RIA_OP_EXIT 0xFF
#define RIA_OP_ZXSTACK 0x00
#define RIA_OP_OPEN 0x01
#define RIA_OP_CLOSE 0x04
#define RIA_OP_READ 0x05
#define RIA_OP_READV 0x06
#define RIA_OP_WRITE 0x08
#define RIA_OP_WRITEV 0x09
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
