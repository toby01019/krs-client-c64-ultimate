#pragma code-name ("CLIENT_CODE")
#pragma bss-name ("CLIENT_BSS")
#pragma charmap (32,32)
#pragma charmap (33,33)
#pragma charmap (34,34)
#pragma charmap (35,35)
#pragma charmap (36,36)
#pragma charmap (37,37)
#pragma charmap (38,38)
#pragma charmap (39,39)
#pragma charmap (40,40)
#pragma charmap (41,41)
#pragma charmap (42,42)
#pragma charmap (43,43)
#pragma charmap (44,44)
#pragma charmap (45,45)
#pragma charmap (46,46)
#pragma charmap (47,47)
#pragma charmap (48,48)
#pragma charmap (49,49)
#pragma charmap (50,50)
#pragma charmap (51,51)
#pragma charmap (52,52)
#pragma charmap (53,53)
#pragma charmap (54,54)
#pragma charmap (55,55)
#pragma charmap (56,56)
#pragma charmap (57,57)
#pragma charmap (58,58)
#pragma charmap (59,59)
#pragma charmap (60,60)
#pragma charmap (61,61)
#pragma charmap (62,62)
#pragma charmap (63,63)
#pragma charmap (64,64)
#pragma charmap (65,65)
#pragma charmap (66,66)
#pragma charmap (67,67)
#pragma charmap (68,68)
#pragma charmap (69,69)
#pragma charmap (70,70)
#pragma charmap (71,71)
#pragma charmap (72,72)
#pragma charmap (73,73)
#pragma charmap (74,74)
#pragma charmap (75,75)
#pragma charmap (76,76)
#pragma charmap (77,77)
#pragma charmap (78,78)
#pragma charmap (79,79)
#pragma charmap (80,80)
#pragma charmap (81,81)
#pragma charmap (82,82)
#pragma charmap (83,83)
#pragma charmap (84,84)
#pragma charmap (85,85)
#pragma charmap (86,86)
#pragma charmap (87,87)
#pragma charmap (88,88)
#pragma charmap (89,89)
#pragma charmap (90,90)
#pragma charmap (91,91)
#pragma charmap (92,92)
#pragma charmap (93,93)
#pragma charmap (94,94)
#pragma charmap (95,95)
#pragma charmap (96,96)
#pragma charmap (97,97)
#pragma charmap (98,98)
#pragma charmap (99,99)
#pragma charmap (100,100)
#pragma charmap (101,101)
#pragma charmap (102,102)
#pragma charmap (103,103)
#pragma charmap (104,104)
#pragma charmap (105,105)
#pragma charmap (106,106)
#pragma charmap (107,107)
#pragma charmap (108,108)
#pragma charmap (109,109)
#pragma charmap (110,110)
#pragma charmap (111,111)
#pragma charmap (112,112)
#pragma charmap (113,113)
#pragma charmap (114,114)
#pragma charmap (115,115)
#pragma charmap (116,116)
#pragma charmap (117,117)
#pragma charmap (118,118)
#pragma charmap (119,119)
#pragma charmap (120,120)
#pragma charmap (121,121)
#pragma charmap (122,122)
#pragma charmap (123,123)
#pragma charmap (124,124)
#pragma charmap (125,125)
#pragma charmap (126,126)
#include <conio.h>
#include <string.h>
#include "client.h"
#pragma code-name ("CLIENT_UI_CODE")
unsigned char term_x,term_y,term_mode,term_lx,term_ly,term_max,term_mask,term_view,term_len,term_dead;
unsigned char term_line[81];
unsigned char term_wide;
static unsigned char footer_copy[80];
static const char guest_help[]="GAST: NAME GUEST / PASSWORT GUESTGUEST";
void bitmap_render(void),bitmap_off(void);
void term_graphics_off(void){bitmap_off();term_wide=0;}
static unsigned char state,saved_x,saved_y,wrap,param_count,reverse;
static unsigned int params[4];
static unsigned char top,bottom;
static void scroll(unsigned char down){
 unsigned int from=(unsigned int)top*80,to=(unsigned int)bottom*80;
 if(down){memmove(cert_buf+from+80,cert_buf+from,to-from);memset(cert_buf+from,32,80);}
 else{memmove(cert_buf+from,cert_buf+from+80,to-from);memset(cert_buf+to,32,80);}
}
void term_reset(void){
 memset(cert_buf,32,2000);term_x=term_y=state=wrap=top=reverse=0;bottom=24;term_view=20;
}
static unsigned char bound(unsigned int n,unsigned char max){return n>max?max:(unsigned char)n;}
void term_put(unsigned char c){
 unsigned int i,n,p;
 if(state==1){
  if(c==91){state=2;param_count=0;memset(params,0,sizeof(params));}
  else{state=0;if(c==99)term_reset();}
  return;
 }
 if(state==2){
  if(c>=48&&c<=57){if(params[param_count]<1000)params[param_count]=params[param_count]*10+c-48;return;}
  if(c==59){if(param_count<3)++param_count;return;}
  if(c==63)return;
  state=0;n=params[0]?params[0]:1;
  if(c==72||c==102){term_y=bound(n-1,24);term_x=bound(params[1]?params[1]-1:0,79);wrap=0;}
  else if(c==65){term_y=n>term_y?0:term_y-n;wrap=0;}
  else if(c==66){term_y=bound(term_y+n,24);wrap=0;}
  else if(c==67){term_x=bound(term_x+n,79);wrap=0;}
  else if(c==68){term_x=n>term_x?0:term_x-n;wrap=0;}
  else if(c==74){memset(cert_buf,32,2000);term_x=term_y=wrap=0;}
  else if(c==75){memset(cert_buf+(unsigned int)term_y*80+term_x,32,80-term_x);}
  else if(c==115){saved_x=term_x;saved_y=term_y;}
  else if(c==117){term_x=saved_x;term_y=saved_y;wrap=0;}
  else if(c==114){top=bound(n-1,24);bottom=bound(params[1]?params[1]-1:24,24);if(bottom<top)bottom=top;term_x=term_y=wrap=0;}
  else if(c==83||c==84){if(n>25)n=25;for(i=0;i<n;i++)scroll(c==84);}
  else if(c==109){for(i=0;i<=param_count;i++){p=params[i];if(p==0||p==27||p==40)reverse=0;else if(p==7||(p>=41&&p<=47)||(p>=100&&p<=107))reverse=128;}}
  return;
 }
 if(c==27){state=1;return;}
 if(c==13){term_x=0;wrap=0;return;}
 if(c==10){wrap=0;if(term_y==bottom)scroll(0);else if(term_y<24)++term_y;return;}
 if(c==8){if(term_x)--term_x;wrap=0;return;}
 if(c==9){term_x=bound((term_x+8)&248,79);wrap=0;return;}
 if(c<32)return;
 if(wrap){term_x=0;if(term_y==bottom)scroll(0);else if(term_y<24)++term_y;wrap=0;}
 p=(unsigned int)term_y*80+term_x;cert_buf[p]=c|reverse;
 if(term_x==79)wrap=1;else ++term_x;
}
static unsigned char plain_screen_char(unsigned char c){
 if(c>=97&&c<=122)return c-96;
 if(c>=64&&c<=95)return c-64;
 if(c>=32&&c<=63)return c;
 if(c==96)return 39;
 if(c>=176)return 43;
 return 63;
}
static unsigned char screen_char(unsigned char c){return plain_screen_char(c&127)|(c&128);}
#pragma code-name ("CLIENT_CODE")
void term_wait(void){
 static const char message[]="VERBINDE NEU... BITTE WARTEN";
 unsigned char i,offset;
 unsigned char*row;
 /* open_again has already saved the mailbox screen in the REU. The
    physical display stays visible while RSA reuses cert_buf as scratch. */
 if(term_wide){
  row=cert_buf+1920;memset(row,160,80);
  offset=(80-(sizeof(message)-1))/2;
  for(i=0;i<sizeof(message)-1;i++)row[offset+i]=message[i]|128;
  bitmap_render();
 }else{
  row=(unsigned char*)0x07c0;memset(row,160,40);
  offset=(40-(sizeof(message)-1))/2;
  for(i=0;i<sizeof(message)-1;i++)row[offset+i]=screen_char(message[i])|128;
 }
}
#pragma code-name ("CLIENT_UI_CODE")
void term_echo(void){
 unsigned char i,c;
 unsigned int pos=(unsigned int)term_ly*80+term_lx;
 for(i=0;i<term_max&&term_lx+i<80;i++){
  c=32;
  if(i<term_len){if(term_mask)c=42;else c=term_line[i];}
  cert_buf[pos+i]=c;
 }
 if(term_len<term_max&&term_lx+term_len<80)cert_buf[pos+term_len]=95;
}
#pragma code-name ("EDITOR_CODE")
void term_render(void){
 unsigned int row,col,base;
 unsigned char*screen=(unsigned char*)0x0400;
 if(term_wide){
  if(guest_hint&&term_mode!=2){
   memcpy(footer_copy,cert_buf+1920,80);memset(cert_buf+1920,160,80);
   for(col=0;col<sizeof(guest_help)-1;col++)cert_buf[1942+col]=guest_help[col]|128;
   bitmap_render();memcpy(cert_buf+1920,footer_copy,80);
  }else bitmap_render();return;
 }
 bitmap_off();
 for(row=0;row<24;row++){
  base=row*80+term_view;
  for(col=0;col<40;col++)screen[row*40+col]=screen_char(cert_buf[base+col]);
 }
 memset(screen+960,32,40);gotoxy(0,24);
 if(term_mode==2)cputs(editor_footer());
 else if(guest_hint)cputs(guest_help);
 else cputs("F1:AUSSCHNITT F2:80 F3/F5 F7:ENDE");
}
