
# Beschreibung der Skripte und Programme

Dieser Ordner enthält alle Hauptkomponenten für den Heizkalender. Ein Teil der Skripte dient nur der Initaialisierung. Diese haben das kürzel _Init_ im Namen.

Skripte mit dem Namenskürzel Skript1 beschreiben die verschiedenen Methoden Termine für den Heizkalender auszulesen.

Das Skript 2 ist das Hauptskript zum Schalten der Heizung.


## HeizkalenderInstallation.exe

Die HeizkalenderInstallation.exe dient zur Installation und Einrichtung der verschiedenen Skriptvarianten.
Dieses Programm kann bestehende Installationen aktualisieren oder kann einfach neue Installationen erzeugen.

Das Programm legt dazu die notwendigen Skripte und Systemvariablen an. Die notwendigen passenden Skripte müssen dazu im Programmverzeichnis liegen.

# Skripte zur Initialisierung

## HK-Init-Skript 1_ChurchDesk.hsc

Dieses Skript legt alle Systemvariablen an, die für den Zugriff auf ChruchDesk benötigt werden. Es spielt hier keine Rolle ob der Zugriff über die ChurchDesk API oder die ChurchDesk iCal Schnittstelle erfolgt.

## HK-Init-Skript 1_ChurchTools.hsc

Dieses Skript dient zum Anlegenden aller Systemvariablen um Termine aus den Ressourcen der ChruchTools API zu lesen.

## HK-Init-Skript 1_Google.hsc

Dieses Skript legt alle Systemvariablen an, die für den Zugriff auf die Google Calendar API verwendet werden.

## HK-Init-Skript 1_iCal.hsc

Dieses Skript legt alle Systemvariablen an, die für den Zugriff auf einen beliebiegen iCal-Kalender benötigt werden.

## HK-Init-Skript 2.hsc

Dieses Skript legt alle Systemvariablen an, die für das Schalt-Skript benötigt werden.

## HK-Init-Variablen Logging.hsc
.wer Kategorien hier.

- Allgemeindes Log. Einträge tragen den Namen *HK-Log*.
- Logging für die Skripte vom Typ 1. Hier werden die Zugriffe auf die Kalender protokolliert. Einträge tragen den Namen *HK1-Log*.
- Logging für das Skript vom Typ 2. Hier werden die Schaltvorgänge der Heizung protokolliert und acuh alle Sonderfunktionen, wie das Rückstellen der Heizung auf Grundtemperatur oder das Abschalten der Heizung, wenn der betroffene Termin gelöscht wird.  Einträge tragen den Namen *HK2-Log*.
- Logging für die Heizkurvenkontrolle. Um die Heizkurve kontrollieren zu können, werden spezielle Einträge im Systemprotokoll verzeichnet: Start des Heizens, Erreichen der Zieltemperatur, Start des Termins, Ende der Heizphase, Ende des Termines. Protokolliert werden Außentemperatur (beim Start), aktuelle Temperatur, bisher erreichte maximale Temperatur.  Einträge tragen den Namen *HK-LogHeizkurvenkontrolle*.

# Skripte zum Einlesen der Termindaten aus diversen Quellen

Es wird für den Betrieb des Heizkalenders ein der nachfolgenden Datenquellen (ChurchTools, ChurchDesk, iCal, Google-Kalender) benötigt. Das entsprechende Skript sollte ca. alle 30min laufen um die Termine für die nächsten Heizzyklen zu bestimmen.

## HK-Skript 1_ChurchDeskAPI.hsc

Skript für den Zugriff auf die internen Kalender in ChurchDesk über die ChurchDesk API.

> [!WARNING]
> Aktuell können nur öffentliche Termine über die ChurchDesk API gelesen werden. Ist ein Termin als privat oder nur für bestimmte Gruppen sichtbar, wird dieser Termin nicht für den Heizkalender berücksichttigt. Aktuell kann nur die ChurchDesk iCal Variante alle Termine lesen.

## HK-Skript 1_ChurchDeskiCal.hsc

Skript für den Zugriff auf die internen Kalender in ChurchDesk über die ChurchDesk iCal Kalender der Ressourcen.

## HK-Skript 1_ChurchTools.hsc

Skript für den Zugriff auf die internen Kalender in ChurchTools über die ChurchTools-API auf die Kalender der Ressourcen.

## HK-Skript 1_Google.hsc

Skript für den Zugriff auf einen Kalender in Google über die Google Calendar-API. Bei diesem Verfahren muss der Raumname mit einem vorangestellten #-Zeichen im Titel des Termines stehen. Es wird ein API-Key für die Google-Calendar-API benötigt. Der Google-Kalender selbst muss auch öffentlich sein.

## HK-Skript 1_iCal.hsc

Skript für den Zugriff auf einen iCal-Kalender über eine URL. Bei diesem Verfahren muss der Raumname mit einem vorangestellten #-Zeichen im Titel des Termines stehen. 

# Schaltskript

Das Schaltskript ist das Herzstück der Heizkalender Software. Es muss zwingend alle min laufen. Das Skript schaltet die Heizthermosta ein und auch wieder aus.

## HK-Skript 2.hsc

Das Skript 2 ist das Hauptskript um die Thermostate zu schalten.

# Sonstige Skripte 

Alle weiteren hier aufgeührten Skripte sind für den Betrieb des Heizkalenders dienlich.

##  HK-Außentemperatur-Open-Meteo.hsc

Wenn kein Außenthermostat zur Verfügung steht kann mit diesem Skript aus den Geodaten der CCU über den Deutschen-Wetter-Dienst, die aktuelle Außentemperatur verwendet werden. Dieses Skript benutzt nicht die aktuelle Außentemperatur, sondern führt eine Berechnung des gleitenden Mittelwertes über 36h durch.

Dieses Skript sollte jede Stunde einmal laufen.

## HK-Heizkurvenkontrolle.hsc

Dieses Skript erzeugt zusätzliche Einträge im Systemprotokoll mit dem die aktuellen Vorheizzeiten der Räume kontrolliert werden können. Dabei werden die folgenden Informationen im Systemprotokoll aufgezeichnet: Start des Heizens, Erreichen der Zieltemperatur, Start des Termins, Ende der Heizphase, Ende des Termines. Protokolliert werden Außentemperatur (beim Start), aktuelle Temperatur, bisher erreichte maximale Temperatur. Einträge tragen den Namen *HK-LogHeizkurvenkontrolle*.

## HK-SystemProtokoll sichern.hsc

Um über einen längeren Zeitraum die Systemprotokolle und damit auch die Heizkurven zu kontrollieren, ist es mit diesem Skript möglich das flüchtige Systemprotokoll auf einen USB Stick zu speichern. Das Speichern des Protokolls erfolgt wochenweise. Üblicherweise werden maximal 10 Protokolle gespeichert bevor das älteste Protokoll gelöscht wird.
Dieses Skript sollte jeweils alle 23-87min laufen. Beachten Sie, dass ein Neustart oder Stromausfall für den Verlust der Einträge im Systemprotokoll führen kann, die in diesem Zeiutrazum noch nicht wieder gespeichert wurden.

## HK-Test-Skript.hsc

Dies ist ein einfaches Testskript, dass die aktuellen Einstellungen des Heizkalenders teilweise prüft und alle Daten ausgibt. Dieses Protokoll kann für den Support oder das eigene Archiv genutzt werden.