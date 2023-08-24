#include "rp6502.h"

int __fastcall__ read_rtc_time(void * buf)
{
    unsigned i;
    int ax, total = 0;
    RIA_OP = RIA_OP_READ_RTC;
    RIA_BLOCK();
    ax = RIA_AX;
    if (ax < 0)
        return _mappederrno(RIA_ERRNO_LO);
    for (i = 0; i < ax; i++)
        ((char *)buf)[total + i] = RIA_VSTACK;
    return ax;

}

int __fastcall__ write_rtc_time(const void *buf)
{
    unsigned i;
    int ax, total = 0;
    int block = 8;
    for (i = block; i;)
        RIA_VSTACK = ((char *)buf)[total + --i];
    RIA_OP = RIA_OP_WRITE_RTC;
    RIA_BLOCK();
    ax = RIA_AX;
    if (ax < 0)
        return _mappederrno(RIA_ERRNO_LO);
    return ax;
}
