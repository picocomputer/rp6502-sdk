#include "rp6502.h"
#include <errno.h>

int __fastcall__ close(int fd)
{
    return ria_call_ax(RIA_OP_CLOSE, fd);
}
