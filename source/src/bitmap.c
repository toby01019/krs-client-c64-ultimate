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
#include "client.h"
#pragma code-name ("BITMAP_CODE")
#pragma bss-name ("CLIENT_BSS")
extern unsigned char font80[1024];
void bitmap_begin(void),bitmap_end(void);
static unsigned char active,old_bank,old_d011,old_d016,old_d018;
static unsigned char fontchar(unsigned char c){c&=127;if(c>=65&&c<=90)return c+32;if(c>=97&&c<=122)return c-32;if(c<32)c=32;return c;}
void bitmap_render(void){
 unsigned int i,p,f;
 unsigned char row,col,r,a,b,ia,ib,na,nb,fresh=0;
 volatile unsigned char*bm=(unsigned char*)0xe000;
 volatile unsigned char*cache=(unsigned char*)0xd800;
 if(!active){
  fresh=1;
  old_bank=*(volatile unsigned char*)0xdd00;old_d011=*(volatile unsigned char*)0xd011;old_d016=*(volatile unsigned char*)0xd016;old_d018=*(volatile unsigned char*)0xd018;
  bitmap_begin();for(i=0;i<1000;i++)((volatile unsigned char*)0xd400)[i]=0x30;bitmap_end();
  *(volatile unsigned char*)0xdd00=old_bank&252;*(volatile unsigned char*)0xd018=0x58;*(volatile unsigned char*)0xd016=0x08;*(volatile unsigned char*)0xd011=0x3b;active=1;
 }
 bitmap_begin();
 for(row=0;row<25;row++)for(col=0;col<40;col++){
  p=(unsigned int)row*80+col*2;a=cert_buf[p];b=cert_buf[p+1];
  if(!fresh&&cache[p]==a&&cache[p+1]==b)continue;
  cache[p]=a;cache[p+1]=b;ia=fontchar(a);ib=fontchar(b);
  p=((unsigned int)row*40+col)*8;
  for(r=0;r<8;r++){f=(unsigned int)r*128;na=font80[f+ia]^15;nb=font80[f+ib]^15;if(a&128)na^=15;if(b&128)nb^=15;bm[p+r]=(na<<4)|nb;}
 }
 bitmap_end();
}
void bitmap_off(void){
 if(!active)return;
 *(volatile unsigned char*)0xdd00=old_bank;*(volatile unsigned char*)0xd011=old_d011&127;*(volatile unsigned char*)0xd016=old_d016;*(volatile unsigned char*)0xd018=old_d018;active=0;
}
