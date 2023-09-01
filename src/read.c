#include <rp6502.h>
#include <errno.h>

int __fastcall__ read(int fd, void *buf, unsigned count)
{
    int ax, total = 0;
    while (count)
    {
        int blockcount = (count > 256) ? 256 : count;
        ax = read_(&((char *)buf)[total], blockcount, fd);
        if (ax < 0)
            return ax;
        total += ax;
        count -= ax;
        if (ax < blockcount)
            break;
    }
    return total;
}

int __fastcall__ read_(void *buf, unsigned count, int fildes)
{
    int i, ax;
    ria_push_int(count);
    ria_set_ax(fildes);
    ax = ria_call_int_errno(RIA_OP_READ);
    for (i = 0; i < ax; i++)
        ((char *)buf)[i] = ria_pop_char();
    return ax;
}

int __fastcall__ readx(xram_addr buf, unsigned count, int fildes)
{
    ria_push_int(buf);
    ria_push_int(count);
    ria_set_ax(fildes);
    return ria_call_int_errno(RIA_OP_READX);
}
