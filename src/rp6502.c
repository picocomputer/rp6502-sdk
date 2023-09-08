/*
 * Copyright (c) 2023 Rumbledethumps
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-License-Identifier: Unlicense
 */

#include <rp6502.h>

unsigned __fastcall__ ria_phi2(void)
{
    return ria_call_int(RIA_OP_PHI2);
}

unsigned __fastcall__ ria_codepage(void)
{
    return ria_call_int(RIA_OP_CODEPAGE);
}
