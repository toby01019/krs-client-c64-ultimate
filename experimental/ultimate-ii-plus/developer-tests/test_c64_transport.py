"""Run the compiled 6510 offload client against a strict UCI register model.
No socket/TLS arithmetic on the 6510; the module itself is tested separately.
"""
from pathlib import Path
from collections import deque
import sys,re,json
r=Path(__file__).resolve().parent;b=r/'c64/tls12-build'
sys.path.insert(0,str(r.parent/'krs-client/py65'))
from py65.devices.mpu6502 import MPU
labels={m[2]:int(m[1],16) for m in re.finditer(r'al ([0-9a-fA-F]+) \.([\w]+)',(b/'client.lbl').read_text())}
prg=(b/'KRS-CLIENT.PRG').read_bytes();load=int.from_bytes(prg[:2],'little')
class Memory(list):
 def __init__(self):
  super().__init__([0]*65536);self[load:load+len(prg)-2]=prg[2:]
  self.cmd=bytearray();self.data=deque();self.status=deque();self.ready=False
  self.commands=[];self.present=True;self.fail_open=False;self.multipart=False
  self.read_payload=b'HTTP/1.1 200 OK\r\n\r\nhello'
 def __getitem__(self,a):
  if isinstance(a,slice):return super().__getitem__(a)
  assert a!=0xd031,'Turbo dependency'
  if a==0xdf1d:return 0xc9
  if a==0xdf1c:return (0x30 if self.multipart else 0x20 if self.ready else 0)|(0x80 if self.data else 0)|(0x40 if self.status else 0)
  if a==0xdf1e:return self.data.popleft()
  if a==0xdf1f:return self.status.popleft()
  return super().__getitem__(a)
 def __setitem__(self,a,v):
  if isinstance(a,slice):return super().__setitem__(a,v)
  assert a!=0xd031,'Turbo dependency'
  if a==0xdf1d:self.cmd.append(v);return
  if a==0xdf1c:
   if v in [2,4]:self.ready=False;self.data.clear();self.status.clear();self.cmd.clear();self.multipart=False
   elif v==1:
    c=bytes(self.cmd);self.cmd.clear();self.commands.append(c)
    assert c[0]==14 and len(c)<=514
    payload=b'';status=b'000 OK'
    if c[1]==1:payload=b'KRSTLS/1' if self.present else b'OTHER/00'
    elif c[1]==2:assert len(c)==34 and len(set(c[2:]))>1
    elif c[1]==0x10:
     if self.fail_open:status=b'502 TLS FAILED'
    elif c[1]==0x12:payload=self.read_payload
    elif c[1]==0x11:assert 2<len(c)<=514
    elif c[1]==0x13:pass
    else:raise AssertionError(c)
    self.data.extend(payload);self.status.extend(status);self.ready=True
   return
  super().__setitem__(a,v)
m=Memory();cpu=MPU(memory=m);cpu.sp=253;cpu.p=32
def word(n,v=None):
 a=labels[n]
 if v is not None:m[a:a+2]=[v&255,v>>8]
 return m[a]|m[a+1]<<8
word('c_sp',0xa000)
ticks=100;rng=0
def invoke(name):
 global ticks,rng
 cpu.pc=labels[name];cpu.stPushWord(0x04ff)
 for _ in range(3000000):
  if cpu.pc==0x0500:return cpu.a|cpu.x<<8
  if cpu.pc==labels['_clock']:
   ticks=100+int(cpu.processorCycles*60/985248);word('sreg',ticks>>16);cpu.a=ticks&255;cpu.x=(ticks>>8)&255
   cpu.pc=(cpu.stPopWord()+1)&65535
  elif cpu.pc==labels['_random_byte']:
   rng=(rng+7)&255;cpu.a=rng;cpu.x=0;cpu.pc=(cpu.stPopWord()+1)&65535
  else:cpu.step()
 raise AssertionError(name+' hung')
assert invoke('_network_init')==1
assert invoke('_send_tls')==0
assert invoke('_connection_open')==1
assert [c[1] for c in m.commands[:3]]==[1,2,0x10]
payload=bytes(range(256))*2;m[0x5000:0x5200]=payload
word('_tls_app_ptr',0x5000);word('_tls_app_len',512)
assert invoke('_send_tls')==1 and m.commands[-1][2:]==payload
assert invoke('_receive_tls')==1
p=word('_tls_app_ptr');n=word('_tls_app_len');assert bytes(m[p:p+n]).endswith(b'hello')
m.read_payload=payload;assert invoke('_receive_tls')==1 and word('_tls_app_len')==512
p=word('_tls_app_ptr');assert bytes(m[p:p+512])==payload
invoke('_connection_close')
assert invoke('_send_tls')==0
m.present=False;assert invoke('_network_init')==0;m.present=True
m.fail_open=True;assert invoke('_connection_open')==0 and invoke('_send_tls')==0
invoke('_connection_forget')
result={'compiled_6510_UCI_transport':True,'no_turbo_access':True,
 'module_version_required':True,'fresh_seed_before_open':True,
 'max_512_byte_write_and_read':True,'read_HTTP_bytes':True,'failed_TLS_no_write':True,
 'close_disables_write':True,'no_raw_TCP_fallback':True,'hardware_tested':False}
(r/'build/c64-transport-test.json').write_text(json.dumps(result,indent=2));print(result)
