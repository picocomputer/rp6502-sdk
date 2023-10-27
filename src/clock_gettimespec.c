/*
 * Copyright (c) 2023 Rumbledethumps
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-License-Identifier: Unlicense
 */

#include <rp6502.h>
#include <clock.h>

// Internal method shared by clock_getres and clock_gettime.
int _clock_gettimespec(struct timespec *ts, unsigned char op)
{
    int ax = ria_call_int_errno(op);
    if (ax >= 0)
    {
        ts->tv_sec = ria_pop_long();
        ts->tv_nsec = ria_pop_long();
    }
    return ax;
}
