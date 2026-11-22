"""Compile the real C++ UCI adapter against upstream CommandTarget ABI.
Only networking and the separately tested TLS core are stubbed here.
"""
from pathlib import Path
import subprocess,json,os
r=Path(__file__).resolve().parent;b=r/'build';shim=b/'shims';shim.mkdir(exist_ok=True)
(shim/'command_target.h').write_bytes((r/'upstream/software_io_command_interface_command_target.h').read_bytes())
(shim/'socket.h').write_text('''#pragma once
#include <stddef.h>
#include <stdint.h>
struct timeval { long tv_sec, tv_usec; };
struct in_addr { uint32_t s_addr; };
struct sockaddr { unsigned short family; char data[14]; };
struct sockaddr_in { short sin_family; unsigned short sin_port; in_addr sin_addr; };
#define AF_INET 2
#define SOCK_STREAM 1
#define SOL_SOCKET 1
#define SO_RCVTIMEO 2
#define SO_SNDTIMEO 3
int socket(int,int,int); int close(int);
int recv(int,void*,size_t,int);int send(int,const void*,size_t,int);
int setsockopt(int,int,int,const void*,size_t);
int connect(int,const sockaddr*,size_t);
unsigned short htons(unsigned short);
''')
(shim/'netdb.h').write_text('''#pragma once
#include <stddef.h>
struct hostent { char *h_addr; int h_length; };
int gethostbyname_r(const char*,hostent*,char*,size_t,hostent**,int*);
''')
(b/'target_test.cc').write_text('''#include "command_target.h"
#include "krs_tls.h"
#include "socket.h"
#include "netdb.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
CommandTarget *command_targets[16]={};
Message c_message_no_target={0,true,NULL},c_status_ok={0,true,NULL},c_status_unknown_command={0,true,NULL},c_message_empty={0,true,NULL};
static int closes=0,opens=0,writes=0;static bool bad=false;
int socket(int,int,int){++opens;return 42;}
int close(int n){assert(n==42);++closes;return 0;}
int recv(int,void*,size_t,int){return -1;}
int send(int,const void*,size_t,int){return -1;}
int setsockopt(int,int,int,const void*p,size_t n){assert(n==sizeof(timeval)&&((const timeval*)p)->tv_sec==8);return 0;}
int connect(int,const sockaddr*,size_t){return 0;}
unsigned short htons(unsigned short n){return (n>>8)|(n<<8);}
int gethostbyname_r(const char *name,hostent*h,char*,size_t,hostent**out,int*){
 assert(!strcmp(name,"quipzz.bplaced.net"));static char ip[4]={1,2,3,4};h->h_addr=ip;h->h_length=4;*out=h;return 0;
}
extern "C" void krs_tls_clear(krs_tls*s){memset(s,0,sizeof(*s));}
extern "C" int krs_tls_open(krs_tls*s,const unsigned char*p,size_t n,const br_rsa_public_key*k,krs_io_fn,krs_write_fn,void*){
 assert(n==32&&p[0]==7&&k->nlen==512&&(k->n[511]&1));s->authenticated=!bad;return !bad;
}
extern "C" int krs_tls_write(krs_tls*s,const unsigned char*,size_t n){assert(s->authenticated&&n<=512);++writes;return 1;}
extern "C" int krs_tls_read(krs_tls*s,unsigned char*p,size_t){assert(s->authenticated);memcpy(p,"hello",5);return 5;}
#include "krs_tls_target.cc"
int main(){
 CommandTarget*t=command_targets[14];assert(t);unsigned char buf[896]={14,1};
 Message c={2,true,buf},*r,*s;
 t->parse_command(&c,&r,&s);assert(r->length==8&&!memcmp(r->message,"KRSTLS/1",8));
 buf[1]=0x10;t->parse_command(&c,&r,&s);assert(s->message[0]=='4'&&opens==0);
 buf[1]=2;c.length=33;t->parse_command(&c,&r,&s);assert(s->message[0]=='4');
 c.length=34;memset(buf+2,7,32);t->parse_command(&c,&r,&s);assert(s->message[0]=='0');
 buf[1]=0x10;c.length=2;t->parse_command(&c,&r,&s);assert(s->message[0]=='0'&&opens==1);
 buf[1]=0x11;c.length=515;t->parse_command(&c,&r,&s);assert(s->message[0]=='4'&&writes==0);
 c.length=514;t->parse_command(&c,&r,&s);assert(s->message[0]=='0'&&writes==1);
 c.length=2;buf[1]=0x12;t->parse_command(&c,&r,&s);assert(r->length==5&&r->last_part&&!memcmp(r->message,"hello",5));
 t->c64_reset();assert(closes==1);
 buf[1]=0x11;c.length=3;t->parse_command(&c,&r,&s);assert(s->message[0]=='5'&&writes==1);
 buf[1]=0x10;c.length=2;t->parse_command(&c,&r,&s);assert(s->message[0]=='4'&&opens==1);
 buf[1]=2;c.length=34;memset(buf+2,7,32);t->parse_command(&c,&r,&s);
 bad=true;buf[1]=0x10;c.length=2;t->parse_command(&c,&r,&s);assert(s->message[0]=='5'&&closes==2);
 buf[1]=0x10;t->parse_command(&c,&r,&s);assert(s->message[0]=='4'&&opens==2);
 c.length=1;t->parse_command(&c,&r,&s);assert(s->message[0]=='4');
 puts("UCI adapter ABI, seed gating, length limits, reset, failed TLS and socket ownership: passed");
}
''')
env=os.environ.copy();env['ZIG_GLOBAL_CACHE_DIR']=str(b/'zig-global');env['TEMP']=env['TMP']=str(b/'tmp')
cmd=[str(r/'toolchain/ziglang/zig.exe'),'c++','-std=c++11','-fno-exceptions','-fno-rtti','-I',str(shim),'-I',str(r),'-I',str(r/'bearssl-0.6/inc'),str(b/'target_test.cc'),'-o',str(b/'target_test.exe')]
subprocess.run(cmd,check=True,env=env)
subprocess.run([str(b/'target_test.exe')],check=True)
(b/'target-test.json').write_text(json.dumps({'compiled_against_upstream_CommandTarget_ABI':True,'seed_required_and_single_use':True,'bounds_checked':True,'failed_TLS_socket_closed':True,'C64_reset_closes_and_wipes':True,'network_and_TLS_stubbed_for_adapter_test':True,'Nios_cross_compile_performed':False},indent=2))
