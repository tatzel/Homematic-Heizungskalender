# Tools

Dieser Ordner enthält optionale Zusatz-Skripte rund um den Heizkalender. Sie sind
für den Betrieb nicht zwingend nötig, erleichtern aber Diagnose, Wartung und
Auswertung. Die Skripte werden auf der CCU manuell per „Skript testen" ausgeführt.

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
| `Tool-Heizgruppen Modus zurücksetzen` | Setzt den Modus aller Heizgruppen und ihrer Einzelthermostate (HmIP-eTRV) auf Manuell oder Auto. Eignet sich für den nächtlichen Betrieb als Absicherung gegen Modusverlust nach CCU-Neustarts. |
| `Tool-Gestörte Kommunikation beheben` | Behebt Kommunikationsstörungen (UNREACH) und überträgt ausstehende Konfigurationsdaten (CONFIG_PENDING) an Geräte. Eignet sich als nächtliches Wartungsprogramm. |
| `Tool-Servicemeldungen automatisch bestätigen` | Bestätigt Servicemeldungen der CCU automatisch. |
| `Tool-Uptime loggen` | Protokolliert die Laufzeit der CCU. |
| `Tool-Reboot` | Startet die CCU neu. |

## CloudMatic-Diagramme

Skripte zum Sichern, Dumpen und Wiederherstellen der CloudMatic-Diagrammdaten:
`Tool-CloudMatic Diagramm Daten sichern`, `Tool-CloudMatic Diagramm-Dump`,
`Tool-CloudMatic Diagramm-Load`.

## Sonstige Hilfsmittel

Weitere Werkzeuge für seltene oder umgebungsspezifische Aufgaben:
`Tool-WakeOnLAN BeamerPC`, `Sammlung-Hilfs-Skripte` sowie eine Sammlung von
Test-Skripten (`Tool-Test Raumvariable auslesen`,
`Tool-Test Raumvariable setzen`, `Test-Temperatur Verschiebung berechnen`,
`Tool-Test Thermostatgruppe auslesen`, `Tool-Test Thermostatgruppe schalten`).

> [!WARNING]
> Die Skripte `Tool-Alle Systemvariablen löschen` und
> `Tool-Alle Systemvariablen und Programme löschen` entfernen unwiderruflich
> Daten auf der CCU. Nur mit Bedacht und nach einem Backup verwenden.

## Entwickler-Tests

Reine Entwickler-Testfälle liegen im Ordner [`Tests/`](../Tests) (z.B.
`Test-Duplikatcheck-Bug.hsc`, `churchtools.http`). Sie sind nicht für den
Produktivbetrieb gedacht.
