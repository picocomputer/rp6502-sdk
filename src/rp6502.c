/*
 * Copyright (c) 2023 Rumbledethumps
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-License-Identifier: Unlicense
 */

#include <rp6502.h>

void __fastcall__ xreg(unsigned value, unsigned reg, int devid)
{
    ria_push_int(value);
    ria_push_int(reg);
    ria_set_a(devid);
    ria_call_int(RIA_OP_XREG);
}

unsigned __fastcall__ phi2(void)
{
    return ria_call_int(RIA_OP_PHI2);
}

unsigned __fastcall__ codepage(void)
{
    return ria_call_int(RIA_OP_CODEPAGE);
}

unsigned long __fastcall__ rand32(void)
{
    return ria_call_long(RIA_OP_RAND);
}

unsigned __fastcall__ rand16(void)
{
    return ria_call_int(RIA_OP_RAND);
}
