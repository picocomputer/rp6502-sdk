/*
 * Copyright (c) 2023 Rumbledethumps
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-License-Identifier: Unlicense
 */

#include <rp6502.h>

void __fastcall__ xreg(unsigned value, unsigned reg, int devid)
{
    RIA.xstack = ((unsigned char *)&value)[1];
    RIA.xstack = ((unsigned char *)&value)[0];
    RIA.xstack = ((unsigned char *)&reg)[1];
    RIA.xstack = ((unsigned char *)&reg)[0];
    ria_call_a(RIA_OP_XREG, devid);
}

unsigned __fastcall__ phi2(void)
{
    return ria_call(RIA_OP_PHI2);
}

unsigned __fastcall__ codepage(void)
{
    return ria_call(RIA_OP_CODEPAGE);
}

unsigned long __fastcall__ rand32(void)
{
    return ria_long(RIA_OP_RAND);
}

unsigned __fastcall__ rand16(void)
{
    return ria_call(RIA_OP_RAND);
}
