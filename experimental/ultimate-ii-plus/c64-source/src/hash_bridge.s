.export _crypto_hash
.import _hash_ptr, _hash_len, _hash_out
.import sha256_init, sha256_process_block, sha256_final
.import sha256_block, sha256_hash
.segment "BSS"
hash_bits: .res 2
hash_remaining: .res 2
hash_cursor: .res 2
hash_tail: .res 1
.segment "CODE"
_crypto_hash:
    lda _hash_ptr
    sta hash_cursor
    lda _hash_ptr+1
    sta hash_cursor+1
    lda _hash_len
    sta hash_remaining
    sta hash_bits
    lda _hash_len+1
    sta hash_remaining+1
    sta hash_bits+1
    asl hash_bits
    rol hash_bits+1
    asl hash_bits
    rol hash_bits+1
    asl hash_bits
    rol hash_bits+1
    jsr sha256_init
full:
    lda hash_remaining+1
    bne copy_full
    lda hash_remaining
    cmp #64
    bcc tail
copy_full:
    lda hash_cursor
    sta $fb
    lda hash_cursor+1
    sta $fc
    ldy #0
:
    lda ($fb),y
    sta sha256_block,y
    iny
    cpy #64
    bne :-
    jsr sha256_process_block
    clc
    lda hash_cursor
    adc #64
    sta hash_cursor
    bcc :+
    inc hash_cursor+1
:
    sec
    lda hash_remaining
    sbc #64
    sta hash_remaining
    lda hash_remaining+1
    sbc #0
    sta hash_remaining+1
    jmp full
tail:
    sta hash_tail
    jsr clear
    lda hash_cursor
    sta $fb
    lda hash_cursor+1
    sta $fc
    ldy #0
:
    cpy hash_tail
    beq pad
    lda ($fb),y
    sta sha256_block,y
    iny
    bne :-
pad:
    lda #128
    sta sha256_block,y
    cpy #56
    bcc finish
    jsr sha256_process_block
    jsr clear
finish:
    lda hash_bits+1
    sta sha256_block+62
    lda hash_bits
    sta sha256_block+63
    jsr sha256_process_block
    jsr sha256_final
    ldx #31
:
    lda sha256_hash,x
    sta _hash_out,x
    dex
    bpl :-
    rts
clear:
    ldx #63
    lda #0
:
    sta sha256_block,x
    dex
    bpl :-
    rts
