#ifndef KRS_CLIENT_H
#define KRS_CLIENT_H
#include <time.h>
extern unsigned char cert_buf[2048],tls_hostname[64],tls_hostname_len,tls_state,tls_substage,net_error;
extern unsigned int cert_buf_len,tls_app_len;
extern unsigned char *tls_app_ptr;
extern unsigned char request[512];
unsigned char network_init(void),connection_open(void),send_tls(void),receive_tls(void);
void connection_close(void),screen_save(void),screen_restore(void);
void connection_forget(void);
extern unsigned char term_x,term_y,term_mode,term_lx,term_ly,term_max,term_mask,term_view,term_len,term_dead;
extern unsigned char term_line[81];
void term_put(unsigned char),term_reset(void),term_render(void),term_echo(void);
extern unsigned char term_wide;
void term_graphics_off(void),term_wait(void);
void json_byte(unsigned char),json_reset(void);
unsigned char api_request(const char*,const char*,const char*,unsigned char);
extern unsigned char session_token[65],http_close,api_error;
extern unsigned int http_status;
extern unsigned char editor_buf[80],editor_count,editor_cap,editor_active,editor_row,editor_col,editor_state,editor_bad;
extern unsigned char guest_hint;
void __fastcall__ editor_load(unsigned char),__fastcall__ editor_store(unsigned char);
void editor_screen_save(void),editor_screen_restore(void);
void editor_reset(void),editor_initial(const char*),editor_render(void),editor_start(void);
const char*editor_footer(void);
unsigned char __fastcall__ editor_key(unsigned char);
unsigned int editor_json_size(void);
#endif
