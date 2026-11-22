.include "constants.inc"
.export start, print_string, print_null_terminated
.export ch_sent_msg,sh_recv_msg,hk1_msg,keys_ok_msg,ee_recv_msg,cert_recv_msg
.export cv_recv_msg,fin_recv_msg,cfin_sent_msg,enc1_msg,rx_msg,got_msg,got2_msg,dec_msg,proc_msg
.importzp c_sp
.import _clientmain,entropy_init,drbg_collect_seed,hmac_drbg_instantiate,drbg_buf_idx
.export mul_tables_init
mul_tables_init=print_string
.segment "EXEHDR"
.word end_basic,10
.byte $9e,"2061",0
end_basic:.word 0
 jmp start
.segment "CODE"
start:
 ; Show a message before the slow entropy/DRBG initialization on a 6510.
 lda #0
 sta classic_boot_index
classic_boot_print:
 ldx classic_boot_index
 lda classic_boot_text,x
 beq classic_boot_done
 jsr $ffd2
 inc classic_boot_index
 bne classic_boot_print
classic_boot_done:
 lda #$36
 sta $01
 sei
 lda #0
 ldy #0
 ldx #32
zero:
 sta $a000,y
 iny
 bne zero
 inc zero+2
 dex
 bne zero
 lda #<$a000
 sta c_sp
 lda #>$a000
 sta c_sp+1
 cli
 jsr entropy_init
 ; Preserve the original hardware-sampling cadence at normal CPU speed.
 ; Only the subsequent deterministic SHA/HMAC work is accelerated.
 jsr drbg_collect_seed
 jsr hmac_drbg_instantiate
 lda #32
 sta drbg_buf_idx

 jsr _clientmain
 lda #$37
 sta $01
 jmp $fce2
classic_boot_index: .byte 0
.segment "RODATA"
classic_boot_text:
 .byte 147,"K.R.S. C64 + ULTIMATE-II+",13,13
 .byte "TLS-MODUL - BITTE WARTEN",13
 .byte "ZUFALLSQUELLE WIRD VORBEREITET",13,0
.segment "CODE"
; Handshake progress strings are deliberately empty in the application UI.
print_string:
print_null_terminated:
 rts
.segment "RODATA"
ch_sent_msg:
sh_recv_msg:
hk1_msg:
keys_ok_msg:
ee_recv_msg:
cert_recv_msg:
cv_recv_msg:
fin_recv_msg:
cfin_sent_msg:
enc1_msg:
rx_msg:
got_msg:
got2_msg:
dec_msg:
proc_msg:.byte 0
