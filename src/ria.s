; Copyright (c) 2023 Rumbledethumps
;
; SPDX-License-Identifier: BSD-3-Clause
; SPDX-License-Identifier: Unlicense

.include        "rp6502.inc"

.export		_ria_call, _ria_call_a, _ria_call_ax, _ria_call_axsreg
.export		_ria_long, _ria_long_a, _ria_long_ax, _ria_long_axsreg
.importzp	sp, sreg
.import		__mappederrno, incsp1

.code

; long __fastcall__ ria_long(unsigned char op);
_ria_long:
        sta RIA_OP
        jmp WAITS

; long __fastcall__ ria_long_axsreg(unsigned char op, unsigned long axsreg);
_ria_long_axsreg:
        ldy RIA_SREG
        sty sreg
        ldy RIA_SREG+1
        sty sreg+1

; long __fastcall__ ria_long_ax(unsigned char op, unsigned int ax);
_ria_long_ax:
        stx RIA_X

; long __fastcall__ ria_long_a(unsigned char op, unsigned char a);
_ria_long_a:
        sta RIA_A
        lda (sp)
        sta RIA_OP
        jsr incsp1

WAITS:  jsr RIA_WAIT
        ldy RIA_SREG+1
        jmp END

; int __fastcall__ ria_call(unsigned char op);
_ria_call:
        sta RIA_OP
        jmp WAITA

; int __fastcall__ ria_call_axsreg(unsigned char op, unsigned long axsreg);
_ria_call_axsreg:
        ldy RIA_SREG
        sty sreg
        ldy RIA_SREG+1
        sty sreg+1

; int __fastcall__ ria_call_ax(unsigned char op, unsigned int ax);
_ria_call_ax:
        stx RIA_X

; int __fastcall__ ria_call_a(unsigned char op, unsigned char a);
_ria_call_a:
        sta RIA_A
        lda (sp)
        sta RIA_OP
        jsr incsp1

WAITA:  jsr RIA_WAIT
        ldy RIA_X

END:    bpl DONE
        lda RIA_ERRNO
        jmp __mappederrno
DONE:   rts
