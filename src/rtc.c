#include "rp6502.h"
#include <errno.h>
#include <stdio.h>

int __fastcall__ get_rtc_time(datetime_t *datetime)
{
    uint8_t *ptr;
    unsigned i;
    int ax;
    uint8_t count = sizeof(*datetime);
    RIA_OP = RIA_OP_GET_RTC;
    RIA_BLOCK();
    ptr = (uint8_t *)datetime;
    ax = RIA_AX;
    if (ax < 0)
        return _mappederrno(RIA_ERRNO_LO);
    for (i = 0; i < count; i++)
        *ptr++ = RIA_VSTACK;
    return 0;

}

int __fastcall__ set_rtc_time(const datetime_t *datetime)
{
    const uint8_t *ptr;
    uint8_t i;
    int ax;
    uint8_t count = sizeof(*datetime);
    ptr = ((uint8_t *)datetime) + count - 1;
    for (i = 0;i < count; ++i)
        RIA_VSTACK = (uint8_t)(uint8_t *)*ptr--;
    RIA_CALL(RIA_OP_SET_RTC);
    RIA_BLOCK();
    ax = RIA_AX;
    if (ax < 0)
        return _mappederrno(RIA_ERRNO_LO);
    return 0;
}
