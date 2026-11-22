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
/* TLS 1.2 AES128-GCM with precomputed RSA key transport.
 * Exactly one independently pinned server key. No application data may flow
 * before certificate pin AND server Finished MAC both verify.
 * Static RSA does not provide forward secrecy; TLS 1.3 remains the preferred
 * profile if the server handshake deadline can accommodate the native CPU.
 */
#include <string.h>
#include <time.h>
#include "client.h"
#include "rsa4096.h"
#pragma code-name ("TLS12_CODE")
#pragma bss-name ("TLS12_BSS")
typedef unsigned char u8;
extern unsigned char transcript[32];
extern unsigned char random_byte(void),tcp_open(void),tcp_send(void);
extern int tcp_byte(void);
extern void reseed(void),transcript_init(void),transcript_update(void),transcript_hash(void),rsa_public(void);
extern u8 gcm(const u8*,const u8*,const u8*,u8*,unsigned int,u8*,u8);
extern void gcm_clear(void),tcp_close(void);
extern u8 encoded[512];
static u8 rec[1100],pms[48],cr[32],sr[32],master[48],keys[40],seqw[8],seqr[8];
static u8 hb[192],pad[64],inner[32],pseed[100],pa[32],prout[32],nonce[12],aad[13],hshead[4];
unsigned int rlen,rpos;
static unsigned int hslen,hr;
u8 wireheader[5];
static u8 rtype,encrypted_read,authenticated,ems;
u8 tls_error,hs_type;
volatile u8 tls12_stage;
volatile u8 crypto_tests;
volatile u8 tls12_fault;
u8 tls12_connections,application_records;
/* RAM-only cache, bound to this single compiled hostname/pin/cipher/EMS/MFL.
 * Publish only after Finished verification. Never extend its original age.
 * Each resumed connection gets new randoms, traffic keys and sequence numbers.
 */
static u8 cached_id[32],cached_master[48],peer_id[32],peer_len,tcp_live;
u8 tls12_session_len,tls12_resumed,tls12_fallbacks;
static clock_t session_created;
static unsigned int be(const u8*p){return ((unsigned int)p[0]<<8)|p[1];}
static void put(u8*p,unsigned int n){p[0]=n>>8;p[1]=n;}
static void hash(const u8*p,unsigned int n){hash_ptr=(u8*)p;hash_len=n;crypto_hash();}
static void update(const u8*p,unsigned int n){hash_ptr=(u8*)p;hash_len=n;transcript_update();}
static void hmac(const u8*k,u8 kn,const u8*d,unsigned int n,u8*out){
 u8 i;memset(pad,54,64);for(i=0;i<kn;i++)pad[i]^=k[i];memcpy(hb,pad,64);memcpy(hb+64,d,n);hash(hb,64+n);memcpy(inner,hash_out,32);
 memset(pad,92,64);for(i=0;i<kn;i++)pad[i]^=k[i];memcpy(hb,pad,64);memcpy(hb+64,inner,32);hash(hb,96);memcpy(out,hash_out,32);
}
static void prf(const u8*k,u8 kn,const char*label,const u8*seed,u8 sn,u8*out,u8 count){
 u8 ln=strlen(label),n,i,pos=0;
 memcpy(pseed,label,ln);memcpy(pseed+ln,seed,sn);n=ln+sn;
 hmac(k,kn,pseed,n,pa);
 while(pos<count){memcpy(rec,pa,32);memcpy(rec+32,pseed,n);hmac(k,kn,rec,32+n,prout);for(i=0;i<32&&pos<count;i++)out[pos++]=prout[i];hmac(k,kn,pa,32,pa);}
}
#pragma code-name ("CODE")
static u8 selftest(void){
 static const u8 expected[32]={0x03,0x88,0xda,0xce,0x60,0xb6,0xa3,0x92,0xf3,0x28,0xc2,0xb9,0x71,0xb2,0xfe,0x78,0x6f,0x24,0x48,0xa6,0x59,0xe0,0xca,0x85,0x95,0x59,0xc5,0xb0,0xcd,0x68,0x6d,0x41};
 static const u8 mac[32]={0xb0,0x34,0x4c,0x61,0xd8,0xdb,0x38,0x53,0x5c,0xa8,0xaf,0xce,0xaf,0x0b,0xf1,0x2b,0x88,0x1d,0xc2,0x00,0xc9,0x83,0x3d,0xa7,0x26,0xe9,0x37,0x6c,0x2e,0x32,0xcf,0xf7};
 u8 i;
 crypto_tests=0;memset(keys,0,40);memset(nonce,0,12);memset(aad,0,13);memset(rec,0,32);
 if(!gcm(keys,nonce,aad,rec,16,rec+16,0)||memcmp(rec,expected,32))return 0;++crypto_tests;
 if(!gcm(keys,nonce,aad,rec,16,rec+16,1))return 0;for(i=0;i<16;i++)if(rec[i])return 0;++crypto_tests;
 memcpy(rec,expected,32);rec[16]^=1;if(gcm(keys,nonce,aad,rec,16,rec+16,1))return 0;for(i=0;i<16;i++)if(rec[i])return 0;++crypto_tests;
 memset(keys,11,20);hmac(keys,20,(const u8*)"Hi There",8,prout);if(memcmp(prout,mac,32))return 0;++crypto_tests;
 return 1;
}
#pragma code-name ("TLS12_CODE")
static int byte(void){
 int b;clock_t start=clock();
 if(tls_error)return 0;
 do{b=tcp_byte();if(b>=0)return b;}while(clock()-start<8*CLOCKS_PER_SEC);
 tls_error=1;return 0;
}
static void incseq(u8*s){u8 i=8;while(i){--i;if(++s[i])return;}tls_error=1;}
static void aead_aad(const u8*s,u8 type,unsigned int n){memcpy(aad,s,8);aad[8]=type;aad[9]=3;aad[10]=3;put(aad+11,n);}
static u8 readrec(void){
 unsigned int i,n;u8 header[5];
 for(i=0;i<5;i++)header[i]=byte();
 if(tls_error)return 0;
 memcpy(wireheader,header,5);
 rtype=header[0];rlen=be(header+3);rpos=0;
 if(header[1]!=3||header[2]!=3||rlen>(encrypted_read?536:512)||!rlen){tls_error=2;return 0;}
 for(i=0;i<rlen;i++)rec[i]=byte();
 if(tls_error)return 0;
 if(encrypted_read){
  if(rlen<24){tls_error=3;return 0;}n=rlen-24;
  memcpy(nonce,keys+36,4);memcpy(nonce+4,rec,8);aead_aad(seqr,rtype,n);
  if(!gcm(keys+16,nonce,aad,rec+8,n,rec+8+n,1)){tls_error=4;return 0;}
  memmove(rec,rec+8,n);rlen=n;incseq(seqr);
 }
 if(rtype==21){tls_error=5;return 0;}return 1;
}
static u8 sendrec(u8 type,const u8*p,unsigned int n,u8 encrypt){
 unsigned int len=n;u8 head[5];
 if(n>512)return 0;
 if(encrypt){
  memmove(rec+8,p,n);memcpy(rec,seqw,8);memcpy(nonce,keys+32,4);memcpy(nonce+4,seqw,8);aead_aad(seqw,type,n);
  if(!gcm(keys,nonce,aad,rec+8,n,rec+8+n,0))return 0;
  len=n+24;p=rec;incseq(seqw);
 }
 head[0]=type;head[1]=3;head[2]=3;put(head+3,len);
 tls_app_ptr=head;tls_app_len=5;if(!tcp_send())return 0;
 tls_app_ptr=(u8*)p;tls_app_len=len;return tcp_send();
}
static u8 hsbyte(void){
 u8 b;
 if(tls_error)return 0;
 if(rpos==rlen){if(!readrec())return 0;if(rtype!=22){tls_error=6;return 0;}}
 b=rec[rpos++];update(&b,1);return b;
}
static u8 next_hs(u8 type){
 u8 i;for(i=0;i<4;i++)hshead[i]=hsbyte();hs_type=hshead[0];
 if(hshead[1]||hs_type!=type){tls_error=7;return 0;}hslen=be(hshead+2);return !tls_error;
}
static u8 serverhello(void){
 unsigned int i,n,extensions;u8 sid,t,idhi,idlo,sz,seen=0,bit;
 if(!next_hs(2)||hslen<38)return 0;hr=hslen;
 if(hsbyte()!=3||hsbyte()!=3)return 0;hr-=2;
 for(i=0;i<32;i++)sr[i]=hsbyte();hr-=32;
 sid=hsbyte();--hr;if(sid>32||hr<sid+3)return 0;peer_len=sid;
 for(i=0;i<sid;i++)peer_id[i]=hsbyte();hr-=sid;
 tls12_resumed=sid&&sid==tls12_session_len&&!memcmp(peer_id,cached_id,sid);
 if(hsbyte()!=0||hsbyte()!=156||hsbyte()!=0)return 0;hr-=3;
 if(hr<2)return 0;extensions=(unsigned int)hsbyte()<<8;extensions|=hsbyte();hr-=2;
 if(extensions!=hr)return 0;t=0;
 while(hr){if(hr<4)return 0;idhi=hsbyte();idlo=hsbyte();n=(unsigned int)hsbyte()<<8;n|=hsbyte();hr-=4;if(n>hr)return 0;
  bit=0;
  if(!idhi&&idlo==1)bit=1;else if(!idhi&&idlo==23)bit=2;else if(idhi==255&&idlo==1)bit=4;else if(!idhi&&!idlo)bit=8;
  if(!bit||(seen&bit))return 0;seen|=bit;
  if(!idhi&&idlo==1){if(n!=1)return 0;sz=hsbyte();if(sz!=1)return 0;t=1;}
  else if(!idhi&&idlo==23){if(n)return 0;ems=1;}
  else if(idhi==255&&idlo==1){if(n!=1||hsbyte()!=0)return 0;}
  else if(n)return 0;hr-=n;
 }
 return t&&!tls_error&&(seen&6)==6;
}
static u8 certificate(void){
 unsigned int total,n,i;u8 first=1;
 if(!next_hs(11)||hslen<3)return 0;
 if(hsbyte()!=0)return 0;total=(unsigned int)hsbyte()<<8;total|=hsbyte();
 if(total!=hslen-3)return 0;
 while(total){if(total<3||hsbyte()!=0)return 0;n=(unsigned int)hsbyte()<<8;n|=hsbyte();total-=3;if(!n||n>total)return 0;
  if(first){if(n>2048)return 0;cert_buf_len=n;for(i=0;i<n;i++)cert_buf[i]=hsbyte();if(tls12_fault&1)cert_buf[0]^=1;if(!rsa_check_certificate(cert_buf,n))return 0;first=0;}
  else for(i=0;i<n;i++)hsbyte();total-=n;
 }
 return !first&&!tls_error;
}
#pragma code-name ("CODE")
static u8 hello(void){
 unsigned int i,p=4,e;
 rec[p++]=3;rec[p++]=3;memcpy(rec+p,cr,32);p+=32;rec[p++]=tls12_session_len;
 memcpy(rec+p,cached_id,tls12_session_len);
 if(tls12_session_len&&(tls12_fault&4))rec[p]^=1; /* test a server cache miss */
 p+=tls12_session_len;
 rec[p++]=0;rec[p++]=2;rec[p++]=0;rec[p++]=156;rec[p++]=1;rec[p++]=0;
 e=p;p+=2;
 rec[p++]=0;rec[p++]=0;put(rec+p,5+tls_hostname_len);p+=2;put(rec+p,3+tls_hostname_len);p+=2;rec[p++]=0;put(rec+p,tls_hostname_len);p+=2;memcpy(rec+p,tls_hostname,tls_hostname_len);p+=tls_hostname_len;
 rec[p++]=0;rec[p++]=1;rec[p++]=0;rec[p++]=1;rec[p++]=1;
 rec[p++]=0;rec[p++]=13;rec[p++]=0;rec[p++]=6;rec[p++]=0;rec[p++]=4;rec[p++]=4;rec[p++]=1;rec[p++]=8;rec[p++]=4;
 rec[p++]=255;rec[p++]=1;rec[p++]=0;rec[p++]=1;rec[p++]=0;
 rec[p++]=0;rec[p++]=23;rec[p++]=0;rec[p++]=0;
 put(rec+e,p-e-2);rec[0]=1;rec[1]=0;put(rec+2,p-4);
 update(rec,p);i=p;return sendrec(22,rec,i,0);
}
#pragma code-name ("TLS12_CODE")
static u8 send_finished(void){
 u8 b=1,verify[12];
 if(!sendrec(20,&b,1,0))return 0;
 transcript_hash();prf(master,48,"client finished",transcript,32,verify,12);
 rec[0]=20;rec[1]=0;rec[2]=0;rec[3]=12;memcpy(rec+4,verify,12);update(rec,16);
 return sendrec(22,rec,16,1);
}
static u8 receive_finished(void){
 u8 b,i,verify[12];
 tls12_stage=6;rlen=rpos=0;
 if(!readrec()||rtype!=20||rlen!=1||rec[0]!=1)return 0;
 encrypted_read=1;rlen=rpos=0;
 transcript_hash();prf(master,48,"server finished",transcript,32,verify,12);
 if(!readrec()||rtype!=22||rlen!=16||rec[0]!=20||rec[1]||rec[2]||rec[3]!=12)return 0;
 if(tls12_fault&2)rec[4]^=1;
 b=0;for(i=0;i<12;i++)b|=verify[i]^rec[4+i];if(b)return 0;
 update(rec,16);return 1;
}
static u8 handshake(void){
 unsigned int i;u8 b;
 authenticated=encrypted_read=tls_error=ems=tls_state=application_records=tls12_resumed=0;rlen=rpos=0;memset(seqw,0,8);memset(seqr,0,8);tls12_stage=1;reseed();
 if(!selftest()){tls12_stage=239;return 0;}
 for(i=0;i<32;i++)cr[i]=random_byte();
 if(!tls12_session_len){
 for(i=0;i<48;i++)pms[i]=random_byte();pms[0]=3;pms[1]=3;
 encoded[0]=0;encoded[1]=2;
 for(i=2;i<463;i++){do{b=random_byte();}while(!b);encoded[i]=b;}
 encoded[463]=0;memcpy(encoded+464,pms,48);rsa_signature=encoded;rsa_signature_len=512;
 tls12_stage=2;rsa_public();if(!rsa_result)return 0;
 }
 tls12_stage=3;if(!tcp_open())return 0;tcp_live=1;transcript_init();if(!hello())return 0;
 tls12_stage=4;if(!serverhello())return 0;
 if(tls12_session_len&&!tls12_resumed)return 2;
 if(tls12_resumed)memcpy(master,cached_master,48);
 else{
 if(!certificate()||!next_hs(14)||hslen)return 0;
 tls12_stage=5;
 hshead[0]=16;hshead[1]=0;hshead[2]=2;hshead[3]=2;update(hshead,4);update((u8*)"\x02\x00",2);update(encoded,512);
 if(!ems)return 0;
 transcript_hash();prf(pms,48,"extended master secret",transcript,32,master,48);memset(pms,0,48);
 }
 /* No bytes of another handshake message may remain behind ServerHello/Done. */
 if(rpos!=rlen)return 0;
 memcpy(rec,sr,32);memcpy(rec+32,cr,32);prf(master,48,"key expansion",rec,64,keys,40);
 if(tls12_resumed){
  if(!receive_finished()||!send_finished())return 0;
 }else{
 memcpy(rec,hshead,4);rec[4]=2;rec[5]=0;memcpy(rec+6,encoded,506);
 if(!sendrec(22,rec,512,0)||!sendrec(22,encoded+506,6,0))return 0;memset(encoded,0,512);
 if(!send_finished()||!receive_finished())return 0;
 memcpy(cached_master,master,48);memcpy(cached_id,peer_id,peer_len);
 session_created=clock();tls12_session_len=peer_len;
 }
 authenticated=1;++tls12_connections;tls12_stage=7;tls_state=7;memset(master,0,48);return 1;
}
#pragma code-name ("RESUME_CODE")
void connection_forget(void){
 tls12_session_len=0;memset(cached_master,0,48);memset(cached_id,0,32);
}
unsigned char connection_open(void){
 u8 result;
 if(tls12_session_len&&clock()-session_created>=1800UL*CLOCKS_PER_SEC)connection_forget();
 result=handshake();
 if(result==2){
  /* A declined session needs a new full handshake. Close before the slow
   * fresh RSA precomputation so no server handshake timeout can expire. */
  connection_close();connection_forget();++tls12_fallbacks;result=handshake();
 }
 if(!result){connection_close();connection_forget();}
 return result;
}
#pragma code-name ("TLS12_CODE")
unsigned char send_tls(void){
 u8*p=tls_app_ptr;unsigned int n=tls_app_len;
 if(!authenticated||n>512)return 0;++application_records;return sendrec(23,p,n,1);
}
unsigned char receive_tls(void){
 if(!authenticated)return 0;
 if(!readrec()){
  /* Fatal protocol/authentication errors invalidate resumability as well.
   * Ordinary EOF/timeout or close_notify may still resume the session. */
  if(tls_error==2||tls_error==3||tls_error==4||(tls_error==5&&rec[0]==2))connection_forget();
  tls_state=255;authenticated=0;return 0;
 }
 if(rtype!=23)return 0;tls_app_ptr=rec;tls_app_len=rlen;return 1;
}
#pragma code-name ("CODE")
void connection_close(void){
 if(tcp_live){tcp_live=0;tcp_close();}authenticated=0;
 memset(pms,0,48);memset(master,0,48);memset(keys,0,40);memset(encoded,0,512);memset(rec,0,sizeof(rec));
 memset(hb,0,sizeof(hb));memset(inner,0,32);memset(pad,0,64);memset(pa,0,32);memset(prout,0,32);gcm_clear();
}
