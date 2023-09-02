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
    const unsigned char spin;
    const unsigned char busy;
    const unsigned char lda;
    unsigned char a;
    const unsigned char ldx;
    unsigned char x;
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
#define RIA_OP_READ_XSTACK 0x05
#define RIA_OP_READ_XRAM 0x06
#define RIA_OP_WRITE_XSTACK 0x08
#define RIA_OP_WRITE_XRAM 0x09
#define RIA_OP_LSEEK 0x0B
#define RIA_OP_XREG 0x10
#define RIA_OP_PHI2 0x11
#define RIA_OP_CODEPAGE 0x12
#define RIA_OP_RAND 0x13

/* C API for the operating system.
 * Commented functions are declared in CC65.
 */

// int __cdecl__ open(const char *name, int flags, ...);
// int __fastcall__ close(int fd);
// int __fastcall__ read(int fd, void *buf, unsigned count)
// int __fastcall__ write(int fd, const void *buf, unsigned count);
// off_t __fastcall__ lseek(int fd, off_t offset, int whence);

typedef unsigned xram_addr;

int __fastcall__ read_xstack(void *buf, unsigned count, int fildes);
int __fastcall__ read_xram(xram_addr buf, unsigned count, int fildes);
int __fastcall__ write_xstack(const void *buf, unsigned count, int fildes);
int __fastcall__ write_xram(xram_addr buf, unsigned count, int fildes);
void __fastcall__ ria_xreg(unsigned value, unsigned reg, int devid);
unsigned __fastcall__ ria_phi2(void);
unsigned __fastcall__ ria_codepage(void);
unsigned long __fastcall__ ria_lrand(void);
unsigned __fastcall__ ria_rand(void);

/* Values in __oserror are the union of these FatFs errors and errno.h */
typedef enum
{
    FR_OK = 32,             /* Succeeded */
    FR_DISK_ERR,            /* A hard error occurred in the low level disk I/O layer */
    FR_INT_ERR,             /* Assertion failed */
    FR_NOT_READY,           /* The physical drive cannot work */
    FR_NO_FILE,             /* Could not find the file */
    FR_NO_PATH,             /* Could not find the path */
    FR_INVALID_NAME,        /* The path name format is invalid */
    FR_DENIED,              /* Access denied due to prohibited access or directory full */
    FR_EXIST,               /* Access denied due to prohibited access */
    FR_INVALID_OBJECT,      /* The file/directory object is invalid */
    FR_WRITE_PROTECTED,     /* The physical drive is write protected */
    FR_INVALID_DRIVE,       /* The logical drive number is invalid */
    FR_NOT_ENABLED,         /* The volume has no work area */
    FR_NO_FILESYSTEM,       /* There is no valid FAT volume */
    FR_MKFS_ABORTED,        /* The f_mkfs() aborted due to any problem */
    FR_TIMEOUT,             /* Could not get a grant to access the volume within defined period */
    FR_LOCKED,              /* The operation is rejected according to the file sharing policy */
    FR_NOT_ENOUGH_CORE,     /* LFN working buffer could not be allocated */
    FR_TOO_MANY_OPEN_FILES, /* Number of open files > FF_FS_LOCK */
    FR_INVALID_PARAMETER    /* Given parameter is invalid */
} FRESULT;

#endif /* _RP6502_H_ */
