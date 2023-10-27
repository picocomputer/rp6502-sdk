/*
 * Copyright (c) 2023 Rumbledethumps
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-License-Identifier: Unlicense
 */

#include <rp6502.h>
#include <clock.h>

extern int _clock_gettimespec(struct timespec *ts, unsigned char op);

int clock_gettime(clockid_t clock_id, struct timespec *tp)
{
    (void)clock_id;
    // time.s doesn't set the stack value for clock_id
    ria_set_ax(CLOCK_REALTIME);
    return _clock_gettimespec(tp, RIA_OP_CLOCK_GETTIME);
}
