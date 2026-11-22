from pathlib import Path
import argparse,subprocess
p=argparse.ArgumentParser();p.add_argument('--cc65',required=True,help='Path to cc65 directory containing bin and lib');a=p.parse_args()
r=Path(__file__).parent;s=r/'src';b=r/'build';b.mkdir(exist_ok=True);t=Path(a.cc65)
def run(a):subprocess.run([str(x) for x in a],check=True)
names=['boot_tls12.s','tls12_bridge.s','bitmap_bridge.s','editor_bridge.s','hash_bridge.s','rsa_fast.s','data.s','der_decode.s','entropy.s','loadaddr.s','reu_exec.s','tls_transcript.s','crypto/sha256.s','crypto/word32.s','crypto/hmac_drbg.s','net/uci/net.s','net/uci/uci_cmd.s','net/uci/net_manifest.s']
objects=[]
for n in names:
 o=b/(n.replace('/','_')+'.o');run([t/'bin/ca65.exe','-D','BACKEND_UCI=1','-I',s,'-I',s/'crypto/shared','-I',s/'net','-I',s/'net/uci','-o',o,s/n]);objects.append(o)
for n in ['rsa4096','client','protocol','terminal','tls12','aesgcm','bitmap','editor']:
 asm=b/(n+'.s');o=b/(n+'.o');run([t/'bin/cc65.exe','-t','c64','-Oirs','-D','RSA_FAST=1','-D','TLS12_ONLY=1','-I',s,'-o',asm,s/(n+'.c')]);run([t/'bin/ca65.exe','-t','c64','-o',o,asm]);objects.append(o)
run([t/'bin/ld65.exe','-C',r/'client.cfg','-o',b/'KRS-CLIENT.PRG',*objects,t/'lib/c64.lib'])
