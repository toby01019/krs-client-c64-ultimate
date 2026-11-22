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
/* C64 application transport for the private KRSTLS/1 firmware target.
   Plain HTTP exists only across the local cartridge bus, TLS on the module.
   No direct-network or plaintext-server fallback is allowed. */
#include <string.h>
#include <time.h>
#include "client.h"
#pragma code-name ("TLS12_CODE")
#pragma bss-name ("TLS12_BSS")
typedef unsigned char u8;
extern u8 random_byte(void);
static u8 command[514],response[512],status[8],live;
static unsigned int response_len;
static u8 status_len;
volatile u8 tls12_stage;
u8 tls_error;
#define REG(a) (*(volatile u8 *)(a))
/* Conservatively paced for a STOCK ~1 MHz C64 only, not a turbo target. */
static void fence(void){u8 i;for(i=0;i<8;++i){asm("nop");}}
static u8 wait_state(u8 ready){
 clock_t start=clock();u8 s;
 do{s=REG(0xdf1c);fence();
  if(ready?(s&0x20)!=0:(s&0x35)==0)return 1;
  if(s&8)return 0;
 }while(clock()-start<60UL*CLOCKS_PER_SEC);
 return 0;
}
static u8 exchange(unsigned int n){
 unsigned int i;clock_t start;u8 s,c;
 response_len=status_len=0;
 if(n<2||n>514||!wait_state(0))goto fail;
 for(i=0;i<n;++i){REG(0xdf1d)=command[i];fence();}
 REG(0xdf1c)=8;fence();REG(0xdf1c)=1;fence();
 if(!wait_state(1))goto fail;
 start=clock();
 while((s=REG(0xdf1c))&0xc0){
  fence();
  if(s&0x80){
   c=REG(0xdf1e);fence();
   if(response_len>=512)goto fail;
   response[response_len++]=c;
  }else{
   c=REG(0xdf1f);fence();
   if(status_len>=8)goto fail;
   status[status_len++]=c;
  }
  if(clock()-start>5UL*CLOCKS_PER_SEC)goto fail;
 }
 /* All replies in KRSTLS/1 are final and <=512 bytes. Reject multipart. */
 if((s&0x30)!=0x20)goto fail;
 REG(0xdf1c)=2;fence();
 if(status_len!=6||memcmp(status,"000 OK",6))return 0;
 return 1;
fail:
 REG(0xdf1c)=4;fence();tls_error=1;live=0;return 0;
}
static u8 op(u8 code){command[0]=14;command[1]=code;return exchange(2);}
unsigned char network_init(void){
 if(REG(0xdf1d)!=0xc9)return 0;
 if(!op(1)||response_len!=8||memcmp(response,"KRSTLS/1",8))return 0;
 return 1;
}
unsigned char connection_open(void){
 u8 i;
 tls_error=0;tls12_stage=1;live=0;
 command[0]=14;command[1]=2;
 for(i=0;i<32;++i)command[i+2]=random_byte();
 i=exchange(34);memset(command+2,0,32);
 if(!i){tls12_stage=239;return 0;}
 tls12_stage=3;
 if(!op(0x10)){tls12_stage=239;return 0;}
 live=1;tls_state=7;tls12_stage=7;return 1;
}
unsigned char send_tls(void){
 u8 ok;
 if(!live||!tls_app_len||tls_app_len>512)return 0;
 command[0]=14;command[1]=0x11;memcpy(command+2,tls_app_ptr,tls_app_len);
 ok=exchange(tls_app_len+2);memset(command+2,0,512);
 if(!ok){live=0;return 0;}return 1;
}
unsigned char receive_tls(void){
 if(!live||!op(0x12)||!response_len){live=0;tls_state=255;return 0;}
 tls_app_ptr=response;tls_app_len=response_len;return 1;
}
void connection_close(void){if(live)op(0x13);live=0;}
void connection_forget(void){
 op(0x13);live=0;memset(command,0,sizeof(command));
 memset(response,0,sizeof(response));memset(status,0,sizeof(status));
}
