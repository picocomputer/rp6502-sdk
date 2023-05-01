#include "rp6502.h"
#include "errno.h"

int __fastcall__ read_(void *buf, unsigned count, int fildes)
{
    unsigned i;
    int ax, total = 0;
    while (count)
    {
        int block = (count > 256) ? 256 : count;
        RIA_VSTACK = ((unsigned char *)&block)[1];
        RIA_VSTACK = ((unsigned char *)&block)[0];
        RIA_CALL_AX(RIA_OP_READ, fildes);
        RIA_BLOCK();
        ax = RIA_AX;
        if (ax < 0)
            return _mappederrno(RIA_ERRNO_LO);
        for (i = 0; i < ax; i++)
            ((char *)buf)[total + i] = RIA_VSTACK;
        total += ax;
        count -= ax;
        if (ax < block)
            break;
    }
    return total;
}

int __fastcall__ read(int fd, void *buf, unsigned count)
{
    return read_(buf, count, fd);
}

int __fastcall__ readx(vram_ptr buf, unsigned count, int fildes)
{
    int ax;
    RIA_VSTACK = buf >> 8;
    RIA_VSTACK = buf & 0xFF;
    RIA_VSTACK = ((unsigned char *)&count)[1];
    RIA_VSTACK = ((unsigned char *)&count)[0];
    RIA_ADDR0 = buf;
    RIA_CALL_AX(RIA_OP_READV, fildes);
    RIA_BLOCK();
    ax = RIA_AX;
    if (ax < 0)
        return _mappederrno(RIA_ERRNO_LO);
    return RIA_AX;
}
