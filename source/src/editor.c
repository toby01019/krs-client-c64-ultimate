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
#include <string.h>
#include "client.h"
#pragma code-name ("EDITOR_CODE")
unsigned char editor_buf[80],editor_count,editor_cap,editor_active,editor_row,editor_col,editor_state,editor_bad;
static unsigned char spare[80],top,full;
static unsigned char length(void){return (unsigned char)strlen((char*)editor_buf);}
void editor_reset(void){
 editor_active=editor_count=editor_row=editor_col=editor_state=editor_bad=top=full=0;editor_cap=64;
}
void editor_initial(const char*s){
 unsigned char i;
 if(editor_count>=editor_cap||strlen(s)>79){editor_bad=1;return;}
 for(i=0;s[i];i++)if((unsigned char)s[i]<32||(unsigned char)s[i]>=127){editor_bad=1;return;}
 memset(editor_buf,0,80);strcpy((char*)editor_buf,s);editor_store(editor_count++);
}
void editor_start(void){
 if(editor_active)return;
 editor_screen_save();editor_active=1;
 if(!editor_count)editor_initial("");
}
const char*editor_footer(void){
 if(editor_bad)return "VORGABE NICHT DARSTELLBAR. ESC:ABBRUCH";
 if(editor_state==1)return "BEITRAG SENDEN? J=JA N=ZURUECK";
 if(editor_state==2)return "ENTWURF VERWERFEN? J=JA N=ZURUECK";
 if(editor_state==3)return "SENDUNG UNKLAR! PRUEFEN. F7:ENDE";
 if(full)return "ZEILE/TEXT VOLL. CURSOR ODER DEL";
 return "F8:SENDEN ESC:ABBRUCH F2:80 F3/F5";
}
static void bar(unsigned char row,const char*s){
 unsigned char i,n=(unsigned char)strlen(s),x;
 unsigned int pos=(unsigned int)row*80;
 x=term_wide?(80-n)/2:term_view+(40-n)/2;
 for(i=0;i<80;i++)cert_buf[pos+i]=160;
 for(i=0;i<n;i++)cert_buf[pos+x+i]=s[i]|128;
}
void editor_render(void){
 unsigned char r,n;
 char title[]="BEITRAG - ZEILE 01/64";
 editor_start();
 if(editor_row<top)top=editor_row;
 if(editor_row>=top+23)top=editor_row-22;
 if(!term_wide){if(editor_col<term_view)term_view=editor_col<20?0:20;else if(editor_col>=term_view+40)term_view=editor_col>=60?40:20;}
 memset(cert_buf,32,2000);
 title[16]=48+(editor_row+1)/10;title[17]=48+(editor_row+1)%10;
 title[19]=48+editor_cap/10;title[20]=48+editor_cap%10;
 bar(0,title);
 for(r=0;r<23&&top+r<editor_count;r++){
  editor_load(top+r);n=length();memcpy(cert_buf+(unsigned int)(r+1)*80,editor_buf,n);
 }
 cert_buf[(unsigned int)(editor_row-top+1)*80+editor_col]^=128;
 bar(24,editor_footer());term_render();
}
/* Shift REU line slots, preserving the current line in spare. */
static unsigned char split(void){
 unsigned char i,n;
 if(editor_count>=editor_cap){full=1;return 0;}
 memcpy(spare,editor_buf,80);
 for(i=editor_count;i>editor_row+1;i--){editor_load(i-1);editor_store(i);}
 memcpy(editor_buf,spare,80);editor_buf[editor_col]=0;editor_store(editor_row);
 n=(unsigned char)strlen((char*)spare+editor_col);
 memmove(editor_buf,spare+editor_col,n+1);editor_store(++editor_row);
 ++editor_count;editor_col=0;return 1;
}
static void backspace(void){
 unsigned char i,n,prev;
 n=length();
 if(editor_col){memmove(editor_buf+editor_col-1,editor_buf+editor_col,n-editor_col+1);--editor_col;editor_store(editor_row);}
 else if(editor_row){
  memcpy(spare,editor_buf,80);editor_load(editor_row-1);prev=length();
  if(prev+n>79){full=1;return;}
  memcpy(editor_buf+prev,spare,n+1);editor_store(editor_row-1);
  for(i=editor_row;i+1<editor_count;i++){editor_load(i+1);editor_store(i);}
  --editor_count;--editor_row;editor_col=prev;
 }
}
unsigned char __fastcall__ editor_key(unsigned char c){
 unsigned char n,result=0;
 if(editor_state){
  if(editor_state==3)return c==136?3:0;
  if(c==74||c==106)result=editor_state;
  editor_state=0;return result;
 }
 full=0;
 if(c==27||c==3||c==136){editor_state=2;return 0;}
 if(editor_bad)return 0;
 if(c==140){editor_state=1;return 0;}
 editor_load(editor_row);n=length();
 if(c==13)split();
 else if(c==20)backspace();
 else if(c==157){if(editor_col)--editor_col;else if(editor_row){--editor_row;editor_load(editor_row);editor_col=length();}}
 else if(c==29){if(editor_col<n)++editor_col;else if(editor_row+1<editor_count){++editor_row;editor_col=0;}}
 else if(c==19)editor_col=0;
 else if(c==145||c==17||c==134||c==135){
  if(c==145&&editor_row)--editor_row;
  if(c==17&&editor_row+1<editor_count)++editor_row;
  if(c==134)editor_row=editor_row>22?editor_row-23:0;
  if(c==135)editor_row=editor_row+23<editor_count?editor_row+23:editor_count-1;
  editor_load(editor_row);n=length();if(editor_col>n)editor_col=n;
 }else if(c>=32&&c<127){
  if(n==79){if(editor_col!=79||!split()){full=1;return 0;}n=0;}
  memmove(editor_buf+editor_col+1,editor_buf+editor_col,n-editor_col+1);
  editor_buf[editor_col++]=c;editor_store(editor_row);
 }
 return 0;
}
#pragma code-name ("CLIENT_CODE")
unsigned int editor_json_size(void){
 unsigned char r,i,c;unsigned int n=2;
 for(r=0;r<editor_count;r++){
  editor_load(r);n+=2+(r!=0);
  for(i=0;(c=editor_buf[i])!=0;i++)n+=c==34||c==92?2:1;
 }
 return n;
}
