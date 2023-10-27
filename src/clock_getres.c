/*
 * Copyright (c) 2023 Rumbledethumps
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-License-Identifier: Unlicense
 */

#include <rp6502.h>
#include <clock.h>

extern int _clock_gettimespec(struct timespec *ts, unsigned char op);

int clock_getres(clockid_t clock_id, struct timespec *res)
{
    ria_set_ax(clock_id);
    return _clock_gettimespec(res, RIA_OP_CLOCK_GETRES);
}
