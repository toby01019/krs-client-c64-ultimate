.include "constants.inc"
.macpack longbranch
.import net_init,net_dhcp_acquire,net_dns_resolve,net_tcp_connect,net_tcp_send,net_tcp_close,net_poll,net_recv_byte,net_send_len
.import tls_app_ptr,tls_app_len,tls_state,cert_buf,cert_buf_len,http_resp_buf
.segment "CLIENT_BSS"
tls_hostname:.res 64
tls_hostname_len:.res 1
.import drbg_random_byte,drbg_reseed,tls_transcript_init,tls_transcript_update,tls_transcript_hash,tls_transcript
.import tcp_recv_head,tcp_recv_tail,tcp_recv_overflow
.import _hash_ptr,_hash_len
.export _raw_network_init,_tcp_open,_tcp_send,_tcp_byte,_tcp_close,_random_byte,_reseed,_transcript_init,_transcript_update,_transcript_hash
.export _tls_hostname,_tls_hostname_len,_tls_app_ptr,_tls_app_len,_tls_state,_cert_buf,_cert_buf_len,_request
.export _transcript
.import tls_rec_buf
.export _encoded
_encoded=tls_rec_buf
_tls_hostname=tls_hostname
_tls_hostname_len=tls_hostname_len
_tls_app_ptr=tls_app_ptr
_tls_app_len=tls_app_len
_tls_state=tls_state
_cert_buf=cert_buf
_cert_buf_len=cert_buf_len
_request=http_resp_buf
_transcript=tls_transcript
.segment "CODE"
_raw_network_init:
 jsr net_init
 jcs failed
 jsr net_dhcp_acquire
 jmp result
_tcp_open:
 lda tcp_recv_tail
 sta tcp_recv_head
 lda tcp_recv_tail+1
 sta tcp_recv_head+1
 lda #0
 sta tcp_recv_overflow
 lda #<tls_hostname
 ldx #>tls_hostname
 jsr net_dns_resolve
 jcs failed
 lda #$bb
 ldx #1
 jsr net_tcp_connect
 jmp result
_tcp_send:
 lda tls_app_len
 sta net_send_len
 lda tls_app_len+1
 sta net_send_len+1
 lda tls_app_ptr
 ldx tls_app_ptr+1
 jsr net_tcp_send
 jmp result
_tcp_byte:
 jsr net_recv_byte
 bcc byte_ok
 jsr net_poll
 jsr net_recv_byte
 bcc byte_ok
 lda #$ff
 tax
 rts
byte_ok:
 ldx #0
 rts
_tcp_close:
 jmp net_tcp_close
_random_byte:
 jsr drbg_random_byte
 ldx #0
 rts
_reseed:
 jmp drbg_reseed
_transcript_init:
 jmp tls_transcript_init
_transcript_hash:
 jmp tls_transcript_hash
_transcript_update:
 lda _hash_ptr
 sta zp_ptr
 lda _hash_ptr+1
 sta zp_ptr+1
 lda _hash_len
 sta zp_count
 lda _hash_len+1
 sta zp_count+1
 jmp tls_transcript_update
result:
 bcs failed
 lda #1
 ldx #0
 rts
failed:
 lda #0
 tax
 rts
.export _screen_save,_screen_restore
.import reu_execute
_screen_save:
 lda #$90
 bne screen_dma
_screen_restore:
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
 sta $df06
 sta $df09
 sta $df0a
 lda #$d0
 sta $df07
 lda #7
 sta $df08
 pla
 jmp reu_execute
