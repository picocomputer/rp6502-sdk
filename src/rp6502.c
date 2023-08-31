/*
 * Copyright (c) 2023 Rumbledethumps
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-License-Identifier: Unlicense
 */

#include "rp6502.h"

void __fastcall__ xreg(unsigned value, unsigned reg, int devid)
{
    RIA.xstack = ((unsigned char *)&value)[1];
    RIA.xstack = ((unsigned char *)&value)[0];
    RIA.xstack = ((unsigned char *)&reg)[1];
    RIA.xstack = ((unsigned char *)&reg)[0];
    RIA_CALL_A(RIA_OP_XREG, devid);
    RIA_BLOCK();
}

unsigned __fastcall__ phi2(void)
{
    RIA_CALL(RIA_OP_PHI2);
    RIA_BLOCK();
    return RIA_AX;
}

unsigned __fastcall__ codepage(void)
{
    RIA_CALL(RIA_OP_CODEPAGE);
    RIA_BLOCK();
    return RIA_AX;
}

unsigned long __fastcall__ rand32(void)
{
    RIA_CALL(RIA_OP_RAND);
    RIA_BLOCK();
    return RIA_AXSREG;
}

unsigned __fastcall__ rand16(void)
{
    RIA_CALL(RIA_OP_RAND);
    RIA_BLOCK();
    return RIA_AX;
}
