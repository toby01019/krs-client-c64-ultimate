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
#pragma code-name ("CLIENT_PARSE_CODE")
unsigned char session_token[65],http_close,api_error;
unsigned int http_status;
static unsigned long seq;
static unsigned char depth,context[8],expect_key[8],lex,iskey,escape_count,key_id,value_len,frame_type,utf_need;
static unsigned int unicode_value,number;
static unsigned char hs,line_len,chunked,done,number_negative;
static unsigned int remaining,content_length;
static unsigned char has_length;
/* JSON scratch lives in the reusable request buffer, never on the C stack. */
#define word (request+160)
static unsigned char ascii_lower(unsigned char c){return c>=65&&c<=90?c+32:c;}
static unsigned char key_code(void){
 if(!strcmp((char*)word,"d"))return 1;
 if(!strcmp((char*)word,"t"))return 2;
 if(!strcmp((char*)word,"m"))return 3;
 if(!strcmp((char*)word,"token"))return 4;
 if(!strcmp((char*)word,"seq"))return 5;
 if(!strcmp((char*)word,"x"))return 6;
 if(!strcmp((char*)word,"y"))return 7;
 if(!strcmp((char*)word,"max"))return 8;
 if(!strcmp((char*)word,"mask"))return 9;
 if(!strcmp((char*)word,"initial"))return 10;
 if(!strcmp((char*)word,"cap"))return 11;
 return 0;
}
static void string_done(void){
 word[value_len]=0;
 if(iskey){key_id=key_code();return;}
 if(key_id==2){
  frame_type=!strcmp((char*)word,"ansi")?1:!strcmp((char*)word,"mode")?2:0;
  if(!strcmp((char*)word,"disconnect")){term_dead=1;term_mode=0;}
 }
 else if(key_id==3){
  if(!strcmp((char*)word,"line")){term_mode=1;term_lx=term_x;term_ly=term_y;term_max=20;term_mask=0;term_len=0;}
  else if(!strcmp((char*)word,"editor")){term_mode=2;editor_reset();}
  else term_mode=0;
 }
 else if(key_id==4&&value_len==64)memcpy(session_token,word,65);
 else if(key_id==9)term_mask=value_len!=0;
 else if(key_id==10){
  if(term_mode==2)editor_initial((char*)word);
  else{term_len=value_len>80?80:value_len;memcpy(term_line,word,term_len);term_line[term_len]=0;}
 }
}
static void decoded(unsigned int c){
 if(!iskey&&key_id==1&&frame_type==1){
  if(c>127){
   if(c==196||c==228)c=65;
   else if(c==214||c==246)c=79;
   else if(c==220||c==252)c=85;
   else if(c==223)c=83;
   else if(c>=0x2500&&c<=0x257f)c=43;
   else if(c==160)c=32;
   else c=63;
  }
  term_put((unsigned char)c);
 }else if(value_len<80){if(term_mode==2&&key_id==10&&c>=127)editor_bad=1;word[value_len++]=(unsigned char)(c<128?c:63);}
}
void json_reset(void){depth=lex=key_id=frame_type=utf_need=0;}
void json_byte(unsigned char c){
 unsigned char v;
again:
 if(lex==3){
  v=c>=48&&c<=57?c-48:c>=65&&c<=70?c-55:c>=97&&c<=102?c-87:255;
  if(v==255){api_error=1;return;}
  unicode_value=(unicode_value<<4)|v;
  if(++escape_count==4){lex=1;decoded(unicode_value);}return;
 }
 if(lex==2){
  lex=1;
  if(c==117){lex=3;escape_count=0;unicode_value=0;return;}
  if(c==110)c=10;else if(c==114)c=13;else if(c==116)c=9;else if(c==98)c=8;else if(c==102)c=12;
  decoded(c);return;
 }
 if(lex==1){
  if(utf_need){
   if((c&192)!=128){api_error=1;return;}
   unicode_value=(unicode_value<<6)|(c&63);if(!--utf_need)decoded(unicode_value);return;
  }
  if(c==92){lex=2;return;}
  if(c==34){lex=0;string_done();return;}
  if(c>=192&&c<224){unicode_value=c&31;utf_need=1;return;}
  if(c>=224&&c<240){unicode_value=c&15;utf_need=2;return;}
  decoded(c);return;
 }
 if(lex==4){
  if(c>=48&&c<=57){if(number<6500)number=number*10+c-48;return;}
  if(!number_negative){
   if(key_id==5)seq=number;
   else if(key_id==6&&frame_type==2)term_lx=number?((number>80?80:number)-1):0;
   else if(key_id==7&&frame_type==2)term_ly=number?((number>25?25:number)-1):0;
   else if(key_id==8&&frame_type==2)term_max=number>80?80:number;
   else if(key_id==11&&frame_type==2&&term_mode==2&&number){editor_cap=number>64?64:number;if(editor_count>editor_cap)editor_bad=1;}
  }
  lex=0;goto again;
 }
 if(c==123||c==91){
  if(depth==8){api_error=1;return;}
  context[depth]=c;expect_key[depth]=c==123;++depth;
  if(c==123&&depth==3)frame_type=0;
 }else if(c==125||c==93){if(depth)--depth;else api_error=1;}
 else if(c==44){if(depth)expect_key[depth-1]=context[depth-1]==123;}
 else if(c==58){if(depth)expect_key[depth-1]=0;}
 else if(c==34){iskey=depth&&expect_key[depth-1];value_len=0;lex=1;}
 else if((c>=48&&c<=57)||c==45){number=0;number_negative=c==45;lex=4;if(c!=45)number=c-48;}
}
static unsigned char starts(const char*s){return !strncmp((char*)request,s,strlen(s));}
static unsigned int decimal(const unsigned char*p){unsigned int n=0;while(*p>=48&&*p<=57){if(n>6553){api_error=1;return 0;}n=n*10+*p++-48;}return n;}
static void http_line(void){
 unsigned char i,c;
 request[line_len]=0;
 if(hs==0){
  if(line_len<12||strncmp((char*)request,"http/1.",7)){api_error=1;return;}
  http_status=decimal(request+9);hs=1;
 }else if(hs==1){
  if(!line_len){hs=chunked?2:6;remaining=content_length;if(!chunked&&!has_length)api_error=1;}
  else if(starts("transfer-encoding: chunked"))chunked=1;
  else if(starts("content-length:")){i=15;while(request[i]==32)i++;content_length=decimal(request+i);has_length=1;}
  else if(starts("connection: close"))http_close=1;
 }else if(hs==2){
  remaining=0;
  for(i=0;i<line_len&&request[i]!=59;i++){
   c=request[i];if(c==32)break;
   c=c>=48&&c<=57?c-48:c>=97&&c<=102?c-87:255;
   if(c==255||remaining>4095){api_error=1;return;}
   remaining=(remaining<<4)|c;
  }
  hs=remaining?3:7;
 }else if(hs==7&&!line_len)done=1;
 line_len=0;
}
static void http_byte(unsigned char c){
 if(hs==3||hs==6){
  if(!remaining){api_error=1;return;}
  json_byte(c);if(!--remaining){if(hs==6)done=1;else hs=4;}
 }else if(hs==4){if(c==10)hs=2;else if(c!=13)api_error=1;}
 else if(c==10)http_line();
 else if(c!=13){if(line_len<127)request[line_len++]=ascii_lower(c);else api_error=1;}
}
#pragma code-name ("CLIENT_UI_CODE")
static unsigned int append(unsigned int pos,const char*s){while(*s&&pos<511)request[pos++]=*s++;return pos;}
static unsigned int integer(unsigned int pos,unsigned long n){
 unsigned char digits[10],i=0;
 do{digits[i++]=(unsigned char)(n%10)+48;n/=10;}while(n&&i<10);
 while(i)request[pos++]=digits[--i];return pos;
}
static unsigned int quoted(unsigned int pos,const char*s){
 unsigned char c;
 request[pos++]=34;
 while((c=*s++)!=0){if(pos>=509){api_error=1;break;}if(c==34||c==92)request[pos++]=92;request[pos++]=c;}
 request[pos++]=34;return pos;
}
#pragma code-name ("EDITOR_CODE")
static unsigned char flush_body(unsigned int n){tls_app_ptr=request;tls_app_len=n;return send_tls();}
static unsigned char editor_body(void){
 unsigned char r,i,c;unsigned int n=0;
 request[n++]=91;
 for(r=0;r<editor_count;r++){
  editor_load(r);if(r)request[n++]=44;request[n++]=34;
  for(i=0;(c=editor_buf[i])!=0;i++){
   if(n>508){if(!flush_body(n))return 0;n=0;}
   if(c==34||c==92)request[n++]=92;request[n++]=c;
  }
  request[n++]=34;
  if(n>508){if(!flush_body(n))return 0;n=0;}
 }
 request[n++]=93;request[n++]=125;return flush_body(n);
}
#pragma code-name ("CLIENT_HTTP_CODE")
unsigned char api_request(const char*a,const char*key,const char*val,unsigned char nullval){
 unsigned int p=200,body_len,header_len,i;
 unsigned char writing=key&&!strcmp(a,"editor")&&!nullval;
 clock_t start;
 api_error=0;
 p=append(p,"{\"a\":");p=quoted(p,a);
 if(session_token[0]){
  p=append(p,",\"token\":");p=quoted(p,(char*)session_token);
  p=append(p,",\"seq\":");p=integer(p,++seq);
 }
 if(key){p=append(p,",");p=quoted(p,key);request[p++]=58;if(!writing)p=nullval?append(p,"null"):quoted(p,val);}
 if(p>=512)return 0;
 if(!writing)request[p++]=125;body_len=p-200;
 if(api_error)return 0;
 p=append(0,"POST /krs/api.php HTTP/1.1\x0d\x0a" "Host: quipzz.bplaced.net\x0d\x0a" "Content-Type: application/json\x0d\x0a" "Content-Length: ");
 p=integer(p,body_len+(writing?editor_json_size()+1:0));header_len=append(p,"\x0d\x0a" "Connection: close\x0d\x0a\x0d\x0a");
 if(header_len>=200)return 0;
 tls_app_ptr=request;tls_app_len=header_len;if(!send_tls())return 0;
 tls_app_ptr=request+200;tls_app_len=body_len;if(!send_tls())return 0;
 if(writing&&!editor_body())return 0;
 /* One response per socket: avoid stale server keep-alive closures. */
 hs=line_len=chunked=done=has_length=0;http_close=1;http_status=0;json_reset();
 start=clock();
 while(!done&&!api_error&&clock()-start<20*CLOCKS_PER_SEC){
  if(receive_tls()){
   start=clock();
   for(i=0;i<tls_app_len;i++){http_byte(tls_app_ptr[i]);if(done||api_error)break;}
  }else if(tls_state&128)return 0;
 }
 return done&&!api_error&&http_status==200&&depth==0&&lex==0;
}
