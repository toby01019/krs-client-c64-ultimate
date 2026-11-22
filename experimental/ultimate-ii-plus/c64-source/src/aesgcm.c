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
#pragma code-name ("AES_CODE")
#pragma bss-name ("CLIENT_BSS")
typedef unsigned char u8;
static const u8 sb[256]={
0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16};
static u8 rk[176],st[16],gh[16],gv[16],gz[16],ctr[16],stream[16],h[16],tagmask[16],block[16];
static u8 xt(u8 x){return (x<<1)^((x&128)?27:0);}
static void setup(const u8*k){
 u8 i,j,r=1,t[4];
 memcpy(rk,k,16);
 for(i=16;i<176;i+=4){memcpy(t,rk+i-4,4);if(!(i&15)){j=t[0];t[0]=sb[t[1]]^r;t[1]=sb[t[2]];t[2]=sb[t[3]];t[3]=sb[j];r=xt(r);}for(j=0;j<4;j++)rk[i+j]=rk[i+j-16]^t[j];}
}
static void aes(const u8*in,u8*out){
 u8 i,r,a,b,c,d,t;
 memcpy(st,in,16);for(i=0;i<16;i++)st[i]^=rk[i];
 for(r=1;r<=10;r++){
  for(i=0;i<16;i++)st[i]=sb[st[i]];
  t=st[1];st[1]=st[5];st[5]=st[9];st[9]=st[13];st[13]=t;
  t=st[2];st[2]=st[10];st[10]=t;t=st[6];st[6]=st[14];st[14]=t;
  t=st[3];st[3]=st[15];st[15]=st[11];st[11]=st[7];st[7]=t;
  if(r<10)for(i=0;i<16;i+=4){a=st[i];b=st[i+1];c=st[i+2];d=st[i+3];t=a^b^c^d;st[i]=a^t^xt(a^b);st[i+1]=b^t^xt(b^c);st[i+2]=c^t^xt(c^d);st[i+3]=d^t^xt(d^a);}
  for(i=0;i<16;i++)st[i]^=rk[(unsigned int)r*16+i];
 }
 memcpy(out,st,16);
}
static void ghash(const u8*b){
 u8 i,j,k,mask,last;
 for(i=0;i<16;i++)gh[i]^=b[i];
 memcpy(gv,h,16);memset(gz,0,16);
 for(i=0;i<16;i++)for(j=128;j;j>>=1){
  mask=(gh[i]&j)?255:0;for(k=0;k<16;k++)gz[k]^=gv[k]&mask;
  last=gv[15]&1;for(k=15;k;k--)gv[k]=(gv[k]>>1)|(gv[k-1]<<7);gv[0]>>=1;gv[0]^=last?225:0;
 }
 memcpy(gh,gz,16);
}
/* TLS GCM: 96-bit nonce, 13-byte AAD, 128-bit authentication tag.
 * Failed authentication wipes all recovered bytes; compare every tag byte.
 */
unsigned char gcm(const u8*key,const u8*nonce,const u8*aad,u8*data,unsigned int len,u8*tag,u8 decrypt){
 unsigned int off,n;
 u8 i,diff=0;
 setup(key);memset(block,0,16);aes(block,h);
 memcpy(ctr,nonce,12);memset(ctr+12,0,4);ctr[15]=1;aes(ctr,tagmask);
 memset(gh,0,16);memset(block,0,16);memcpy(block,aad,13);ghash(block);
 for(off=0;off<len;off+=n){
  n=len-off;if(n>16)n=16;
  for(i=15;i>=12;i--)if(++ctr[i])break;
  aes(ctr,stream);memset(block,0,16);
  if(decrypt)memcpy(block,data+off,n);
  for(i=0;i<n;i++)data[off+i]^=stream[i];
  if(!decrypt)memcpy(block,data+off,n);
  ghash(block);
 }
 memset(block,0,16);block[7]=104;block[14]=(u8)(len>>5);block[15]=(u8)(len<<3);ghash(block);
 for(i=0;i<16;i++){gh[i]^=tagmask[i];if(decrypt)diff|=gh[i]^tag[i];else tag[i]=gh[i];}
 if(diff){memset(data,0,len);return 0;}return 1;
}
void aes_kat(u8*out){u8 z[16];memset(z,0,16);setup(z);aes(z,out);}
void gcm_clear(void){memset(rk,0,sizeof(rk));memset(st,0,16);memset(h,0,16);memset(tagmask,0,16);memset(stream,0,16);memset(gh,0,16);memset(gv,0,16);memset(gz,0,16);}
