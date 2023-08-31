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

int __cdecl__ open(const char *name, int flags, ...)
{
    size_t namelen, i;
    namelen = strlen(name);
    if (namelen > 255)
    {
        _seterrno(EINVAL);
        return -1;
    }
    for (i = namelen; i;)
        RIA.xstack = name[--i];
    return ria_call_ax(RIA_OP_OPEN, flags);
}
