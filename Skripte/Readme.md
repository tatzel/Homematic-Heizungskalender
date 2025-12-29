# Skripte

Dieser Ordner enthält alle Skripte für den Heizkalender in der MRi-Variante.

## Dateiheader

Die Datei-Header wurden vereinheitlicht. Wo ich eine Quelle oder anderen Code verwendet habe ist dies überall (so hoffe ich) erwähnt.
Auch die Quelle aus den älteren Heizkalender Versionen sind erwähnt. Auch der ursprüngliche Autor wird entsprechend überall genannt.

## Versionierung

Ich verzichte auf eine Versionsnummer in meinen Skripten. Auch das erscheint mir zu kompliziert. Der Stand jedes Skriptes ist an dem Datum im Kopf des Skriptes erkennbar.
Kompatible Skripte finden sich immer ein einem Release zu einem Stand-Datum (siehe Versions-Label in GitHub). Auch als Release-Label verwende ich ein Datum.

## Extension .c versus .hsc

Die ursprünglichen Skripte wurden mit der Extension .c erzeugt. Hintergrund ist vermutlich, dass ein Editor mit c-Syntaxcolorierung verwendet wurde.
entsprechend sind die meisten Kommentare als !// ausgeführt. Das Ausrufezeichen ist das Kommentarzeichen in den Homematic Skripten.
Damit aber ein Kommentar im Editor auch entsprechend angezeigt wird wurde zusätzlich ein C-Kommentar ergänzt.

Ich nutze NotePad++ mit einer entsprechenden Sprach-Erweiterung. Und zusätzlich den **SDV 5.0**.
https://github.com/HMMike/Script-Developer-CCU
Es gibt keine feste Regel für Erweiterungen der Skripte. In den Foren und in **SDV** wird meistens .hsc verwendet. Ich habe mich aber entschieden auch diese Erweiterung .hsc zu verwenden, da ich eben auch sehr viel andere C-Programme schreibe. Und letzten Endes benutze ich für meine C-Programme Visual-Studio. Es störte einfach, dass der falsche Editor startete, wenn ich eine Datei im Explorer anklickte.

## Ursprünglicher Code 

Der ursprüngliche Code, der mit ausgehändigt wurde findet sich [in diesem Ordner](Backup-Originale/3.2.1)
Dies ist der Code mit dem ich meine ersten Schritte mit der CCU3 machte.

### Alte und neue Dateinamen

Die Namensgebung der alten Skripte war mir zu kompliziert und übersichtlich. Ich habe deshalb die Namensgebung vereinfacht und auch grundsätzlich immer alle Skripte im Paket gesendet. Damit kann ich gewährleisten, dass die entsprechenden Skripte untereinander kompatibel sind.
Die nachfolgende Tabelle zeigt wie ich die Namen der alten Skripte, auf denen meine Version basiert, verändert habe.

Diese Dateien befinden sich alle [in diesem Ordner](Backup-Originale/3.2.1)

| Alter Name | Neuer Name |
| :--------- | :--------- |
| HKP-AT-1.0.2  Außentemperatur-Open-Meteo | HK-Außentemperatur-Open-Meteo |
| HKP-ICS-CD-V-3.2.1 Variablen zu Skript1_iCal_ChurchDesk_RessourceV1.3 | HK-Init-Skript 1_ChurchDesk |
| HKP-CT-V-3.2.1 Variablen zu Skript1_ChurchTools_V1.4 | HK-Init-Skript 1_CurchTools |
| HKP-GK-V-3.2.1 Variablen zu Skript1_GoogleKalender_V1.2 | HK-Init-Skript 1_Google |
| HKP-ICS-A-V-3.2.1 Variablen zu Skript1_iCal_V1.2 | HK-Init-Skript 1_iCal |
| HKP-S2-V-3.1.1 Variablen zu Skript2_V1.3 | HK-Init-Skript 2 |
| HKP-ICS-CD-3.1.1 ICal_ChurchDesk_V1.1.6 | HK-Skript 1_ChurchDesk |
| HKP-CT-3.2.1 churchtools_Ressource_V2_8_8 | HK-Skript 1_ChurchTools |
| HKP-GK-3.2.1 Googlekalender_V3_4_4 | HK-Skript 1_Google |
| HKP-ICS-A-3.1.1  ICal_Skript1_V1.3.2 | HK-Skript 1_iCal |
| HKP-S2-3.3.1  Skript2 Schalten_Heizkalender V2.13.7 | HK-Skript 2 |
