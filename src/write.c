/*
 * Copyright (c) 2023 Rumbledethumps
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-License-Identifier: Unlicense
 */

#include "rp6502.h"
#include <errno.h>

int __fastcall__ write(int fd, const void *buf, unsigned count)
{
    int ax, total = 0;
    while (count)
    {
        int blockcount = (count > 256) ? 256 : count;
        ax = write_(&((char *)buf)[total], blockcount, fd);
        if (ax < 0)
            return ax;
        total += ax;
        count -= ax;
        if (ax < blockcount)
            break;
    }
    return total;
}

int __fastcall__ write_(const void *buf, unsigned count, int fildes)
{
    unsigned i;
    for (i = count; i;)
        RIA.xstack = ((char *)buf)[--i];
    return ria_call_ax(RIA_OP_WRITE, fildes);
}

int __fastcall__ writex(xram_addr buf, unsigned count, int fildes)
{
    RIA.xstack = buf >> 8;
    RIA.xstack = buf & 0xFF;
    RIA.xstack = ((unsigned char *)&count)[1];
    RIA.xstack = ((unsigned char *)&count)[0];
    return ria_call_ax(RIA_OP_WRITEX, fildes);
}
