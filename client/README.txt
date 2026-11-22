K.R.S.-Client fuer Commodore 64 Ultimate - experimentell
Programmstand: 18.11.2026; Downloadpaket: 22.11.2026

START
SD / The very Second / BBS / KRS Client / KRS-CLIENT.PRG auswaehlen und
"Run" / "Start" benutzen. Die erste Verbindung braucht jetzt etwa 20-22 Sekunden.
Der Windows-PC wird nur zum Einrichten und Testen verwendet. Zur Nutzung
ist kein PC, Browser, WinSCP oder Proxy erforderlich.

VORAUSSETZUNGEN
Ultimate mit Internetverbindung, aktiviertem Command Interface und REU.
U64 Specific Settings: Turbo Control = C64U Turbo Registers, CPU Speed = 1.
Der Client schaltet selbst auf Turbo und bei F7 wieder auf 1 MHz zurueck.
Diese Einstellungen vor dem Start am eigenen Ultimate pruefen.

ANMELDUNG UND LESEN
Du brauchst fuer den Gastzugang kein eigenes Konto. L waehlt Login.
Oeffentlicher Betreiber-Gastzugang (auch direkt im Client eingeblendet):
Handle: guest
Passwort: guestguest
Den englischen Namen guest eintippen, nicht das deutsche Wort Gast.
Enter bestaetigt. Bei -- More -- Enter druecken. G waehlt deutsche Menues.
M = Nachrichtenmenue, W = Board wechseln, Nummer eingeben und Enter.
L = Beitragsliste. Cursortasten waehlen, Enter oeffnet den Beitrag.
Die alte K.R.S.-Darstellung hat 80 Spalten. Die normale Ansicht zeigt
standardmaessig den mittleren Ausschnitt. F1 wechselt zwischen Mitte,
rechts und links. F2 (Shift+F1) schaltet eine komplette 80-Spalten-Ansicht
mit kleinerer 4x8-Pixel-Schrift ein und aus. Eine andere HDMI-Aufloesung
ist dafuer nicht notwendig. F3/F5 senden PGUP/PGDN.
F7 beendet den Client. ESC oder RUN/STOP bricht eine Eingabe ab.

BEITRAEGE UND ANTWORTEN
M oeffnet das Nachrichtenmenue. S startet einen neuen Beitrag.
Zum Antworten erst einen Beitrag ganz lesen, dann R fuer Reply waehlen.
Titel/Empfaenger nach den Fragen der Mailbox eingeben. Sobald K.R.S. den
Editor aufruft, erscheint der lokale Beitragseditor.
Cursor: im Text bewegen. DEL: Zeichen zurueck loeschen. HOME: Zeilenanfang.
RETURN: neue Zeile. An Zeilenanfang verbindet DEL mit der vorherigen
Zeile, wenn zusammen hoechstens 79 Zeichen entstehen.
F3/F5: im Text blaettern. Shift+F1: 40/80 Spalten umschalten.
F8 (Shift+F7): Absenden vorbereiten. Erst J auf die Rueckfrage sendet.
N geht zurueck zum Text. ESC/RUN STOP oder F7: Abbruch mit Rueckfrage.
Maximal 64 Zeilen zu 79 Zeichen; kleinere Vorgaben der Mailbox gelten.
Bei 79 Zeichen folgt eine neue Zeile. Umlaute bitte als ae/oe/ue/ss tippen.
Die Zitatauswahl des Webeditors ist noch nicht enthalten. Zu lange oder
nicht darstellbare Textvorgaben werden gesperrt statt unbemerkt gekuerzt.
Schreibrechte vergibt die Mailbox fuer das jeweilige Konto und Board.
Bei unklarer Sendeantwort erfolgt KEINE automatische Wiederholung.
Dann mit F7 beenden und nach erneuter Anmeldung nachsehen, ob der Beitrag
angekommen ist. Ein Entwurf ueberlebt derzeit keinen Programmneustart.

WIEDERVERBINDUNG
Der Webserver schliesst HTTPS-Verbindungen regelmaessig. Der Client baut
sie selbst neu auf und behaelt Mailbox-Sitzung und Bildschirminhalt.
Die schnelle TLS-Wiederaufnahme braucht auf diesem Ultimate normalerweise
etwa 1 bis 2 Sekunden (gemessen 1,2 bis 1,6 Sekunden statt zuvor ca. 23).
Unten erscheint kurz "VERBINDE NEU... BITTE WARTEN", auch bei 80 Spalten.
Falls der Server die Sitzung nicht mehr kennt oder nach hoechstens
30 Minuten, erfolgt wieder ein vollstaendiger Aufbau von etwa 16 Sekunden.
Der erste Start braucht etwa 20-22 Sekunden statt zuvor rund 30. Nach einem Neustart
gibt es keinen gespeicherten TLS-Sitzungsschluessel.

TECHNIK UND GRENZEN
Native TLS 1.2, AES-128-GCM, RSA-4096 mit e=65537, zwingendes Extended
Master Secret und 512-Byte Max Fragment Length. Die RSA-Vorberechnung
findet vor TCP-Connect statt, um die kurze Server-Wartefrist einzuhalten.
Die optimierte 6502-Multiplikationsroutine braucht rund 37 Prozent weniger
CPU-Takte als die vorherige Fassung, bei identischen Rechenergebnissen.
Beim Start erfolgt die Hardware-Zufallsabtastung weiterhin bei Normaltempo;
die anschliessende deterministische SHA/HMAC-Arbeit nutzt bereits Turbo.
Der bekannte RSA-Schluessel wird exakt geprueft; das Server-Finished muss
korrekt sein, bevor Anwendungsdaten gesendet oder angenommen werden.
TLS-1.2-Session-IDs erlauben anschliessend die schnelle Wiederaufnahme.
Nur erfolgreich authentifizierte EMS-Sitzungen werden bis zu 30 Minuten
im RAM behalten. Neue Zufallswerte erzeugen bei jeder Wiederaufnahme neue
Verbindungsschluessel; Finished und AES-GCM werden unveraendert geprueft.
Abgelehnte Sitzungen werden verworfen und vor einem neuen TCP-Aufbau
frisch per RSA vorbereitet. Beim Beenden wird der Sitzungsschluessel
geloescht. Keine TLS-Sitzungsschluessel werden auf die SD-Karte geschrieben.
Der Pin stammt aus einer unabhaengig per CA validierten Verbindung.
Keine allgemeine CA-Ketten-/Datum-Pruefung im C64. Bei Schluesselwechsel
des Betreibers muss der Client mit unabhaengig geprueftem Pin neu gebaut
werden. Das aktuelle Zertifikat laeuft am 06.02.2027 ab; ein unveraenderter
Schluessel bleibt akzeptiert, auch bei Zertifikatserneuerung.
Dieser RSA-Schluesselaustausch bietet KEINE Forward Secrecy. Nur den
oeffentlichen Gastzugang verwenden; keine persoenlichen Passwoerter.
Die Entropiegewinnung verwendet die vorhandene SID/CIA-DRBG-Bibliothek;
es gibt keine unabhaengige Entropie-Zertifizierung der FPGA-Hardware.
ASCII/PETSCII-Anzeige, einfache Umlautumschrift, inverse Hervorhebung;
keine vollstaendige farbige ANSI-Terminalemulation.

LIZENZEN UND QUELLEN
Enthaelt Teile von JC-000/c64-https (MIT), dessen Lizenz liegt bei.
Der 4x8-Zeichensatz stammt aus cc65 soft80 (Groepaz/Hitmen).
Eigene Anpassungen: RSA, TLS-1.2-Profil, AES-GCM, HTTP/JSON-Parser und UI.
Die Quelltexte und ein cc65-Buildskript liegen im ZIP-Paket.
https://github.com/JC-000/c64-https
https://github.com/cc65/cc65
https://quipzz.bplaced.net/krs/

18.11.2026: Neuer RSA-Serverschluessel unabhaengig CA-/Hostnamen-validiert.
Gastlogin, Nachrichtenbereiche und TLS-Wiederaufnahme auf Firmware 1.2.1 getestet.

PAKETAKTUALISIERUNG 22.11.2026
Neuer, am 18.11.2026 unabhaengig gepruefter K.R.S.-Serverschluessel.
Ersetzt die Programmfassung im Download vom 19.10.2026.
Gastlogin und Wiederaufnahme am 18.11. auf Firmware 1.2.1 getestet;
weiterer Hardwaretest am 19.11. auf Firmware 1.1.0s2 erfolgreich.
Kein neuer Hardwaretest am Tag dieser Paketveroeffentlichung.
Schreibfunktion vorhanden, kein oeffentlicher Ende-zu-Ende-Test bestaetigt.
