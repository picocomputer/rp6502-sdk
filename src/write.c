/*
 * Copyright (c) 2023 Rumbledethumps
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-License-Identifier: Unlicense
 */

#include <rp6502.h>
#include <errno.h>

int __fastcall__ write(int fd, const void *buf, unsigned count)
{
    int ax, total = 0;
    while (count)
    {
        int blockcount = (count > 256) ? 256 : count;
        ax = write_xstack(&((char *)buf)[total], blockcount, fd);
        if (ax < 0)
            return ax;
        total += ax;
        count -= ax;
        if (ax < blockcount)
            break;
    }
    return total;
}

int __fastcall__ write_xstack(const void *buf, unsigned count, int fildes)
{
    unsigned i;
    for (i = count; i;)
        ria_push_char(((char *)buf)[--i]);
    ria_set_ax(fildes);
    return ria_call_int_errno(RIA_OP_WRITE_XSTACK);
}

int __fastcall__ write_xram(xram_addr buf, unsigned count, int fildes)
{
    ria_push_int(buf);
    ria_push_int(count);
    ria_set_ax(fildes);
    return ria_call_int_errno(RIA_OP_WRITE_XRAM);
}
