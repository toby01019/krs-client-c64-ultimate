from pathlib import Path
import os, subprocess, time, json
r=Path(__file__).resolve().parent
results={}
for mode in ['good-pin','bad-pin']:
    start=time.monotonic()
    p=subprocess.run([str(r/'build/host_probe.exe'),mode],input=os.urandom(32),
                     capture_output=True,timeout=35)
    output=p.stdout.decode(errors='replace')
    assert p.returncode==0,(mode,p.returncode,output,p.stderr.decode(errors='replace'))
    assert ('authenticated=1' if mode=='good-pin' else 'authenticated=0') in output
    if mode=='good-pin':assert 'http_status=200' in output and 'key_wipe=ok' in output
    results[mode]={'passed':True,'seconds':round(time.monotonic()-start,3),'output':output.splitlines()}
    print(mode,results[mode],flush=True)
results['hardware_tested']=False
results['same_portable_core_as_firmware']=True
results['host_timing_is_not_Ultimate_II_plus_timing']=True
(r/'build/host-test.json').write_text(json.dumps(results,indent=2))
