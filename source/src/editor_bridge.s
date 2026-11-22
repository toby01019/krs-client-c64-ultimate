.export _editor_load,_editor_store,_editor_screen_save,_editor_screen_restore
.import _editor_buf,cert_buf,reu_execute,_term_view
.segment "CLIENT_BSS"
old_view:.res 1
.segment "CODE"
_editor_load:
 ldx #$91
 bne line_dma
_editor_store:
 ldx #$90
line_dma:
 pha
 txa
 pha
 lda #<_editor_buf
 sta $df02
 lda #>_editor_buf
 sta $df03
 pla
 tax
 pla
 lsr
 sta $df05
 lda #0
 ror
 sta $df04
 lda #1
 sta $df06
 lda #80
 sta $df07
 lda #0
 sta $df08
 sta $df09
 sta $df0a
 txa
 jmp reu_execute
_editor_screen_save:
 lda _term_view
 sta old_view
 lda #$90
 bne screen_dma
_editor_screen_restore:
 lda old_view
 sta _term_view
 lda #$91
screen_dma:
 pha
 lda #<cert_buf
 sta $df02
 lda #>cert_buf
 sta $df03
 lda #0
 sta $df04
 sta $df05
 sta $df09
 sta $df0a
 lda #2
 sta $df06
 lda #$d0
 sta $df07
 lda #7
 sta $df08
 pla
 jmp reu_execute
