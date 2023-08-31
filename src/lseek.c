/*
 * Copyright (c) 2023 Rumbledethumps
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-License-Identifier: Unlicense
 */

#include "rp6502.h"
#include <unistd.h>
#include <stdio.h>
#include "errno.h"

long __fastcall__ lseek32(long offset, char whence, int fildes)
{
    long axsreg;
    RIA.xstack = ((unsigned char *)&offset)[3];
    RIA.xstack = ((unsigned char *)&offset)[2];
    RIA.xstack = ((unsigned char *)&offset)[1];
    RIA.xstack = ((unsigned char *)&offset)[0];
    RIA.xstack = whence;
    RIA_CALL_AX(RIA_OP_LSEEK, fildes);
    RIA_BLOCK();
    axsreg = RIA_AXSREG;
    if (axsreg < 0)
        return _mappederrno(RIA.errno_lo);
    return axsreg;
}

long __fastcall__ lseek16(int offset, char whence, int fildes)
{
    long axsreg;
    RIA.xstack = ((unsigned char *)&offset)[1];
    RIA.xstack = ((unsigned char *)&offset)[0];
    RIA.xstack = whence;
    RIA_CALL_AX(RIA_OP_LSEEK, fildes);
    RIA_BLOCK();
    axsreg = RIA_AXSREG;
    if (axsreg < 0)
        return _mappederrno(RIA.errno_lo);
    return axsreg;
}

off_t __fastcall__ lseek(int fd, off_t offset, int whence)
{
    // Change CC65 whence to what everyone else uses
    switch (whence)
    {
    case SEEK_SET:
        whence = 0;
        break;
    case SEEK_CUR:
        whence = 1;
        break;
    case SEEK_END:
        whence = 2;
        break;
    }
    return lseek32(offset, whence, fd);
}
