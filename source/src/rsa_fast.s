; Bytewise CIOS Montgomery multiply/add row, 512 bytes.
; Called synchronously; IRQs remain enabled and never call this routine.
; Writable resident code patches absolute indexed operands once per row.
; ZP $A4-$A9 is disjoint from cc65/crypto $80-$99, as in the old row.
.export _rsa_fastrow
.import _rsa_row_a,_rsa_row_t,_rsa_row_b,_rsa_row_shift,_rsa_row_carry
.import mul_dma_lo,mul_dma_hi
carry=$a4
low=$a5
tp=$a6
ap=$a8
.segment "CLIENT_BSS"
pages:.res 1
.segment "RSA_ROW_CODE"
_rsa_fastrow:
 lda _rsa_row_a
 sta ap
 sta source+1
 lda _rsa_row_a+1
 sta ap+1
 sta source+2
 lda _rsa_row_t
 sta tp
 sta accum+1
 sec
 sbc _rsa_row_shift
 sta dest+1
 lda _rsa_row_t+1
 sta tp+1
 sta accum+2
 sbc #0
 sta dest+2

 ; X holds the high product byte. The low byte is read back from its
 ; table entry, avoiding memory counters in the 256-entry table loop.
 lda #0
 tax
 tay
table:
 sta mul_dma_lo,y
 txa
 sta mul_dma_hi,y
 lda mul_dma_lo,y
 clc
 adc _rsa_row_b
 bcc :+
 inx
:
 iny
 bne table
 lda #2
 sta pages
 lda #0
 sta carry
 lda _rsa_row_shift
 beq loop
 ; Shifted reduction discards byte zero; never write before tmp.
 lda (ap),y
 tax
 clc
 lda mul_dma_lo,x
 adc (tp),y
 lda mul_dma_hi,x
 adc #0
 sta carry
 iny
loop:
source:
 lda $ffff,y
 tax
 clc
 lda mul_dma_lo,x
 adc carry
 sta low
 lda mul_dma_hi,x
 adc #0
 tax
 clc
accum:
 lda $ffff,y
 adc low
dest:
 sta $ffff,y
 ; Propagate carry without a data-dependent branch in the inner loop.
 txa
 adc #0
 sta carry
 iny
 bne loop
 inc source+2
 inc accum+2
 inc dest+2
 dec pages
 bne loop
 lda carry
 sta _rsa_row_carry
 rts
