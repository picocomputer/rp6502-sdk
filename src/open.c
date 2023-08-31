/*
 * Copyright (c) 2023 Rumbledethumps
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-License-Identifier: Unlicense
 */

#include "rp6502.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stddef.h>

int open(const char *name, int flags, ...)
{
    size_t namelen, i;
    int ax;
    namelen = strlen(name);
    if (namelen > 255)
    {
        _seterrno(EINVAL);
        return -1;
    }
    for (i = namelen; i;)
        RIA.xstack = name[--i];
    RIA_CALL_AX(RIA_OP_OPEN, flags);
    RIA_BLOCK();
    ax = RIA_AX;
    if (ax < 0)
        _mappederrno(RIA.errno_lo);
    return RIA_AX;
}
