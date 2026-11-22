; RSA-PSS-RSAE-SHA256 (0x0804), pinned 4096-bit leaf key.
.include "constants.inc"
.export tls_handle_certificate,tls_handle_cert_verify
.export tls_cert_stream_finish,tls_hs_ptr_reset,x509_extract_pubkey
.import cert_buf,cert_buf_len,tls_rec_buf,tls_transcript
.import _rsa_pin_validate,_rsa_verify,_rsa_result
.import _rsa_signature,_rsa_signature_len,_rsa_message_hash
.import sha256_init,sha256_process_block,sha256_final,sha256_block,sha256_hash
.import tls_recv_sub_progress
.segment "CRYPTO_CODE"
tls_hs_ptr_reset:
 lda #<tls_rec_buf
 sta tls_hs_ptr
 lda #>tls_rec_buf
 sta tls_hs_ptr+1
 rts
tls_cert_stream_finish:
 sta cert_buf_len
 stx cert_buf_len+1
x509_extract_pubkey:
 jsr _rsa_pin_validate
 cmp #1
 beq :+
 sec
 rts
:
 clc
 rts
tls_handle_certificate:
 ; This build requires the bounded streaming certificate deframer.
 sec
 rts
tls_handle_cert_verify:
 lda #$20
 sta tls_recv_sub_progress
 ldy #0
 lda (tls_hs_ptr),y
 cmp #15
 beq :+
 jmp error
:
 iny
 lda (tls_hs_ptr),y
 beq :+
 jmp error
:
 iny
 lda (tls_hs_ptr),y
 cmp #2
 beq :+
 jmp error
:
 iny
 lda (tls_hs_ptr),y
 cmp #4
 beq :+
 jmp error
:
 iny
 lda (tls_hs_ptr),y
 cmp #8
 beq :+
 jmp error
:
 iny
 lda (tls_hs_ptr),y
 cmp #4
 beq :+
 jmp error
:
 iny
 lda (tls_hs_ptr),y
 cmp #2
 beq :+
 jmp error
:
 iny
 lda (tls_hs_ptr),y
 beq :+
 jmp error
:
 clc
 lda tls_hs_ptr
 adc #8
 sta _rsa_signature
 lda tls_hs_ptr+1
 adc #0
 sta _rsa_signature+1
 lda #0
 sta _rsa_signature_len
 lda #2
 sta _rsa_signature_len+1
 jsr sha256_init
 ldx #63
 lda #32
:
 sta sha256_block,x
 dex
 bpl :-
 jsr sha256_process_block
 ldx #0
:
 lda label,x
 sta sha256_block,x
 inx
 cpx #33
 bne :-
 lda #0
 sta sha256_block+33
 ldx #0
:
 lda tls_transcript,x
 sta sha256_block+34,x
 inx
 cpx #30
 bne :-
 jsr sha256_process_block
 ldx #63
 lda #0
:
 sta sha256_block,x
 dex
 bpl :-
 lda tls_transcript+30
 sta sha256_block
 lda tls_transcript+31
 sta sha256_block+1
 lda #128
 sta sha256_block+2
 lda #4
 sta sha256_block+62
 lda #16
 sta sha256_block+63
 jsr sha256_process_block
 jsr sha256_final
 ldx #31
:
 lda sha256_hash,x
 sta _rsa_message_hash,x
 dex
 bpl :-
 lda #$26
 sta tls_recv_sub_progress
 jsr _rsa_verify
 lda #$27
 sta tls_recv_sub_progress
 lda _rsa_result
 cmp #1
 bne error
 clc
 rts
error:
 sec
 rts
.segment "RODATA"
label:.byte "TLS 1.3, server CertificateVerify",0
