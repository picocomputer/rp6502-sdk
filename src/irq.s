; Copyright (c) 2023 Rumbledethumps
;
; SPDX-License-Identifier: BSD-3-Clause
; SPDX-License-Identifier: Unlicense

.export initirq, doneirq
.import callirq, _exit

.include "rp6502.inc"

.segment "ONCE"

initirq:
    LDA #<handler
    LDX #>handler
    SEI
    STA $FFFE
    STX $FFFF
    CLI
    RTS

.code

doneirq:
    SEI
    RTS

.segment "LOWCODE"

handler:
    CLD
    PHX
    TSX
    PHA
    INX
    INX
    LDA $100,X
    AND #$10
    BNE break
    PHY
    JSR callirq
    PLY
    PLA
    PLX
    RTI

break:
    LDA #$FF
    STA RIA_A
    JMP _exit
