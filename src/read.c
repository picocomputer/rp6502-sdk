#include <rp6502.h>
#include <errno.h>

int __fastcall__ read_(void *buf, unsigned count, int fildes)
{
    int i, ax;
    RIA.xstack = ((unsigned char *)&count)[1];
    RIA.xstack = ((unsigned char *)&count)[0];
    RIA_CALL_AX(RIA_OP_READ, fildes);
    RIA_BLOCK();
    ax = RIA_AX;
    if (ax < 0)
        return _mappederrno(RIA.errno_lo);
    for (i = 0; i < ax; i++)
        ((char *)buf)[i] = RIA.xstack;
    return ax;
}

int __fastcall__ read(int fd, void *buf, unsigned count)
{
    int ax, total = 0;
    while (count)
    {
        int blockcount = (count > 256) ? 256 : count;
        ax = read_(&((char *)buf)[total], blockcount, fd);
        if (ax < 0)
            return _mappederrno(RIA.errno_lo);
        total += ax;
        count -= ax;
        if (ax < blockcount)
            break;
    }
    return total;
}

int __fastcall__ readx(vram_ptr buf, unsigned count, int fildes)
{
    int ax;
    RIA.xstack = buf >> 8;
    RIA.xstack = buf & 0xFF;
    RIA.xstack = ((unsigned char *)&count)[1];
    RIA.xstack = ((unsigned char *)&count)[0];
    RIA_CALL_AX(RIA_OP_READX, fildes);
    RIA_BLOCK();
    ax = RIA_AX;
    if (ax < 0)
        return _mappederrno(RIA.errno_lo);
    return RIA_AX;
}
