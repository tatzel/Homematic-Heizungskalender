# Tools/Debug+Test

Dieser Ordner enthält optionale Zusatz-Skripte rund um den Heizkalender. Sie sind
für den Betrieb nicht zwingend nötig, erleichtern aber Diagnose, Wartung und
Auswertung.

**Diese Skripte werden im Allgemeinden auf der CCU manuell per „Skript testen" ausgeführt.**

## Logging und Auswertung

| Skript | Zweck |
| :--- | :--- |
| `Tool-Log des Heizkalenders ausgeben` | Gibt das Systemprotokoll des Heizkalenders (HK-Log/HK1-Log/HK2-Log) aus. |
| `Tool-Log der Heizkurvenkontrolle ausgeben` | Gibt die Protokolleinträge der Heizkurvenkontrolle aus (Start/Ziel/Ende der Heizphasen). |
| `Tool-Raumvariablen als Tabelle` | Listet alle `HKG-Raum-*`-Variablen als Markdown-Tabelle auf. |

## Diagnose

| Skript | Zweck |
| :--- | :--- |
| `Tool-Diagnose Raumzuordnung ChurchTools` | Prüft die Zuordnung von ChurchTools-Ressourcen zu Raumvariablen. |
| `Tool-Diagnose Geraetereferenzen` | Prüft die in den Raumvariablen referenzierten Aktoren/Geräte. |

## Wartung und Betrieb

| Skript | Zweck |
| :--- | :--- |
| `Tool-Heizgruppen eTRV Modus setzen` | Sonderfall: setzt `CONTROL_MODE` direkt auf allen `HmIP-eTRV`-Einzelthermostaten, nicht auf der Gruppenadresse. Nur nötig wenn ein Thermostat nach einem Firmware-Update den Gruppenmodus nicht übernommen hat. |
| `Tool-HMIPServer Neustart` | Startet den HMIPServer (Java-Prozess) neu. Nötig nach dem Anlegen, Umbenennen oder Löschen von Diagrammen, da sonst ein interner Fehler (RRD already closed) dazu führt, dass Diagramme keine Daten mehr anzeigen. Kann auch als zeitgesteuertes CCU-Programm (z.B. nächtlich) eingesetzt werden. |

## CloudMatic-Diagramme

Skripte zum Sichern, Dumpen und Wiederherstellen der CloudMatic-Diagrammdaten:
`Tool-CloudMatic Diagramm Daten sichern`, `Tool-CloudMatic Diagramm-Dump`,
`Tool-CloudMatic Diagramm-Load`.

## RRD4J-Diagrammdaten exportieren

Die CCU3 speichert Messwerte (Temperatur, Luftfeuchte etc.) im RRD4J-Format unter
`/media/usb0/measurement/` (bei USB-Speicher) bzw. `/usr/local/sdcard/measurement/`.
Die `.rrd`-Dateien lassen sich mit dem enthaltenen Tool `DumpRRD.java` nach XML
konvertieren, um sie extern auszuwerten.

### Voraussetzungen

- Java Runtime auf dem auswertenden Rechner (nicht auf der CCU)
- Die Bibliothek [`rrd4j-3.10.jar`](https://mvnrepository.com/artifact/org.rrd4j/rrd4j/3.10) (oder neuer)
- Die `.rrd`-Dateien von der CCU kopieren (z.B. per SCP)

### Dateien von der CCU kopieren

```bash
scp -r root@192.168.2.130:/media/usb0/measurement/ logs/measurement/
```

### Nutzung

```bash
# rrd4j-3.10.jar und DumpRRD.java in dasselbe Verzeichnis wie die .rrd-Dateien legen
java -cp ./rrd4j-3.10.jar DumpRRD.java <eingabe.rrd> <ausgabe.xml>
```

Beispiel:

```bash
java -cp ./rrd4j-3.10.jar DumpRRD.java 22.rrd 22.xml
```

### XML-Struktur

Die exportierte XML-Datei enthält:

- **Data Sources (`<ds>`)**: Die Sensoren/Kanäle, z.B. `HEQ0105582:1_5` (Kanal 1, Datenpunkt 5)
- **Round Robin Archives (`<rra>`)**: Mehrere Archive pro Datei mit unterschiedlicher Auflösung:

| Auflösung | Typ | Zeitraum ca. |
|-----------|-----|-------------|
| 2,5 min (step×1) | AVERAGE, MAX, MIN | 3 Tage |
| 30 min (step×12) | AVERAGE, MAX, MIN | 31 Tage |
| 6 h (step×144) | AVERAGE, MAX, MIN | 1 Jahr |
| 24 h (step×576) | AVERAGE, MAX, MIN | 5 Jahre |

Zeitstempel in der XML-Datei sind **UTC**. Für die Anzeige in Lokalzeit (Europe/Berlin)
muss die Zeitzone berücksichtigt werden.

### Welche Datei enthält welche Sensoren?

Die Zuordnung Dateiname → Sensoren ergibt sich aus der Diagramm-Konfiguration auf der
CCU (Einstellungen → Diagramme). Die Dateinummer entspricht der internen Diagramm-ID.

## Sonstige Hilfsmittel

`Sammlung-Hilfs-Skripte` enthält einige hilfreiche Skript Snippets.

Weitere Werkzeuge für seltene oder umgebungsspezifische Aufgaben sowie eine Sammlung von
Test-Skripten:

- `Tool-Test Raumvariable auslesen`
- `Tool-Test Raumvariable setzen`
- `Test-Temperatur Verschiebung berechnen`
- `Tool-Test Thermostatgruppe auslesen`
- `Tool-Test Thermostatgruppe schalten`

> [!WARNING]
> Die Skripte `Tool-Alle Systemvariablen löschen` und `Tool-Alle Systemvariablen und Programme löschen` entfernen unwiderruflich Daten aus der CCU.<br>
> Diese Skripte sollten nur mit Bedacht und nach einem Backup verwenden werden. Sie dienen dazu Testmaschinen zu säubern oder zu bereinigen, ohne die Gerätezuordnungen zu beeinflussen.<br>
> **Diese Programme sind wirklich nur mit äußerster Vorsicht und Expertenwissen zu benutzen.**<br>
> Um eine versehentliche Nutzung zu verhindern wurde der `quit`-Befehl am Anfang der Skripte ergänzt. Dieser muss vor der Ausführung bewusst entfernt werden.
