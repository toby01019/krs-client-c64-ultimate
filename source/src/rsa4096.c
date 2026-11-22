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
/* RSA-4096 / e=65537 with SHA-256 PSS, fixed 32-byte salt (TLS 1.3).
 * Authenticate only the pinned, independently CA-validated K.R.S. key.
 * All attacker-controlled lengths and encodings fail closed.
 */
#include <string.h>
#include "rsa4096.h"
#include "rsa_pin.h"

#ifdef RSA_TEST
static unsigned char workspace[2048];
#else
extern unsigned char cert_buf[2048];
#define workspace cert_buf
#endif
/* The leaf certificate is dead once its pinned key was checked. */
#define tmp workspace
#define base (workspace+520)
#define value (workspace+1032)
unsigned char rsa_message_hash[32],rsa_result;
volatile unsigned char rsa_progress;
unsigned char *rsa_signature,*hash_ptr;
unsigned int rsa_signature_len,hash_len;
unsigned char hash_out[32];
#ifndef TLS12_ONLY
static unsigned char h[32],hash_block[72];
#endif
#ifdef RSA_FAST
unsigned char *rsa_row_a,*rsa_row_t;
unsigned char rsa_row_b,rsa_row_shift,rsa_row_carry;
void rsa_fastrow(void);
#endif

static int compare(const unsigned char*a,const unsigned char*b) {
    unsigned int j=RSA_BYTES;
    while(j){--j;if(a[j]!=b[j])return a[j]>b[j]?1:-1;}
    return 0;
}
static void mont(unsigned char*out,const unsigned char*a,const unsigned char*b) {
    unsigned int i,j,z,carry;
    unsigned char factor,bi;
    memset(tmp,0,RSA_BYTES+2);
    for(i=0;i<RSA_BYTES;i++){
        carry=0;bi=b[i];
#ifdef RSA_FAST
        rsa_row_a=(unsigned char*)a;rsa_row_t=tmp;rsa_row_b=bi;rsa_row_shift=0;
        rsa_fastrow();carry=rsa_row_carry;
#else
        for(j=0;j<RSA_BYTES;j++){
            z=(unsigned int)a[j]*bi+tmp[j]+carry;
            tmp[j]=(unsigned char)z;carry=z>>8;
        }
#endif
        z=tmp[RSA_BYTES]+carry;tmp[RSA_BYTES]=(unsigned char)z;tmp[RSA_BYTES+1]=z>>8;
        factor=(unsigned char)(tmp[0]*RSA_NPRIME);
        carry=0;
#ifdef RSA_FAST
        rsa_row_a=(unsigned char*)rsa_modulus;rsa_row_t=tmp;rsa_row_b=factor;rsa_row_shift=1;
        rsa_fastrow();carry=rsa_row_carry;
#else
        for(j=0;j<RSA_BYTES;j++){
            z=(unsigned int)rsa_modulus[j]*factor+tmp[j]+carry;
            if(j)tmp[j-1]=(unsigned char)z;
            carry=z>>8;
        }
#endif
        z=tmp[RSA_BYTES]+carry;
        tmp[RSA_BYTES-1]=(unsigned char)z;
        tmp[RSA_BYTES]=tmp[RSA_BYTES+1]+(z>>8);
        tmp[RSA_BYTES+1]=0;
    }
    if(tmp[RSA_BYTES]||compare(tmp,rsa_modulus)>=0){
        carry=0;
        for(j=0;j<RSA_BYTES;j++){
            z=(unsigned int)tmp[j]-rsa_modulus[j]-carry;
            tmp[j]=(unsigned char)z;carry=(z>>8)&1;
        }
    }
    memcpy(out,tmp,RSA_BYTES);
    ++rsa_progress;
}
#ifndef TLS12_ONLY
static void hash(unsigned char*p,unsigned int len){hash_ptr=p;hash_len=len;crypto_hash();}
#endif
/* Fixed public operation also used for TLS 1.2 RSA key transport.
 * Caller supplies an independently randomized PKCS#1 v1.5 encoded message.
 * Compute before opening TCP, then require the pinned peer's Finished MAC.
 */
void rsa_public(void){
 unsigned int j;
 unsigned char k;
 rsa_result=0;rsa_progress=0;
 if(rsa_signature_len!=RSA_BYTES)return;
 for(j=0;j<RSA_BYTES;j++)base[j]=rsa_signature[RSA_BYTES-1-j];
 if(compare(base,rsa_modulus)>=0)return;
 mont(value,base,rsa_r2);memcpy(base,value,RSA_BYTES);
 for(k=0;k<16;k++)mont(value,value,value);
 mont(value,value,base);memset(base,0,RSA_BYTES);base[0]=1;
 mont(value,value,base);
 for(j=0;j<RSA_BYTES;j++)rsa_signature[j]=value[RSA_BYTES-1-j];
 rsa_result=1;
}
#ifndef TLS12_ONLY
void rsa_verify(void) {
    unsigned int j,k,offset;
    unsigned char pass,ctr,copy;
    rsa_result=0;rsa_progress=0;
    if(rsa_signature_len!=RSA_BYTES)return;
    for(j=0;j<RSA_BYTES;j++)base[j]=rsa_signature[RSA_BYTES-1-j];
    if(compare(base,rsa_modulus)>=0)return;
    mont(value,base,rsa_r2);
    memcpy(base,value,RSA_BYTES);
    for(copy=0;copy<16;copy++)mont(value,value,value);
    mont(value,value,base);
    memset(base,0,RSA_BYTES);base[0]=1;
    mont(value,value,base);
    for(j=0;j<RSA_BYTES;j++)base[j]=value[RSA_BYTES-1-j];
    if(base[RSA_BYTES-1]!=188||(base[0]&128))return;
    memcpy(h,base+RSA_BYTES-33,32);
    memcpy(hash_block,h,32);memset(hash_block+32,0,4);
    offset=0;ctr=0;
    while(offset<RSA_BYTES-33){
        hash_block[35]=ctr++;hash(hash_block,36);
        for(k=0;k<32&&offset<RSA_BYTES-33;k++,offset++)base[offset]^=hash_out[k];
    }
    base[0]&=127;
    pass=0;
    for(j=0;j<RSA_BYTES-66;j++)pass|=base[j];
    if(pass||base[RSA_BYTES-66]!=1)return;
    memset(hash_block,0,8);memcpy(hash_block+8,rsa_message_hash,32);
    memcpy(hash_block+40,base+RSA_BYTES-65,32);hash(hash_block,72);
    pass=0;for(j=0;j<32;j++)pass|=h[j]^hash_out[j];
    rsa_result=pass==0;
}
#endif
/* Bounded, canonical DER TLV. The cursor becomes the content start. */
static unsigned int tlv(const unsigned char*p,unsigned int*cursor,unsigned int end,unsigned char tag){
    unsigned int len,pos=*cursor;
    unsigned char b;
    if(pos>=end||p[pos++]!=tag||pos>=end)return 0;
    b=p[pos++];len=b;
    if(b&128){
        if(b==129){if(pos>=end||p[pos]<128)return 0;len=p[pos++];}
        else if(b==130){if(end-pos<2||!p[pos])return 0;len=((unsigned int)p[pos]<<8)|p[pos+1];pos+=2;}
        else return 0;
    }
    if(len>end-pos)return 0;
    *cursor=pos;return pos+len;
}
unsigned char rsa_check_certificate(const unsigned char*p,unsigned int len){
    static const unsigned char oid[]={6,9,42,134,72,134,247,13,1,1,1,5,0};
    unsigned int pos=0,outer,tbs,spki,algorithm,bits,key,n,end,j;
    outer=tlv(p,&pos,len,48);if(!outer||outer!=len)return 0;
    tbs=tlv(p,&pos,outer,48);if(!tbs)return 0;
    if(p[pos]==160){end=tlv(p,&pos,tbs,160);if(!end)return 0;pos=end;}
    for(j=0;j<5;j++){end=tlv(p,&pos,tbs,j==0?2:48);if(!end)return 0;pos=end;}
    spki=tlv(p,&pos,tbs,48);if(!spki)return 0;
    algorithm=tlv(p,&pos,spki,48);
    if(!algorithm||algorithm-pos!=sizeof(oid)||memcmp(p+pos,oid,sizeof(oid)))return 0;
    pos=algorithm;bits=tlv(p,&pos,spki,3);
    if(!bits||bits!=spki||pos>=bits||p[pos++]!=0)return 0;
    key=tlv(p,&pos,bits,48);if(!key||key!=bits)return 0;
    n=tlv(p,&pos,key,2);if(!n||n-pos!=RSA_BYTES+1||p[pos++]!=0)return 0;
    for(j=0;j<RSA_BYTES;j++)if(p[pos+j]!=rsa_modulus[RSA_BYTES-1-j])return 0;
    pos=n;end=tlv(p,&pos,key,2);
    return end==key&&end-pos==3&&p[pos]==1&&p[pos+1]==0&&p[pos+2]==1;
}
