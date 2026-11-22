"""Opt-in SOURCE integration, no flashing or image generation.

Requires a full, submodule-initialized Ultimate firmware checkout and Nios II
toolchain for the subsequent separate build. Fails rather than patching an
unrecognized command-task lifecycle. Run only on an isolated checkout.
"""
from pathlib import Path
import argparse, shutil
p=argparse.ArgumentParser()
p.add_argument('firmware_checkout',type=Path)
a=p.parse_args();fw=a.firmware_checkout.resolve();r=Path(__file__).resolve().parent
mk=fw/'target/u2plus/nios/ultimate/Makefile'
uci=fw/'software/io/command_interface/command_intf.cc'
text=uci.read_text()
if 'c64_reset_pending' not in text or 'command_targets[i]->c64_reset()' not in text:
    raise SystemExit('Unsupported reset lifecycle: require the reviewed serialized-reset implementation.')
anchor='include ../../../common/rules.mk'
original=mk.read_text()
if anchor not in original:raise SystemExit('Unsupported firmware Makefile')
stack='"UCI Server", configMINIMAL_STACK_SIZE'
expanded='"UCI Server", 8192 /* KRSTLS: 32 KiB stack on 32-bit Nios II */'
if stack not in text and expanded not in text:raise SystemExit('Unsupported command-task stack declaration')
target=fw/'software/io/krs_tls'
if target.exists():raise SystemExit('Destination already exists; inspect it before re-integrating.')
target.mkdir()
for name in ['krs_tls.c','krs_tls.h','krs_tls_target.cc','rsa_pin.h','krs_tls.mk']:
    shutil.copyfile(r/name,target/name)
shutil.copytree(r/'bearssl-0.6',target/'bearssl-0.6',ignore=shutil.ignore_patterns('*.original'))
include='include $(PATH_SW)/io/krs_tls/krs_tls.mk'
if include not in original:mk.write_text(original.replace(anchor,include+'\n\n'+anchor))
uci.write_text(text.replace(stack,expanded))
print('Experimental sources integrated. No firmware image built and no device flashed.')
print('Reserve private UCI target 0x0E; build with the original Nios II toolchain.')
