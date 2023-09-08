; Copyright (c) 2023 Rumbledethumps
;
; SPDX-License-Identifier: BSD-3-Clause
; SPDX-License-Identifier: Unlicense

; CC65 will promote variadic char arguments to int. It will not demote longs.
; int __cdecl__ ria_xreg(char device, char channel, unsigned char address, ...);

.include        "rp6502.inc"

.export		_ria_xreg
.importzp	sp
.import		addysp, _ria_call_int_errno

.code

.proc	_ria_xreg

        ; save variadic size in X
        tya
        tax

        ; copy stack
@copy:  dey
        lda (sp),y
        sta RIA_XSTACK
        tya
        bne @copy

        ; recover variadic size and move sp
        txa
        tay
        jsr addysp

        ; run RIA operation
        lda #RIA_OP_XREG
        jmp _ria_call_int_errno

.endproc
