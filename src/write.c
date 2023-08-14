/*
 * Copyright (c) 2023 Rumbledethumps
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-License-Identifier: Unlicense
 */

#include "rp6502.h"
#include <errno.h>

int __fastcall__ write_(const void *buf, unsigned count, int fildes)
{
    unsigned i;
    int ax, total = 0;
    while (count)
    {
        int block = (count > 256) ? 256 : count;
        for (i = block; i;)
            RIA_VSTACK = ((char *)buf)[total + --i];
        RIA_CALL_AX(RIA_OP_WRITE, fildes);
        RIA_BLOCK();
        ax = RIA_AX;
        if (ax < 0)
            return _mappederrno(RIA_ERRNO_LO);
        total += ax;
        count -= ax;
        if (ax < block)
            break;
    }
    return total;
}

int __fastcall__ writex(vram_ptr buf, unsigned count, int fildes)
{
    int ax;
    RIA_VSTACK = buf >> 8;
    RIA_VSTACK = buf & 0xFF;
    RIA_VSTACK = ((unsigned char *)&count)[1];
    RIA_VSTACK = ((unsigned char *)&count)[0];
    RIA_CALL_AX(RIA_OP_WRITEV, fildes);
    RIA_BLOCK();
    ax = RIA_AX;
    if (ax < 0)
        return _mappederrno(RIA_ERRNO_LO);
    return RIA_AX;
}

int __fastcall__ write(int fd, const void *buf, unsigned count)
{
    return write_(buf, count, fd);
}
