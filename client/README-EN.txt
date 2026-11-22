K.R.S. Client for Commodore 64 Ultimate
Client update: 18 November 2026
Download package: 22 November 2026
Documentation: English / German (LIESMICH.txt)

SUPPORTED HARDWARE
For the new Commodore 64 Ultimate with turbo, networking, Command Interface
and REU. Guest login, reading, display and reconnection were tested on a
Commodore 64 Ultimate. This PRG is NOT for an original C64 with Ultimate-II+.
No running PC, browser, WinSCP or proxy is required.

SETUP
1. Extract the ZIP and copy KRS-CLIENT.PRG to any folder on your SD card.
2. Set up Internet access, enable Command Interface and REU (e.g. 16 MB).
3. Under U64 Specific Settings set Turbo Control = C64U Turbo Registers
   and CPU Speed = 1. Save settings. The client enables turbo at startup
   and disables it again on normal exit.
4. Select and run KRS-CLIENT.PRG in the file browser. The initial connection
   takes about 20-22 seconds on the test device.

GUEST LOGIN
Press L to log in. Enter username guest (not Gast), password guestguest.
Confirm each entry with RETURN. This is the operator's public guest account;
no personal account is required. Choose G for German menus; M opens messages.
Guest posting permissions depend on mailbox settings. For now, use only the
public guest account with this experimental release. Do not enter personal
passwords.

CONTROLS
RETURN confirms input and continues at -- More --.
Cursor keys move the selection. F3/F5 send page up/down.
F1 changes the visible 40-column section: centre, right, left.
F2 (Shift+F1) toggles the full 80-column view with a smaller font.
There is no need to change the HDMI resolution.
F7 exits. ESC/RUN STOP cancels input.
A wait message appears during reconnection. Fast resumption takes about
1-2 seconds on the test device; rebuilding a session may take longer if the
server has discarded the TLS session.

POSTS AND REPLIES
In the message menu press S for a new post, or R to reply after reading a
post. Enter the title/recipient when prompted by the mailbox.
The simple local editor supports up to 64 lines of 79 characters.
Cursor keys move; DEL deletes; RETURN inserts a new line.
F8 (Shift+F7) prepares sending; J confirms transmission. N returns to editing.
ESC/RUN STOP or F7 cancels after a confirmation prompt.
An unclear send result does NOT trigger automatic retransmission.
The editor and cancellation were tested; no public test post was published.
Actual sending and posting permissions are therefore not confirmed for
every account/board. Drafts are lost on program restart.
Write German umlauts as ae/oe/ue/ss.

TECHNICAL LIMITATIONS
This client is specific to K.R.S.; it is not a browser or general Telnet
terminal. Internet destination: quipzz.bplaced.net.
TLS 1.2, AES128-GCM, RSA4096, Extended Master Secret, fixed server key pin.
During development the pin was independently checked using a CA-validated
connection. The C64 itself does not perform general CA/date validation.
A server key change requires a verified client update.
This RSA key exchange does not provide forward secrecy. The SID/CIA random
source has not been independently certified for entropy quality.
TLS resumption keys remain in RAM only, for at most 30 minutes, and are
erased on exit. No TLS keys are stored on the SD card.
ASCII/PETSCII display; not a complete ANSI or browser renderer.

SOURCE AND LICENCES
Source code and the cc65 build script are in source/.
source/build.py --cc65 PATH_TO_CC65_DIRECTORY
Requires Python 3 and cc65 with bin/ and lib/.
Includes parts of JC-000/c64-https (MIT) and the cc65 soft80 font.
The respective licence notices are included. Private development logs,
device addresses and recorded session data are not included.

MAILBOX AND PROJECT SOURCES
https://quipzz.bplaced.net/krs/
https://github.com/JC-000/c64-https
https://github.com/cc65/cc65

UPDATE 22 NOVEMBER 2026
Includes the independently verified K.R.S. server key update from
18 November 2026. Replaces the binary distributed on 19 October 2026.
Guest login and TLS resumption were tested on firmware 1.2.1 on
18 November; a further hardware test passed on firmware 1.1.0s2 on
19 November. No new hardware test was performed on the packaging date.
The source rebuild matches the included PRG byte for byte.
The separate Ultimate-II+ prototype is unchanged and remains untested
on its intended hardware; it is not a ready-to-use release.
