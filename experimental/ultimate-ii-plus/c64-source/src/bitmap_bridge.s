.export _bitmap_begin,_bitmap_end,_font80
.import soft80_charset
_font80=soft80_charset
.segment "CLIENT_BSS"
saved_status:.res 1
.segment "CODE"
_bitmap_begin:
 php
 pla
 sta saved_status
 sei
 lda #$34
 sta $01
 rts
_bitmap_end:
 lda #$36
 sta $01
 lda saved_status
 pha
 plp
 rts
