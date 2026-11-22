K.R.S. Ultimate-II+ TLS-Offload - ENTWICKLUNGSPROTOTYP
Stand: 14.10.2026

STATUS
Der TLS-Kern und die C64-Schnittstelle sind implementiert und separat
getestet. KEINE fertige oder auf Ultimate-II+ bestaetigte Firmware.
Keine .u2p-, .app- oder .elf-Datei zum Flashen enthalten. Die PRG benoetigt
die passende angepasste Firmware. Auf Serienfirmware meldet sie
"TLS-FIRMWARE FEHLT". Nicht als funktionierende Endnutzer-Version verteilen.
Dein bisheriger Commodore-64-Ultimate-Client wurde nicht veraendert.

AUFTEILUNG
C64: Bildschirm, 40/80-Spalten-Ansicht, Tastatur, HTTP/JSON, Menues und Editor.
Ultimate-II+: eigener TCP-Socket und kompletter TLS-Verbindungsaufbau,
Serverschluesselpruefung, Verschluesselung und Authentifizierung der Daten.
Die UCI-Verbindung laeuft lokal ueber den Modulsteckplatz. Es ist kein
PC-Proxy, Cloud-Dienst oder dauerhaft laufender Rechner vorgesehen.
Ein PC wird zum Bauen/Einrichten und fuer die bisherigen Tests verwendet.
PC-freier Betrieb auf dem Zielgeraet wurde NOCH NICHT nachgewiesen.

WAS ERFOLGREICH GETESTET WURDE
1. Der echte portable TLS-Kern konnte auf dem Windows-Testrechner mit dem
   bekannten K.R.S.-Serverschluessel HTTPS aufbauen und HTTP 200 abrufen.
   Mit absichtlich falschem Pin wurde die Authentifizierung verweigert.
   Keine Login-Daten und keine Beitraege gesendet. Testzeiten sind KEINE
   Messung der Ultimate-II+-Geschwindigkeit.
2. Der echte C++-Firmware-Adapter wurde gegen die originale CommandTarget-
   Header-Schnittstelle kompiliert. Netzwerk und TLS waren in DIESEM
   Adaptertest simuliert: Befehlslaengen, frischer Seed, Schliessen nach
   TLS-Fehler und C64-Reset sowie Verbindungsbesitz wurden geprueft.
3. Die kompilierte C64-PRG wurde im 6502-Emulator gegen ein UCI-Modell
   ausgefuehrt: Firmware-Erkennung, Seed/Open, 512-Byte-Senden/Empfangen,
   Fehlerverhalten, Schliessen und fehlende Turbozugriffe bestanden.
4. Die Quelltext-Integration wurde auf den heruntergeladenen originalen
   Makefile-/CommandInterface-Dateien geprueft. KEIN Nios-II-Firmware-Build.
Details stehen in validation/. Die beiden Teile wurden noch NICHT zusammen
auf echter Hardware oder in einer kompletten Firmware ausgefuehrt.

FIRMWARE-INTEGRATION NUR FUER ENTWICKLER
Vollstaendigen offiziellen Firmware-Checkout einschliesslich Submodulen
und originale Nios-II-/BSP-Bauumgebung bereitstellen. Separaten Checkout
verwenden. In firmware/ liegt integrate_firmware.py CHECKOUT_PFAD.
Das Skript kopiert die Quellen nach software/io/krs_tls, ergaenzt die
Ultimate-II+-Nios-Makefile und vergroessert den UCI-Taskstack auf 8192
Stack-Worte (32 KiB bei 32-bit Nios II). Es baut und flasht nichts.
Es verlangt die gepruefte serialisierte C64-Reset-Behandlung und bricht
bei unbekannten Ausgangsdateien oder bestehendem Zielverzeichnis ab.
Der private UCI-Target-Wert 0x0E ist NICHT offiziell reserviert. Vor dem
Build auf Kollisionen mit anderen Erweiterungen pruefen.
Danach originalen Firmware-Build ausfuehren. Imagegroesse, Heap, Stack-
Highwater-Mark, Laufzeiten, Socket-Zeitlimits, Reset/Abbruch und Scheduler
auf echtem Ultimate-II+ pruefen. Erst nach erfolgreicher Pruefung waere
eine geraetespezifische Test-Firmware vertretbar. Ein Quellenpaket auf
USB zu kopieren installiert KEIN Firmware-Modul.
Der passende Crosscompiler und echte Ultimate-II+-Hardware fehlen hier;
deshalb kann dieses Paket keine installierbare Firmware garantieren.

C64-SEITE
experimental/KRS-U2-TLS.PRG ist die passende ungetestete Hardware-Testfassung.
Voraussetzung: C64 + Ultimate-II+, Ethernet/Internet, Command Interface,
REU (z.B. 16 MB) und das private TLS-Modul in angepasster Firmware.
source/build.py gibt es als c64-source/build.py --cc65 PFAD_ZU_CC65.
Das Ergebnis im build/-Ordner heisst KRS-CLIENT.PRG.
Der Client greift nicht auf das C64-Ultimate-Turboregister zu. Die bisher
verwendete SID/CIA-Entropie und HMAC-DRBG erzeugen vor jeder Verbindung
32 neue Startbytes fuer die Firmware; diese Quelle ist nicht unabhaengig
zertifiziert. Zufallsqualitaet auf echten unterschiedlichen SID-Modellen
muss vor einer Freigabe fuer persoenliche Konten untersucht werden.
Noch unbenutzte alte Krypto-Routinen befinden sich aus Build-Kompatibilitaet
im Binary, werden fuer TLS jedoch nicht aufgerufen.

TLS-PROFIL UND GRENZEN
BearSSL 0.6, TLS 1.2, ausschliesslich ECDHE_RSA_AES128_GCM_SHA256.
Ephemere ECDHE-Schluessel bieten Forward Secrecy. Der bekannte RSA-4096-
Serverschluessel prueft die signierte ECDHE-Vereinbarung. Server-Finished
muss vor Erfolg von OPEN authentifiziert sein; erst danach gehen HTTP-
oder Login-Daten auf die Leitung. Kein Klartext-/Direkt-TCP-Fallback.
Pin-Bindung an quipzz.bplaced.net:443, kein allgemeiner CA-/Datumsvalidator.
Die Firmware-Pindatei ist BIG ENDIAN; die bisherige 6502-Montgomery-Datei
ist LITTLE ENDIAN und darf NICHT unveraendert uebernommen werden.
Bei Betreiber-Schluesselwechsel ist eine unabhaengig validierte Pin-
Aktualisierung erforderlich. Nur den oeffentlichen Gastzugang verwenden:
L fuer Login, Name guest, Passwort guestguest; NICHT Gast eintippen.
Keine persoenlichen Passwoerter in diesem Entwicklungsprototyp verwenden.
BearSSL 0.6 implementiert hier keine erzwungene EMS-Aushandlung. Deshalb
sind Wiederaufnahme und Neuverhandlung deaktiviert; jede Verbindung startet
mit geloeschtem TLS-Kontext, frischem C64-Seed und neuer ECDHE-Vereinbarung.
Socket-Timeouts pro Empfang/Senden 8 Sekunden, C64-UCI-Wartebudget 60
Sekunden. Das ist kein bewiesenes Ende-zu-Ende-Handshake-Zeitlimit.
Reset/Abbruch/Fehler schliessen den eigenen Socket und loeschen den Kontext.
Ein im Handshake blockierter Aufruf wird erst nach Rueckkehr abgewickelt;
kein asynchrones Freigeben eines gerade verwendeten TLS-Kontexts.

PRIVATES UCI-PROTOKOLL KRSTLS/1
Target 0x0E, hoechstens 514 Befehlsbytes, Antworten final und <=512 Bytes.
01 IDENTIFY -> acht Bytes KRSTLS/1
02 SEED + exakt 32 Bytes -> schliesst Altverbindung, einmaliger Startwert
10 OPEN -> fest K.R.S., verbraucht Seed auch im Fehlerfall
11 WRITE + 1..512 Klartextbytes -> nur nach erfolgreicher TLS-Authentifizierung
12 READ -> 1..512 entschluesselte/authentifizierte Bytes oder Fehler
13 CLOSE -> schliessen und loeschen; beliebig oft erlaubt
Erfolg im Statuskanal: exakt sechs ASCII-Bytes 000 OK.
Jede UCI-Antwort wird komplett aus Daten UND Status gelesen und einmal
bestaetigt. Keine automatische Wiederholung von Beitrags-Schreibaktionen.

WEITERE SCHRITTE ZUR NUTZBAREN VERSION
1. Vollstaendiger Nios-II-Firmware-Build und Beseitigen etwaiger Portfehler.
2. Hardwaretest PAL/NTSC C64 + Ultimate-II+, Geschwindigkeit/Entropie/RAM.
3. Echter Gast-Login, Lesen, lange Antworten, Netzunterbrechung, Reset,
   Editor und Abbruch pruefen. Keine oeffentlichen Testbeitraege senden.
4. Erst dann Freigabe und nachvollziehbare Firmware-/Client-Versionsbindung.

QUELLEN
https://github.com/GideonZ/1541ultimate
https://1541u-documentation.readthedocs.io/en/latest/uci/core_uci_architecture.html
https://bearssl.org/
https://quipzz.bplaced.net/krs/
