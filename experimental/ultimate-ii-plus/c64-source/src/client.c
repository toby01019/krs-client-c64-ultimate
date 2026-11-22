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
#include <time.h>
#include "rsa4096.h"
#include "client.h"
#pragma code-name ("CLIENT_UI_CODE")
volatile unsigned char client_stage;
volatile unsigned char client_key;
volatile unsigned char test_reconnect;
volatile unsigned char client_reconnecting;
volatile unsigned char test_editor;
unsigned char guest_hint;
static unsigned char editor_local;
static unsigned char connected;
unsigned char rsa_pin_validate(void){return rsa_check_certificate(cert_buf,cert_buf_len);}
static unsigned char open_again(void){
 screen_save();term_wait();client_reconnecting=1;
 connection_close();connected=connection_open();screen_restore();
 term_render();client_reconnecting=0;
 return connected;
}
static unsigned char request_api(const char*a,const char*k,const char*v,unsigned char cancel){
 if(http_close&&!open_again())return 0;
 if(!api_request(a,k,v,cancel)){
  /* A poll has no user write to duplicate; retry once on a fresh socket. */
  if(strcmp(a,"poll")||!session_token[0]||!open_again()||!api_request(a,k,v,cancel))return 0;
 }
 if(term_mode==1){term_echo();term_view=term_lx>=60?40:term_lx<20?0:20;}
 if(term_mode==2)editor_render();else term_render();return 1;
}
static unsigned char getkey(void){unsigned char c=client_key;if(c){client_key=0;return c;}return kbhit()?cgetc():0;}
void clientmain(void){
 clock_t last;
 unsigned char c,ok=1;
 unsigned char action;
 char special[2];
 bordercolor(6);bgcolor(0);textcolor(3);
 clrscr();cputs("K.R.S. - C64 + ULTIMATE-II+");gotoxy(0,2);cputs("TLS-MODUL WIRD VERBUNDEN...");
 gotoxy(0,4);cputs("GASTZUGANG: NAME GUEST");gotoxy(0,5);cputs("PASSWORT GUESTGUEST");gotoxy(0,7);cputs("NACH DEM VERBINDEN: L FUER LOGIN");guest_hint=1;
 client_stage=1;
 if(!network_init()){client_stage=240;ok=0;goto error;}
 memcpy(tls_hostname,"quipzz.bplaced.net",19);tls_hostname_len=18;
 client_stage=2;
 if(!connection_open()){connection_close();client_stage=241;ok=0;goto error;}
 connected=1;client_stage=3;term_reset();
 if(!request_api("connect",0,0,0)){client_stage=242;ok=0;goto error;}
 client_stage=4;last=clock();
 while(ok&&!term_dead){
  if(test_reconnect){test_reconnect=0;http_close=1;last=clock()-2*CLOCKS_PER_SEC;}
  if(test_editor){test_editor=0;editor_local=1;editor_reset();term_mode=2;editor_render();}
  c=getkey();
  if(c==136&&term_mode!=2)break;
  if(c==137){term_wide^=1;if(term_mode==2)editor_render();else term_render();continue;}
  if(c==133&&term_mode!=2){term_view=term_view>=40?0:term_view+20;term_render();continue;}
  if(term_mode==2){
   if(c){
    if(c>=65&&c<=90)c+=32;else if(c>=193&&c<=218)c-=128;
    action=editor_key(c);
    if(action==3)break;
    if(action){
     editor_screen_restore();term_mode=0;editor_active=0;
     if(editor_local){editor_local=0;term_render();}
     else if(!request_api("editor","lines","",action==2)){
      /* Never retry a post: the server may already have stored it. */
      term_mode=2;editor_active=1;editor_state=3;editor_render();
     }
     last=clock();
    }else editor_render();
   }else if(!editor_local&&editor_state!=3&&clock()-last>=2*CLOCKS_PER_SEC){
    if(!request_api("poll",0,0,0)){editor_state=3;editor_render();}last=clock();
   }
  }else if(c==134||c==135){ok=request_api("key","key",c==134?"PGUP":"PGDN",0);last=clock();}
  else if(c){
   if(c>=65&&c<=90)c+=32;else if(c>=193&&c<=218)c-=128;
   if(term_mode==1){
    if(c==13){term_line[term_len]=0;if(term_mask)guest_hint=0;ok=request_api("line","line",(char*)term_line,0);memset(term_line,0,sizeof(term_line));last=clock();}
    else if(c==27||c==3){ok=request_api("line","line",0,1);last=clock();}
    else if(c==20){if(term_len)--term_len;term_echo();term_render();}
    else if(c>=32&&c<127&&term_len<term_max&&term_len<80){term_line[term_len++]=c;term_echo();term_view=term_lx+term_len>=60?40:term_lx<20?0:20;term_render();}
   }else{
    const char*key=0;
    if(c==13)key="ENTER";else if(c==27||c==3)key="ESC";
    else if(c==17)key="DOWN";else if(c==145)key="UP";
    else if(c==29)key="RIGHT";else if(c==157)key="LEFT";
    else if(c>=32&&c<127){special[0]=c;special[1]=0;key=special;}
    if(key){ok=request_api("key","key",key,0);last=clock();}
   }
  }else if(clock()-last>=2*CLOCKS_PER_SEC){ok=request_api("poll",0,0,0);last=clock();}
 }
error:
 term_graphics_off();
 if(!ok&&client_stage==4)client_stage=250;
 if(!ok){gotoxy(0,24);cputs(client_stage==240?"TLS-FIRMWARE FEHLT. F7:ENDE":"VERBINDUNG FEHLGESCHLAGEN. F7:ENDE");while(getkey()!=136){};}
 if(connected)connection_close();
 connection_forget();
 memset(session_token,0,65);memset(term_line,0,81);memset(request,0,512);
}
