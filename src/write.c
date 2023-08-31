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
            RIA.xstack = ((char *)buf)[total + --i];
        RIA_CALL_AX(RIA_OP_WRITE, fildes);
        RIA_BLOCK();
        ax = RIA_AX;
        if (ax < 0)
            return _mappederrno(RIA.errno_lo);
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
    RIA.xstack = buf >> 8;
    RIA.xstack = buf & 0xFF;
    RIA.xstack = ((unsigned char *)&count)[1];
    RIA.xstack = ((unsigned char *)&count)[0];
    RIA_CALL_AX(RIA_OP_WRITEX, fildes);
    RIA_BLOCK();
    ax = RIA_AX;
    if (ax < 0)
        return _mappederrno(RIA.errno_lo);
    return RIA_AX;
}

int __fastcall__ write(int fd, const void *buf, unsigned count)
{
    return write_(buf, count, fd);
}
