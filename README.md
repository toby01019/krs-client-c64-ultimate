# K.R.S. Client for Commodore 64 Ultimate

Experimental native client for the K.R.S. web mailbox at
https://quipzz.bplaced.net/krs/. Developed by Toby with Codex/AI assistance.
Independent client project; K.R.S. itself is operated by its own project team.

## Download and status

**C64 Ultimate: guest login, reading, display and TLS reconnection tested on
the author's hardware.** Runs without a PC, browser or external proxy.

Download **KRS-Client-Ultimate-20261122-casefix.zip** from this repository's Releases
or [itch.io](https://toby01019.itch.io/krs-client-c64-ultimate).
This package includes the independently verified server-key correction dated
18 November 2026. It replaces the older October binary. Hardware evidence:
firmware 1.2.1 on 18 November and 1.1.0s2 on 19 November. The 22-November case fix passed all 26 letter pairs in the compiled
6502 renderer. Native K.R.S. startup/TLS and F2 switching also passed on
firmware 1.2.1 that day; REST could not supply a physical pixel comparison.
Source rebuild is byte-identical. Replace your old PRG with this case fix.

**Original C64 + Ultimate-II+: EXPERIMENTAL DEVELOPMENT PROTOTYPE ONLY.**
Not tested on its intended hardware, not ready for mailbox use, requires
custom firmware; no flashable firmware provided. See
[experimental/ultimate-ii-plus](experimental/ultimate-ii-plus/README.md).
It is separate from the tested C64 Ultimate client.

## Quick start (C64 Ultimate)

1. Extract the release ZIP and copy KRS-CLIENT.PRG to your SD card.
2. Configure Internet access; enable Command Interface and REU (e.g. 16 MB).
3. U64 Specific Settings: Turbo Control = C64U Turbo Registers; CPU Speed = 1.
   Save settings. The client enables turbo and restores 1 MHz on normal exit.
4. Run KRS-CLIENT.PRG. First connection took about 20–22 seconds on the test
   hardware; subsequent TLS resumption typically took 1–2 seconds.
5. Press L. Public guest login: **guest**, password **guestguest** (not Gast).
   RETURN confirms each entry. G selects German menus; M opens messages.

REU is currently required. No additional physical REU is needed on the Ultimate.
F1 moves the 40-column window; Shift+F1 toggles the complete 80-column view.
F7 exits. See [English instructions](client/README-EN.txt) or
[deutsche Anleitung](client/LIESMICH.txt) for editor controls and limitations.
The local editor exists, but public posting has not been confirmed by an
end-to-end test. Use the public guest account only, not personal passwords.

## Source and build

Source is under `source/`. Requires Python 3 and cc65 with `bin/` and `lib/`:

```text
python source/build.py --cc65 PATH_TO_CC65
```

Output: `source/build/KRS-CLIENT.PRG`. This existing build script uses Windows
cc65 executable names. Included `client/KRS-CLIENT.PRG` SHA-256:

```text
4aba652c14ac75910af785d2ec15720e1ca71c7b8d7ebb10a518d8478f376f32
```

The current binary uses the Ultimate UCI networking interface and turbo
registers. REU emulation alone does not establish compatibility with VICE.
This download is the native Ultimate version. A separate private VICE
test build exists; it is not included in this release.

## Security limits

TLS 1.2, AES-128-GCM, RSA-4096, mandatory Extended Master Secret and a fixed
server public-key pin. The pin was independently CA/hostname validated when
updated; the C64 does not perform general CA-chain/date validation. A server
key change needs another independently verified update. RSA key exchange
does not provide forward secrecy. SID/CIA entropy has no independent quality
certification. Session keys stay in RAM and are cleared on exit; no session
keys are written to SD. This is experimental software.

## Feedback and attribution

For a bug report include client package date, device/firmware, REU setting
and steps to reproduce. Do not attach passwords, session keys or private
device configuration. No credentials or private logs are distributed here.

Based on [JC-000/c64-https](https://github.com/JC-000/c64-https) and included
crypto components, with a cc65 soft80 font. Original contributor and license
notices are retained. See [LICENSES.md](LICENSES.md). AI assistance was used
for development and documentation; test claims above distinguish actual
hardware checks from prototypes.
