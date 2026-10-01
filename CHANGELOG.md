# Changelog

Alle nennenswerten Änderungen an diesem Projekt werden in dieser Datei
dokumentiert.

Das Format orientiert sich an [Keep a Changelog](https://keepachangelog.com/de/1.1.0/).
Anstelle von SemVer-Versionsnummern verwendet dieses Projekt **Datums-Stände**
(passend zu den `vJJJJ-MM-TT`-Git-Tags) — alle Skripte eines Standes gehören
zusammen und müssen gemeinsam eingespielt werden.

Diese Datei **ergänzt** die Changelog-Blöcke in den Datei-Headern (`!// MRi:` /
`!// TT:`); diese bleiben erhalten und sind die feinste Änderungsebene.

## [Unveröffentlicht]

## [2026-10-01]

### Hinzugefügt

- Systemvariable `HK1-SchaltlisteNachlauf` (Default 30 min): Haltezeit, die ein
  Termin nach seinem Ende in der Schaltliste verbleibt. Sicherheitspuffer, damit
  HK-Skript 2 den Termin noch ausschalten kann — kein Heiz-Nachlauf.
- Systemvariable `HK1-SchaltlisteVorlauf` (Default 720 min ChurchTools / 480 min
  übrige Varianten): wie früh ein Termin vor Beginn in die Schaltliste kommt;
  muss ≥ der längsten Vorheizzeit der Heizkurve `HK2-Kurve` sein.
- Werkzeuge zum Reproduzieren und Diagnostizieren des Duplikat-Bugs
  (`Tests/Test-Duplikatcheck-Bug.hsc`, `Tools/Tool-Diagnose Raumzuordnung
  ChurchTools.hsc`, `Tests/churchtools.http`).

### Geändert

- Vor- und Nachlaufzeit der Schaltliste sind nun über Systemvariablen
  konfigurierbar statt hartcodiert; fehlt die Variable, greift der bisherige
  Hardcode-Wert als Fallback (Verhalten unverändert).
- Vergangenheits-Check in `HK-Skript 1_ChurchTools` von `<=` auf `<`
  vereinheitlicht (1-Sekunden-Divergenz am Nachlaufende entfernt — jetzt alle
  Skript-1-Varianten gleich).
- Log-Format der Raumnamen bereinigt: Normalfall `Name(ID)` statt
  `Name (ID)-Name`, Multiraum-Zusatz nur bei Bedarf; betrifft `HK-Skript 2`,
  `HK-Heizkurvenkontrolle` und alle Skript-1-Varianten.
- Dokumentation (`Skripte/Dokumentation/Readme.md`) um beide neuen Variablen und
  eine ausführlichere `HK2-Kurve`-Erklärung erweitert.

### Behoben

- False-Positive im Duplikat-Check: Eine einstellige Ressource-ID (z.B. `1`)
  wurde fälschlich als Teil einer mehrstelligen ID (z.B. `11`) erkannt und der
  Termin übersprungen — kein Heizen, kein Log. Fix: Semikolon-Präfix verankert
  die Suche an Eintraggrenzen. Betrifft alle fünf Skript-1-Varianten.

## [2026-09-22]

### Hinzugefügt

- Konsistenzprüfung in `HK-Skript 1_ChurchTools`: warnt im Log, wenn benötigte
  Systemvariablen fehlen (nur Warnung, kein Abbruch).
- Test-Tools für Raumvariablen (`Tools/`): auslesen, einzelnes Feld setzen, alle
  `HKG-Raum-*` als Markdown-Tabelle ausgeben.
- `DUP:`- und `DIVERGENZ:`-Marker an bekannten Code-Dubletten bzw. Unterschieden
  der Skript-1-Varianten (reine Dokumentation).

### Geändert

- Logging in `HK-Skript 2`: Meldungen im Ausschalt-Block erhalten den
  Raumname-Präfix, konsistent zu den übrigen Schaltmeldungen.

### Behoben

- Header-Versionskorrektur `HK-Skript 2`: `HKP-S2-3.2.1` → `3.3.1`.
- Tippfehler- und Label-Korrekturen im `HK-Test-Skript` (u.a. `Chruchtools` →
  `Churchtools`).

## [2026-09-21]

### Hinzugefügt

- Räume ohne Temperatursensor werden in der `HK-Heizkurvenkontrolle` nicht mehr
  stumm übersprungen: Log-Eintrag im Systemprotokoll, auf einmal pro Heizphase
  begrenzt.
- Neues Tool „Log des Heizkalenders ausgeben": durchsucht die USB-Logdateien
  nach konfigurierbarem Suchbegriff und gibt die letzten Einträge aus.

### Geändert

- Logging in `HK-Skript 2` um Raumnamen erweitert; AT/GT/IST-Zeile mit
  Bezeichnung, °C und einheitlichen Nachkommastellen.
- Robustheit in `HK-Außentemperatur-Open-Meteo`: `pos<0`-Prüfungen nach den
  `Find()`-Aufrufen, redundante Konversionen entfernt.

### Behoben

- Nachtschaltung in `HK-Skript 2`: `objDP.State(0)` schrieb versehentlich den
  Wert AUS und lieferte zugleich `true` zurück. Fix: `State(0)` → `State()` (nur
  lesen). CCU-verifiziert.
- Drei RRULE-Bugs in `HK-Skript 1_iCal`: Tippfehler `DAYLY` → `DAILY`;
  `iMaxCount=0`-Default brach ohne `COUNT=` sofort ab (Fix: Sentinel 9999);
  `UNTIL` wurde nicht gelesen (Datum nun korrekt aus RRULE geparst).
- `HK-Heizkurvenkontrolle`: `HSFlag` von `boolean` auf `string` (korrekte
  `H`/`S`/`HS`-Vergleiche); `strTemp`-Shadowing im DEBUG-Block behoben.
- Init-Skripte: `iPos>=0`-Guard vor `Substr` (ChurchTools); `color`-Guard
  `iPos>=0` statt `iPos&&` (ChurchDesk — verfehlte sonst Position 0 und -1).
- Nullpointer-Zugriff im Installer (`ScriptEngine`): Verbindungskontext wird vor
  dem Nullsetzen gesichert.
- Fehlerprüfung beim Parsen der Raumnamen (`HK-Init-Skript 1_ChurchDesk`/
  `_ChurchTools`): geprüft wurde `iPos` statt `iPosEnd`, wodurch die
  Fehlerbehandlung nie griff.
- Kaputte Umlaute (`U+FFFD`) in `ScriptEngine.cpp`/`.h` repariert; Debug-Ausgabe
  in `HK-Heizkurvenkontrolle` deaktiviert (`DEBUG=1` → `0`).
- Rechtschreib- und Grammatikkorrekturen in Skript-Kommentaren und `Readme.md`.

## [Ältere Stände — 2025-01 bis 2026-08]

Zusammenfassung der früheren Entwicklung; Details in den Datei-Header-Changelogs
und den Git-Tags (`git tag -l`, von `v2025-01-16` bis `v2026-08-10`):

- Erststruktur des Heizkalenders mit getrennten Skript-1-Varianten (ChurchTools,
  ChurchDesk API/iCal, Google, iCal) und zentralem Schaltskript `HK-Skript 2`.
- Multi-Raum-Variante: einer Ressource lassen sich mehrere Räume zuordnen.
- „Heizen mit Schalten": Schaltliste um den Heiz-/Schalt-Parameter erweitert.
- Sonderbefehle in Termintexten (`#EIN#`, `#AUS#`, `#GT#`, `#NS#`, `#NH#`,
  `#RESET#`, `#<Zahl><Text>#`).
- RRULE-Unterstützung (Terminwiederholungen) in der iCal-Variante.
- Logging über Systemvariablen (`HK1-Log`/`HK1-Logging`, `HK2-Log`/`HK2-Logging`)
  und die Heizkurvenkontrolle im Systemprotokoll.
- Automatisches Anlegen zusätzlicher Räume und Übernahme der Ressourcen-
  Klartextnamen in die `HK1-R-Liste`.
