.include "constants.inc"
.include "net_abi.inc"
.export _network_init,_connection_open,_connection_close,_send_tls,_receive_tls
.export _tls_hostname,_tls_hostname_len,_tls_app_ptr,_tls_app_len,_tls_state
.export _cert_buf,_cert_buf_len,_tls_substage,_net_error
.export _request
.import http_resp_buf
_request=http_resp_buf
.import tls_hostname,tls_hostname_len,tls_app_ptr,tls_app_len,tls_state
.import cert_buf,cert_buf_len,tls_recv_sub_progress,tls_connect,tls_send,tls_recv,tls_close
_tls_hostname=tls_hostname
_tls_hostname_len=tls_hostname_len
_tls_app_ptr=tls_app_ptr
_tls_app_len=tls_app_len
_tls_state=tls_state
_cert_buf=cert_buf
_cert_buf_len=cert_buf_len
_tls_substage=tls_recv_sub_progress
_net_error=net_last_error
.segment "CODE"
_network_init:
 jsr net_init
 bcs failed
 jsr net_dhcp_acquire
 jmp result
_connection_open:
 lda #<tls_hostname
 ldx #>tls_hostname
 jsr net_dns_resolve
 bcs failed
 lda #$bb
 ldx #1
 jsr net_tcp_connect
 bcs failed
 jsr tls_connect
 jmp result
_send_tls:
 jsr tls_send
 jmp result
_receive_tls:
 jsr tls_recv
 jmp result
_connection_close:
 jsr tls_close
 jsr net_tcp_close
 rts
result:
 bcs failed
 lda #1
 ldx #0
 rts
failed:
 lda #0
 tax
 rts
; Preserve the terminal image while certificate / RSA scratch reuses it.
.import reu_execute
.segment "CODE"
.export _screen_save,_screen_restore
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
 sta $df08
 sta $df09
 sta $df0a
 lda #$40
 sta $df06
 lda #$d0
 sta $df07
 lda #7
 sta $df08
 pla
 jmp reu_execute
