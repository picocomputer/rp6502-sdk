#include "rp6502.h"
#include <errno.h>

int __fastcall__ close(int fd)
{
    RIA_CALL_AX(RIA_OP_CLOSE, fd);
    RIA_BLOCK();
    return _mappederrno(RIA_ERRNO_LO);
}
