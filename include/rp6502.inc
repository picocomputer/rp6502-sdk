; Copyright (c) 2023 Rumbledethumps
;
; SPDX-License-Identifier: BSD-3-Clause
; SPDX-License-Identifier: Unlicense

; 6502 Vectors
IRQVec          := $FFFA
BRKVec          := $FFFC
NMIVec          := $FFFE

; VSYNC from PIX VGA
RIA_VSYNC       := $FFE3

; RIA UART
RIA_READY       := $FFE0 ; TX=$80 RX=$40
RIA_TX          := $FFE1
RIA_RX          := $FFE2

; RIA VRAM portal 0
RIA_RW0         := $FFE4
RIA_STEP0       := $FFE5
RIA_ADDR0       := $FFE6

; RIA VRAM portal 1
RIA_RW1         := $FFE8
RIA_STEP1       := $FFE9
RIA_ADDR1       := $FFEA

; RIA OS fastcall
RIA_ERRNO       := $FFED
RIA_A           := $FFF6
RIA_X           := $FFF4
RIA_SREG        := $FFF8
RIA_VSTACK      := $FFEC
RIA_OP          := $FFEF
RIA_BUSY        := $FFF2 ; Bit $80

; 6522 VIA
VIA             := $FFD0        ; VIA base address
VIA_PB          := VIA+$0       ; Port register B
VIA_PA1         := VIA+$1       ; Port register A
VIA_PRB         := VIA+$0       ; *** Deprecated ***
VIA_PRA         := VIA+$1       ; *** Deprecated ***
VIA_DDRB        := VIA+$2       ; Data direction register B
VIA_DDRA        := VIA+$3       ; Data direction register A
VIA_T1CL        := VIA+$4       ; Timer 1, low byte
VIA_T1CH        := VIA+$5       ; Timer 1, high byte
VIA_T1LL        := VIA+$6       ; Timer 1 latch, low byte
VIA_T1LH        := VIA+$7       ; Timer 1 latch, high byte
VIA_T2CL        := VIA+$8       ; Timer 2, low byte
VIA_T2CH        := VIA+$9       ; Timer 2, high byte
VIA_SR          := VIA+$A       ; Shift register
VIA_CR          := VIA+$B       ; Auxiliary control register
VIA_PCR         := VIA+$C       ; Peripheral control register
VIA_IFR         := VIA+$D       ; Interrupt flag register
VIA_IER         := VIA+$E       ; Interrupt enable register
VIA_PA2         := VIA+$F       ; Port register A w/o handshake
