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
    ria_push_long(offset);
    ria_push_char(whence);
    ria_set_ax(fildes);
    return ria_call_long_errno(RIA_OP_LSEEK);
}

long __fastcall__ lseek16(int offset, char whence, int fildes)
{
    ria_push_int(offset);
    ria_push_char(whence);
    ria_set_ax(fildes);
    return ria_call_long_errno(RIA_OP_LSEEK);
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
