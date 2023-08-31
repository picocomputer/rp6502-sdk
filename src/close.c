#include "rp6502.h"
#include <errno.h>

int __fastcall__ close(int fd)
{
    RIA_CALL_AX(RIA_OP_CLOSE, fd);
    RIA_BLOCK();
    if (!RIA.a)
        return 0;
    return _mappederrno(RIA.errno_lo);
}
