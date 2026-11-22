from pathlib import Path
import subprocess, concurrent.futures, shutil, os, re
r=Path(__file__).resolve().parent
zig=r/'toolchain/ziglang/zig.exe'; bear=r/'bearssl-0.6'; b=r/'build'; b.mkdir(exist_ok=True)
env=os.environ.copy()
for key,part in [('ZIG_GLOBAL_CACHE_DIR','zig-global'),('ZIG_LOCAL_CACHE_DIR','zig-local'),('TEMP','tmp'),('TMP','tmp')]:
    path=b/part;path.mkdir(exist_ok=True);env[key]=str(path)
# Native 6502 Montgomery integers are little endian; BearSSL expects BE.
original=(r.parent/'krs-client/rsa_pin.h').read_text()
modulus=bytes(int(x) for x in re.findall(r'\d+',original.split('{')[1].split('}')[0]))[::-1]
assert len(modulus)==512 and modulus[-1]&1
pin_text='/* Big-endian public K.R.S. RSA-4096 modulus. */\nstatic const unsigned char rsa_modulus[512]={\n'+','.join(str(x) for x in modulus)+'\n};\n'
if not (r/'rsa_pin.h').exists() or (r/'rsa_pin.h').read_text()!=pin_text:(r/'rsa_pin.h').write_text(pin_text)
flags=['-O2','-std=c99','-DBR_USE_WIN32_RAND=0','-DBR_USE_WIN32_TIME=0',
       '-DBR_USE_URANDOM=0','-DBR_RDRAND=0','-DBR_AES_X86NI=0',
       '-DBR_SSE2=0','-DBR_INT128=0','-DBR_POWER8=0',
       '-I',str(bear/'inc'),'-I',str(bear/'src'),'-I',str(r)]
sources=list((bear/'src').rglob('*.c'))+[r/'krs_tls.c',r/'host_probe.c']
header_time=max(p.stat().st_mtime for p in [r/'krs_tls.h',r/'rsa_pin.h',*list((bear/'inc').glob('*.h'))])
def compile_one(src):
    obj=b/(str(src.relative_to(r)).replace('/','_').replace('\\','_')+'.o')
    if not obj.exists() or max(src.stat().st_mtime,header_time)>obj.stat().st_mtime:
        subprocess.run([str(zig),'cc',*flags,'-c',str(src),'-o',str(obj)],check=True,capture_output=True,env=env)
    return obj
with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:
    try: objects=list(pool.map(compile_one,sources))
    except subprocess.CalledProcessError as e:
        print(e.stderr.decode(errors='replace'));raise
rsp=b/'link.rsp'
rsp.write_text('\n'.join('"'+str(o).replace('\\','/')+'"' for o in objects)+'\n-lws2_32\n-o\n"'+str(b/'host_probe.exe').replace('\\','/')+'"\n')
subprocess.run([str(zig),'cc','@'+str(rsp)],check=True,env=env)
print('Compiled portable TLS core and host probe:',len(objects),'objects',flush=True)
