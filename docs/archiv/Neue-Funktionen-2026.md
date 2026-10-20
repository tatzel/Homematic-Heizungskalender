# Neue Funktionen (historisches Dokument, Stand 2026)

> **Hinweis:** Dies ist ein historisches Release-Begleitdokument (ursprünglich als
> persönliche E-Mail von Martin Richter verfasst). Die aktuellen Inhalte finden
> sich strukturiert im [Anwenderhandbuch](../Anwenderhandbuch.md), der
> [Skript-Referenz](../../Skripte/Dokumentation/Readme.md) und im
> [CHANGELOG](../../CHANGELOG.md). Es wird nur noch zu Referenzzwecken aufbewahrt;
> enthaltene Token/Zugangsdaten wurden durch Platzhalter ersetzt.

## Zusendung der Skripte

Ich werde (wie schon angekündigt) alles demnächst im Paket in einer
ZIP-Datei senden. Das erleichtert das archivieren und auch den Zugang zu
einzelnen Versionen.\
Das Datum der Version steht im Datei Header.

Dadurch lassen sich auch Versionsstände vergleichen.

## GitHub

Ich habe ein privates öffentlich nicht sichtbares) Repository erzeugt,
mit diesen und den alten Dateien. Damit lässt sich vergleichen was wie
war und wie es von mir geändert wurde.

Wenn Ihr einen GitHub Account habt, dann teilt mir Eure E-Mail mit oder
den Benutzernamen auf GitHub bzw. legt Euch einen neuen GitHub Account
zu, sofern Ihr auf das Repository zugreifen wollt.\
Das ist nicht nötig, aber evtl. nützlich für zukünftiges weiteres
gemeinsames Arbeiten.\
Es ist wie gesagt privat und ohne Rücksprache werde ich das nicht
veröffentlichen.

Die Revisionen, wie diese, werde ich dort auch einstellen. Sie sind also
auch rückwirkend verfügbar.

Wichtig war mir die Sicherung und der Verlauf der Änderungen zu
protokollieren und die Sicherheit des Codes zu gewährleisten.

**Ich stelle alle meine Daten auf GiHub ein. Aktuell privat. Sofern man
Zugang möchte benötige ich den GitHub Benutzernamen.**

**Benutzer sind damit automatisch voll berechtigt.**

## CCU2 wird nicht mehr unterstützt

Hintergrund: Ich benutze sehr stark, die Schlüsselwörter *break;* und
*continue;* in meinen Programmen. Die CCU2 hat keine Updates mehr für
die Skriptsprache bekommen, die dies erlaubt. Was die unterstützten
Geräte betrifft hat sich nichts geändert.

## Logging 

Ich habe das Logging ja umgestellt auf die Ausgabe in das System-Skript.
Es war mir zu mühsam immer über Skript-Testen Die Funktion zu prüfen.
Auch waren mir die Log-Ausgaben auf USB Stick zu umständlich. Man kann
ja nicht einfach an die Daten auf dem USB Stick kommen.

Das Skript legt die entsprechenden Variablen an. Über die zwei Variablen
HK1-Logging und HK2-Logging kann man das Log ein- und ausschalten. Ich
betreibe meine CCU3 nur mit Logging.

Die Variablen sind für den Betrieb nicht nötig. Alle Skripte
funktionieren auch so.

Jetzt kann man in der Software der CCU selber das System-Protokoll
öffnen und findet dort alles was in den Skripten passiert, wenn das
Logging eingeschaltet ist.

Vorteil ist auch, dass ein Nutzer einfach das System-Protokoll
exportieren kann und ein „Techniker" kann Fehlersuche anhand des Logs
durchführen.

Da das Systemprotokoll aber nicht gespeichert wird und bei vielen
Log-Ausgaben evtl. nicht mal über einen Tag kommen wir zum nächsten
Punkt.

Ihr findet eine Testausgabe (Beispiel HK-Log_20251119.log) auch unter
den Dateien in der Anlage.

Ihr könnt sehen, dass alles Wissenswertes dort protokolliert wird.

Toll ist einfach auch, dass alle Änderungen der Variablen hier
protokolliert werden! Das erleichtert die Fehlersuche!\
Alles findet sich jetzt im Systemprotokoll.

## Erklärung meiner Benamung

Da ich mit den Namen der Skripte, nicht klargekommen bin und immer
wieder Skripte verwechselt habe bin ich zu einer einfacheren Benamung
übergegangen.

Das Lesen des Verzeichnisses war auch immer schwer.

**Wenn gewünscht kann ich gerne die Dateien in der alten Nomenklatur
senden. Ich habe einen Batch, der die alten Namen für Euch
wiederherstellt.**

**Das ist die Gegenüberstellung alt und neu (ohne MRi und Datum):**

  -----------------------------------------------------------------------------
  **Alter Name**                                **Mein Name**
  --------------------------------------------- -------------------------------
  HKP-AT-1.0.2  Außentemperatur-Open-Meteo      HK-Außentemperatur-Open-Meteo

  HKP-ICS-CD-V-3.2.1 Variablen zu               HK-Init-Skript 1_ChurchDesk
  Skript1_iCal_ChurchDesk_RessourceV1.3         

  HKP-CT-V\--3.2.1 Variablen zu                 HK-Init-Skript 1_CurchTools
  Skript1_ChurchTools_V1.4                      

  HKP-GK-V-3.2.1 Variablen zu                   HK-Init-Skript 1_Google
  Skript1_GoogleKalender_V1.2                   

  HKP-ICS-A-V-3.2.1 Variablen zu                HK-Init-Skript 1_iCal
  Skript1_iCal_V1.2                             

  HKP-S2-V-3.1.1 Variablen zu Skript2_V1.3      HK-Init-Skript 2

  HKP-V-Variablen zum Logging                   HK-Init-Variablen Logging

  HKP-ICS-CD-3.1.1 ICal_ChurchDesk_V1.1.6       HK-Skript 1_ChurchDesk

  HKP-CT-3.3.1 churchtools_Ressource_V2_8_8     HK-Skript 1_ChurchTools

  HKP-GK-3.2.1 Googlekalender_V3_4_4            HK-Skript 1_Google

  HKP-ICS-A-3.1.1  ICal_Skript1_V1.3.2          HK-Skript 1_iCal

  HKP-S2-3.3.1  Skript2 Schalten_Heizkalender   HK-Skript 2
  V2.13.7                                       
  -----------------------------------------------------------------------------

**Die Namen der Skripte sprechend für sich. Und so schön nach Nutzung
gruppiert.**

Alle anderen Dateien sind sowieso neu und von mir. Und haben auch einen
sprechenden Namen (Tools- etc.).

Da alle Dateien Bestandteil einer Zip-Datei sind sollte die Zuordnung
und Versionierung eigentlich einfacher sein, als mit einzelnen Dateien
im Anhang.

## Interpolation der Heizkurve

Auch die Interpolation der Zwischentemperaturen der Heizkurve finde ich
erwähnenswert. Meine Heizkurve ist aktuell fast linear, allerdings muss
das noch kontrolliert werden:\
HK2-Kurve= 162;130;100;59;50;41;30;20

Das macht sich bei langen Zeiten und Temperaturen zwischen 0 und 8 Grad
sehr bemerkbar. Extrem wenn man die original Heizkurve nimmt.

Meine Interpolation nimmt jetzt bei 4 Grad nach der Originalheizkurve
210min. Nach dem originalen Programm wäre er bei 180 geblieben und bei 0
auf 240 gesprungen.\
Hier werden nun alle Zwischenzeiten Minuten- und Gradgenau berechnet.

## Multi-Raum Funktion

Die Multiraum Funktion beeinflusst Skript 1 und Skript 2. Am einfachsten
zeigt sich die Funktion, wenn man meine Gemeinde als Beispiel nimmt.
Siehe auch (Test-Skript Ausgabe)

In meinem Beispiel könnt Ihr sehen, dass ich das sehr stark nutze. Ich
habe 5 Ressourcen in Churchtools:\
*HK1-R-Liste=3;2;4;5;9*

Hier meine Raumparameter Liste:\
*HK2-HKG-Liste=HKG-Raum-GrSaal+HKG-Raum-Foyer+HKG-Raum-WCs;HKG-Raum-KlSaal+HKG-Raum-Foyer+HKG-Raum-WCs;HKG-Raum-Küche;HKG-Raum-JuHu+HKG-Raum-VorraumKüche+HKG-Raum-Dreiecksraum;HKG-Raum-KiGo.*

Aufgeschlüsselt nach Ressourcen Nummer und Namen, seht Ihr, welche Räume
ich schalte:

  -------------------------------------------------------------------------------------------------------------
  Ressource               Raumname                Geschaltete Räume
  ----------------------- ----------------------- -------------------------------------------------------------
  3                       Großer Saal             *HKG-Raum-GrSaal+HKG-Raum-Foyer+HKG-Raum-WCs*

  2                       Kleiner Saal            *HKG-Raum-KlSaal+HKG-Raum-Foyer+HKG-Raum-WCs*

  4                       Küche                   *HKG-Raum-Küche*

  5                       JuHu-Raum               *HKG-Raum-JuHu+HKG-Raum-VorraumKüche+HKG-Raum-Dreiecksraum*

  9                       KiGo-Raum               *HKG-Raum-KiGo*
  -------------------------------------------------------------------------------------------------------------

Wunderbar ist, dass ich jetzt den Räumen eigene Wohlfühltemperaturen
geben kann (WC=18, Foyer=18, alle anderen 20°C). Auch die Vorheizzeiten,
sind jetzt einzeln einzustellen. Bei mir, der Große Saal mit 5h Minimum,
weil wir eine Fußbodenheizung haben, die auch 60min früher abschaltet.

Zusammenfassend: In der HK2-HKG-Liste können Räume/Aktoren durch ein +
Zeichen zusammengefasst werden.\
Jeder Raum hat damit eine eigene Raum-Variable mit eigenem Vorlauf,
eigener Wohlfühltemperatur. Und man kann Heizen/Schalten kombinieren.

Dieses Feature lässt sich abschalten, sollten in den Raumvariablen
bereits Plus-Zeichen verwendet werden. Grundsätzlich ist es
eingeschaltet.

## Behandlung von Terminen und Änderungen in Skript 2

1.  Die Schaltliste wird immer nur für die Termine, die in
    ChurchTools/ChurchDesk-Kalender eingetragen sind übernommen, die
    zeitlich relevant sind.

2.  Wird ein Termin gelöscht (selbst in der Heizphase), wird das Heizen
    sofort beendet.

3.  Wird ein Termin eingetragen und wir befinden uns noch in der Zeit
    zwischen Terminstart und Terminende, wird der Heizvorgang sofort
    gestartet.

4.  Wird ein Raum in einer Heizphase (1 auf erster Stelle der
    Raumvariable) gefunden und die Schaltliste ist leer, oder es findet
    sich kein Schaltliste Eintrag gefunden, dann wird die Heizphase
    beendet (kann passieren, wenn ein Systemneustart erfolgt, zum
    Zeitpunkt des Ausschaltpunktes des Termines)

5.  Die Skripte 1 zur Erzeugung wurden von den Namen der Variablen
    angepasst. Der Aufbau der neuen Heizliste ist jetzt identisch und
    kann dadurch einfach gepflegt werden sollten sich Fehler zeigen.

6.  Ansteuerungen der Thermostate in der Nachtschaltung erfolgt nur,
    wenn die Temperatur abweicht (Reduktion des Duty Cycles)

7.  Wurde die Außentemperatur Grenze erreicht, wurde der Raum auf Heizen
    gestellt (1 auf erster Stelle) und nur das Schalten der Heizung
    verhindert.\
    Ich habe die Logik umgedreht. Ist die Außentemperaturgrenze erreicht
    geht ein Raum nicht mehr in den Heizmodus.

## Init Skript für ChurchTools

Die Version wurde komplett überarbeitet.

Da ich über die ChurchTools API Zugriff auf alle internen IDs und
Ressourcen habe, kann ich auch alles selber bestimmen und vorbelegen.

Vorgehensweise in Kürze:

1.  Im Skript, werden die Variablen: *apiToken* und *gemeindeName*
    vorbelegt.

2.  Das Skript wird ausgeführt.

3.  Über die API werden alle *Ressourcen* vom Typ *Raum* ausgelesen und
    die Ids in der Raum Liste angelegt.

4.  Weiterhin werden für alle Ressourcen Raumvariablen angelegt und
    vorbelegt.

Werden diese Variablen nicht vorbelegt´, werden nur die
Standardvariablen angelegt.\
Werden die Variablen später ausgefüllt und das Skript nochmals
ausgeführt und die HK1-R-Liste und HK2-HKG-Liste sind leer, werden
nicht, existierende Raumvariablen erzeugt.

Existierende Variablen werden nicht geändert, wenn diese existieren.

**Testausgabe des Skriptes mit Vorgabe der Daten für unsere Gemeinde:**

Start \...!

Variable HK1-CT-Gemeindename angelegt

Variable HK1-Schaltliste angelegt

Variable HK1-R-Liste angelegt

Variable HK1-CT-Token angelegt

Variable HK2-HKG-Liste angelegt

Variable HK1-R-Liste wird gesetzt auf: 3;2;5;9;10;4

Raumvariable HKG-Raum-GroßerSaal angelegt! Wert:
0;H;IP;18;0;30;Heizgruppe: GroßerSaal:1

Raumvariable HKG-Raum-KleinerSaal angelegt! Wert:
0;H;IP;18;0;30;Heizgruppe: KleinerSaal:1

Raumvariable HKG-Raum-Ju-HuRaum angelegt! Wert:
0;H;IP;18;0;30;Heizgruppe: Ju-HuRaum:1

Raumvariable HKG-Raum-KiGoRaum angelegt! Wert:
0;H;IP;18;0;30;Heizgruppe: KiGoRaum:1

Raumvariable HKG-Raum-Spielraum angelegt! Wert:
0;H;IP;18;0;30;Heizgruppe: Spielraum:1

Raumvariable HKG-Raum-Küche angelegt! Wert: 0;H;IP;18;0;30;Heizgruppe:
Küche:1

Variable HK2-HKG-Liste wird gesetzt auf:
HKG-Raum-GroßerSaal;HKG-Raum-KleinerSaal;HKG-Raum-Ju-HuRaum;HKG-Raum-KiGoRaum;HKG-Raum-Spielraum;HKG-Raum-Küche

Alles erledigt\...!

**Das Ergebnis. Folgende Variablen werden automatisch angelegt und
befüllt:**

+----------------------+-------------------+------------+-------------------------------------------------------------------------------------------------------------------------+
| Name                 | Beschreibung      | Letzte     | Status                                                                                                                  |
|                      |                   |            |                                                                                                                         |
|                      |                   | Änderung   |                                                                                                                         |
+======================+===================+============+=========================================================================================================================+
| Filter               |                   |            |                                                                                                                         |
+----------------------+-------------------+------------+-------------------------------------------------------------------------------------------------------------------------+
| HK1-CT-Gemeindename  | Name der Gemeinde | 08.12.2025 |   ------------------                                                                                                    |
|                      | in Churchtools    | 11:38:43   |   **baptisten-hu**                                                                                                      |
|                      |                   |            |   ------------------                                                                                                    |
|                      |                   |            |                                                                                                                         |
|                      |                   |            |   ------------------                                                                                                    |
+----------------------+-------------------+------------+-------------------------------------------------------------------------------------------------------------------------+
| HK1-CT-Token         | Sicherheitstoken  | 08.12.2025 |   -----------------------------                                                                                         |
|                      | für ChurchTools   | 11:38:43   |   **&lt;CT-Token&gt;**                                                                                                  |
|                      |                   |            |   -----------------------------                                                                                         |
|                      |                   |            |                                                                                                                         |
|                      |                   |            |   -----------------------------                                                                                         |
+----------------------+-------------------+------------+-------------------------------------------------------------------------------------------------------------------------+
| HK1-R-Liste          | Zuordnung der     | 08.12.2025 |   ------------------                                                                                                    |
|                      | Ressourcen in     | 11:38:43   |   **3;2;5;9;10;4**                                                                                                      |
|                      | ChurchTools       |            |   ------------------                                                                                                    |
|                      |                   |            |                                                                                                                         |
|                      |                   |            |   ------------------                                                                                                    |
+----------------------+-------------------+------------+-------------------------------------------------------------------------------------------------------------------------+
| HK1-Schaltliste      | Schaltliste. Hier | 08.12.2025 |   -------                                                                                                               |
|                      | bitte nichts      | 21:33:13   |                                                                                                                         |
|                      | verändern!        |            |   -------                                                                                                               |
|                      |                   |            |                                                                                                                         |
|                      |                   |            |   -------                                                                                                               |
+----------------------+-------------------+------------+-------------------------------------------------------------------------------------------------------------------------+
| HK2-HKG-Liste        | Liste der         | 08.12.2025 |   --------------------------------------------------------------------------------------------------------------------- |
|                      | HK-Raum-Variablen | 11:38:43   |   **HKG-Raum-GroßerSaal;HKG-Raum-KleinerSaal;HKG-Raum-Ju-HuRaum;HKG-Raum-KiGoRaum;HKG-Raum-Spielraum;HKG-Raum-Küche**   |
|                      |                   |            |   --------------------------------------------------------------------------------------------------------------------- |
|                      |                   |            |                                                                                                                         |
|                      |                   |            |   --------------------------------------------------------------------------------------------------------------------- |
+----------------------+-------------------+------------+-------------------------------------------------------------------------------------------------------------------------+
| HKG-Raum-GroßerSaal  | Raumbeschreibung  | 07.12.2025 |   ------------------------------                                                                                        |
|                      | für HK-Skript 2   | 20:34:47   |   **0;H;IP;18;0;30;Heizgruppe:                                                                                          |
|                      |                   |            |   GroßerSaal:1**                                                                                                        |
|                      |                   |            |   ------------------------------                                                                                        |
|                      |                   |            |                                                                                                                         |
|                      |                   |            |   ------------------------------                                                                                        |
+----------------------+-------------------+------------+-------------------------------------------------------------------------------------------------------------------------+
| HKG-Raum-Ju-HuRaum   | Raumbeschreibung  | 07.12.2025 |   ------------------------------                                                                                        |
|                      | für HK-Skript 2   | 20:34:47   |   **0;H;IP;18;0;30;Heizgruppe:                                                                                          |
|                      |                   |            |   Ju-HuRaum:1**                                                                                                         |
|                      |                   |            |   ------------------------------                                                                                        |
|                      |                   |            |                                                                                                                         |
|                      |                   |            |   ------------------------------                                                                                        |
+----------------------+-------------------+------------+-------------------------------------------------------------------------------------------------------------------------+
| HKG-Raum-KiGoRaum    | Raumbeschreibung  | 07.12.2025 |   ------------------------------                                                                                        |
|                      | für HK-Skript 2   | 20:34:47   |   **0;H;IP;18;0;30;Heizgruppe:                                                                                          |
|                      |                   |            |   KiGoRaum:1**                                                                                                          |
|                      |                   |            |   ------------------------------                                                                                        |
|                      |                   |            |                                                                                                                         |
|                      |                   |            |   ------------------------------                                                                                        |
+----------------------+-------------------+------------+-------------------------------------------------------------------------------------------------------------------------+
| HKG-Raum-KleinerSaal | Raumbeschreibung  | 07.12.2025 |   ------------------------------                                                                                        |
|                      | für HK-Skript 2   | 20:34:47   |   **0;H;IP;18;0;30;Heizgruppe:                                                                                          |
|                      |                   |            |   KleinerSaal:1**                                                                                                       |
|                      |                   |            |   ------------------------------                                                                                        |
|                      |                   |            |                                                                                                                         |
|                      |                   |            |   ------------------------------                                                                                        |
+----------------------+-------------------+------------+-------------------------------------------------------------------------------------------------------------------------+
| HKG-Raum-Küche       | Raumbeschreibung  | 07.12.2025 |   ------------------------------                                                                                        |
|                      | für HK-Skript 2   | 20:34:47   |   **0;H;IP;18;0;30;Heizgruppe:                                                                                          |
|                      |                   |            |   Küche:1**                                                                                                             |
|                      |                   |            |   ------------------------------                                                                                        |
|                      |                   |            |                                                                                                                         |
|                      |                   |            |   ------------------------------                                                                                        |
+----------------------+-------------------+------------+-------------------------------------------------------------------------------------------------------------------------+
| HKG-Raum-Spielraum   | Raumbeschreibung  | 07.12.2025 |   ------------------------------                                                                                        |
|                      | für HK-Skript 2   | 20:34:47   |   **0;H;IP;18;0;30;Heizgruppe:                                                                                          |
|                      |                   |            |   Spielraum:1**                                                                                                         |
|                      |                   |            |   ------------------------------                                                                                        |
|                      |                   |            |                                                                                                                         |
|                      |                   |            |   ------------------------------                                                                                        |
+----------------------+-------------------+------------+-------------------------------------------------------------------------------------------------------------------------+

## Init Skript für ChurchDesk

Die Version wurde komplett überarbeitet.

Da ich über die ChurchDesk API Zugriff auf alle internen IDs und
Ressourcen habe, kann ich auch alles selber bestimmen und vorbelegen.

Vorgehensweise in Kürze:

1.  Im Skript, werden die Variablen: *loginToken* und *organizationId*
    vorbelegt.

2.  Das Skript wird ausgeführt.

3.  Über die API werden alle *Ressourcen* vom Typ *Raum* ausgelesen und
    die Ids in der Raum Liste angelegt.

4.  Weiterhin werden für alle Ressourcen Raumvariablen angelegt und
    vorbelegt.

Werden diese Variablen nicht vorbelegt, werden nur die Standardvariablen
angelegt.\
Werden die Variablen später ausgefüllt und das Skript nochmals
ausgeführt und die HK1-R-Liste und HK2-HKG-Liste sind leer, werden
nicht, existierende Raumvariablen erzeugt.

Existierende Variablen werden nicht geändert, wenn diese existieren.

**Achtung: Hier habe ich eine Systemvariable umbenannt in den Namen, den
ChurchDesk benutzt**

**Testausgabe des Skriptes mit Vorgabe der Daten für meinen ChurchDesk
Test Account:**

Start \...!

Variable HK1-Schaltliste angelegt

Variable HK1-R-Liste angelegt

Variable HK1-CD-OrganisationsId angelegt

Variable HK1-CD-Token angelegt

Variable HK2-HKG-Liste angelegt

Variable HK1-R-Liste wird gesetzt auf: 99694;99695;99696;99697;99698

Raumvariable HKG-Raum-Kirchenraum angelegt! Wert:
0;H;IP;18;0;30;Heizgruppe: Kirchenraum:1

Raumvariable HKG-Raum-Gemeindesaal angelegt! Wert:
0;H;IP;18;0;30;Heizgruppe: Gemeindesaal:1

Raumvariable HKG-Raum-Konferenzraum angelegt! Wert:
0;H;IP;18;0;30;Heizgruppe: Konferenzraum:1

Raumvariable HKG-Raum-Küche angelegt! Wert: 0;H;IP;18;0;30;Heizgruppe:
Küche:1

Raumvariable HKG-Raum-KleinerSaal angelegt! Wert:
0;H;IP;18;0;30;Heizgruppe: KleinerSaal:1

Variable HK2-HKG-Liste wird gesetzt auf:
HKG-Raum-Kirchenraum;HKG-Raum-Gemeindesaal;HKG-Raum-Konferenzraum;HKG-Raum-Küche;HKG-Raum-KleinerSaal

Alles erledigt\...!

 

**Das Ergebnis. Folgende Variablen werden automatisch angelegt und
befüllt:**

+------------------------+-------------------------+------------+---------------------------------------------------------------------------------------------------------------+
| Name                   | Beschreibung            | Letzte     | Status                                                                                                        |
|                        |                         |            |                                                                                                               |
|                        |                         | Änderung   |                                                                                                               |
+========================+=========================+============+===============================================================================================================+
| HK1-CD-OrganisationsId | Liste der               | 06.12.2025 |   ----------                                                                                                  |
|                        | HK-Raum-Variablen       | 21:23:00   |   **&lt;OrganisationsId&gt;**                                                                                 |
|                        |                         |            |   ----------                                                                                                  |
|                        |                         |            |                                                                                                               |
|                        |                         |            |   ----------                                                                                                  |
+------------------------+-------------------------+------------+---------------------------------------------------------------------------------------------------------------+
| HK1-CD-Token           |                         | 06.12.2025 |   ----------------------------                                                                                |
|                        |                         | 21:23:00   |   **&lt;CD-Token&gt;**                                                                                        |
|                        |                         |            |   ----------------------------                                                                                |
|                        |                         |            |                                                                                                               |
|                        |                         |            |   ----------------------------                                                                                |
+------------------------+-------------------------+------------+---------------------------------------------------------------------------------------------------------------+
| HK1-R-Liste            | Zuordnung der Räume aus | 06.12.2025 |   -----------------------------------                                                                         |
|                        | ChurchDeskOrganisations | 21:23:00   |   **99694;99695;99696;99697;99698**                                                                           |
|                        | ID in                   |            |   -----------------------------------                                                                         |
|                        | ChurchDeskAPI-Token für |            |                                                                                                               |
|                        | ChurchDesk              |            |   -----------------------------------                                                                         |
+------------------------+-------------------------+------------+---------------------------------------------------------------------------------------------------------------+
| HK1-Schaltliste        | Schaltliste. Hier bitte | 06.12.2025 |                                                                                                               |
|                        | nichts verändern!       | 21:23:00   |                                                                                                               |
+------------------------+-------------------------+------------+---------------------------------------------------------------------------------------------------------------+
| HK2-HKG-Liste          |                         | 06.12.2025 |   ----------------------------------------------------------------------------------------------------------- |
|                        |                         | 21:23:00   |   **HKG-Raum-Kirchenraum;HKG-Raum-Gemeindesaal;HKG-Raum-Konferenzraum;HKG-Raum-Küche;HKG-Raum-KleinerSaal**   |
|                        |                         |            |   ----------------------------------------------------------------------------------------------------------- |
|                        |                         |            |                                                                                                               |
|                        |                         |            |   ----------------------------------------------------------------------------------------------------------- |
+------------------------+-------------------------+------------+---------------------------------------------------------------------------------------------------------------+
| HKG-Raum-Gemeindesaal  | Raumbeschreibung für    | 06.12.2025 |   ------------------------------                                                                              |
|                        | HK-Skript 2             | 21:23:00   |   **0;H;IP;18;0;30;Heizgruppe:                                                                                |
|                        |                         |            |   Gemeindesaal:1**                                                                                            |
|                        |                         |            |   ------------------------------                                                                              |
|                        |                         |            |                                                                                                               |
|                        |                         |            |   ------------------------------                                                                              |
+------------------------+-------------------------+------------+---------------------------------------------------------------------------------------------------------------+
| HKG-Raum-Kirchenraum   | Raumbeschreibung für    | 06.12.2025 |   ------------------------------                                                                              |
|                        | HK-Skript 2             | 21:23:00   |   **0;H;IP;18;0;30;Heizgruppe:                                                                                |
|                        |                         |            |   Kirchenraum:1**                                                                                             |
|                        |                         |            |   ------------------------------                                                                              |
|                        |                         |            |                                                                                                               |
|                        |                         |            |   ------------------------------                                                                              |
+------------------------+-------------------------+------------+---------------------------------------------------------------------------------------------------------------+
| HKG-Raum-KleinerSaal   | Raumbeschreibung für    | 06.12.2025 |   ------------------------------                                                                              |
|                        | HK-Skript 2             | 21:23:00   |   **0;H;IP;18;0;30;Heizgruppe:                                                                                |
|                        |                         |            |   KleinerSaal:1**                                                                                             |
|                        |                         |            |   ------------------------------                                                                              |
|                        |                         |            |                                                                                                               |
|                        |                         |            |   ------------------------------                                                                              |
+------------------------+-------------------------+------------+---------------------------------------------------------------------------------------------------------------+
| HKG-Raum-Konferenzraum | Raumbeschreibung für    | 06.12.2025 |   ------------------------------                                                                              |
|                        | HK-Skript 2             | 21:23:00   |   **0;H;IP;18;0;30;Heizgruppe:                                                                                |
|                        |                         |            |   Konferenzraum:1**                                                                                           |
|                        |                         |            |   ------------------------------                                                                              |
|                        |                         |            |                                                                                                               |
|                        |                         |            |   ------------------------------                                                                              |
+------------------------+-------------------------+------------+---------------------------------------------------------------------------------------------------------------+
| HKG-Raum-Küche         | Raumbeschreibung für    | 06.12.2025 |   ------------------------------                                                                              |
|                        | HK-Skript 2             | 21:23:00   |   **0;H;IP;18;0;30;Heizgruppe:                                                                                |
|                        |                         |            |   Küche:1**                                                                                                   |
|                        |                         |            |   ------------------------------                                                                              |
|                        |                         |            |                                                                                                               |
|                        |                         |            |   ------------------------------                                                                              |
+------------------------+-------------------------+------------+---------------------------------------------------------------------------------------------------------------+

## Sonderbefehle ChurchTools/ChurchDesk

**Sonderbefehle**

Folgende Sonderbefehle werden jetzt verarbeitet. (Groß Kleinschreibung
spielt keine Rolle:

#EIN# - Dauerhaft an

#AUS#  - Dauerhaft aus

#RESET# / #NORMAL# - Dauerhaft ein/aus aufheben.

#GT# - Wohlfühltemperatur ignorieren und Grundtemperatur schalten.

#NS# / #NH# - Nicht schalten/ nicht heizen. (Siehe unten)

#\<Zahl\>\<Text\># - Setzt die gewünschte Temperatur (Minimal 0, maximal
30), wird begrenzt durch den Aktor.

Der Text muss auf den 50 Zeichen der Beschreibung in der Ressource
stehen. (Nicht in der Termin Beschreibung)-

Erklärungen:

- Der letzte Parameter #NS# / #NH# wurde eingebaut für den Fall bei uns,
  wenn eine Reinigungsfirma bei uns die Räume reinigt. In diesem Fall
  will ich die Ressource (diese Räume) belegen aber nicht Heizen. Wird
  dieser Parameter verwendet, dann entscheidet Skript 1, dass dieser
  Termin nicht in die Schaltliste aufgenommen wird.

- #GT# ist einfach ein Kürzel, um die gesetzte Grundtemperatur zu
  setzen. Einfach nur eine Abkürzung.

## Skript 1 ChurchDesk API

Die Version wurde neu geschrieben.\
Es werden die gleichen Sonderbefehle verarbeitet wie bei ChurchTools,
wenn dies im „Interne Notizen" abgelegt.

Der Vorteil hier gegenüber der iCal Version ist die schnellere
Übertragung und weitaus weniger Datentransfer. Es belastet die CCU3 viel
weniger. Und natürlich, die Sonderbefehle, die in der iCal Version nicht
verwendet werden können, denn würde der Text in der öffentlichen
Beschreibung stehen.

## Skript 1 ChurchTools iCal

Die Version wurde komplett überarbeitet.

Hier können keine Sonderbefehle verarbeitet werden!

## Skript 2 Schaltskript. Behandlung der Sonderbefehle korrigiert

Hier wurden Korrekturen gemacht, weil ich Probleme mit den #EIN# und
#NORMAL# gab. Ich hatte Probleme, dass #NORMAL# gar nicht ausgeführt
wurde, weil in der Schalliste ein falscher Wert stand.  Ich habe ein
zusätzliches Feature eingebaut für die Temperatur Verschiebung nach
vorne. Wir haben eine feste Verschiebung und eine Verschiebung per
Temperatur.

## Optionale Heizkurvenanpassung

Ich habe Heizkurven für einzelne Räume bei uns errechnet und kam zu dem
Problem, dass bei zwei außen liegenden Räumen mit vielen Fenstern, die
normale Heizkurve zu flach war. Eine feste Verschiebung passt da auch
nicht. Also habe ich (optional) für den 5ten Raumparameter einen Faktor
eingebaut.

*0;H;IP;18;30;60* bedeutet, dass wir 30min+Temperaturverschiebung vorher
einschalten und 1h vorher abschalten. Nun kann der 5te Parameter um
einen Faktor erweitert werden

*0;H;IP;18;10\*1.25;60* bedeutet, dass wir 10min vorher einschalten aber
die Verschiebung für die Temperatur mit dem Faktor 1.25 mal nehmen, d.h.
noch mal um 25% verlängern und damit die Heizkurve steiler machen.

Faktoren sind als Wert zwischen 0.25 (1/4) und 3 (x3) erlaubt. Falsche
oder fehlende Werte werden als Faktor=1.0 gewertet.

## Nachtschaltung

Die Nachschaltung überwacht nun auch Dauer-Ein / Dauer-Aus und Aus. Ist
ein Raum in einem Schaltzyklus bleibt dieser unverändert.

Alle Aktoren werden mit Ist- und Sollzustand kontrolliert. Gibt es eine
Abweichung wird der Sollzustand (Grundtemperatur, Raumtemperatur bei
Dauer-Ein und Dauer-Aus) wieder hergestellt.

## Neuer Export für CloudMatic Daten auf den USB Stick

Bisher kann man Diagramm Daten aus der CloudMatic exportieren. Aber das
geht nicht automatisch. Zudem ist (je nach Intervall) das Zeitfenster
nur etwas mehr als ein Tag.

Ich kann nun mit dem neuen Skript, alle Diagrammdaten auf den USB Stick
schreiben (wochenweise).

Das Skript sollte 1-5mal am Tag laufen.

## Speichern des Systemprotokolls

Das Systemprotokoll ist für mich die beste Quelle um Probleme zu finden.
Ich hatte bereits Code gesendet um die Daten auf einem USB Stick zu
speichern.

Ich habe das Skript verbessert. Es benötigt nun weniger Ressourcen und
ist schneller. Vor allem kommt es jetzt damit klar, dass im
Systemprotokoll, mehrere Tage vergangen sein können nach dem letzten
speichern...

## Heizkurvenverlauf von Räumen

In der neuen Version habe ich ein spezielles Skript gebaut, dass den
Heizverlauf eines Raumes protokolliert. Dazu werden 5 markante wichtige
Punkte protokolliert.

Der Heizbeginn, mit Start Temperatur, Zieltemperatur, und jeweiligen
erreichten Maximaltemperatur sowie, Außentemperatur.

Hier mal eine Beispielausgabe für einen Raum:

2026-01-08 11:10:30     HK-LogHeizkurvenkontrolle     GrSaal (3)-GrSaal
Heizbeginn: -379min, Ist: 17.2 Ziel: 19.0, AT: 0.9

2026-01-08 17:30:30     HK-LogHeizkurvenkontrolle     GrSaal (3)-GrSaal
Terminbeginn: +0min, Ist: 18.4 Ziel: 19.0, Max.: 18.4

2026-01-08 19:30:30     HK-LogHeizkurvenkontrolle     GrSaal (3)-GrSaal
Zieltemperatur: +120min, Ist: 19.2 Ziel: 19.0, Max.: 19.2

2026-01-08 20:30:30     HK-LogHeizkurvenkontrolle     GrSaal (3)-GrSaal
Heizende: +180min, Ist: 19.6 Ziel: 19.0, Max.: 19.6

2026-01-08 22:00:30     HK-LogHeizkurvenkontrolle     GrSaal (3)-GrSaal
Terminende: +270min, Ist: 19.2 Ziel: 19.0, Max.: 19.8

Das erlaubt eine gute Kontrolle, ob die Vorgaben funktionieren oder ob
die Zieltemperaturen gar nicht erreicht werden oder viel zu früh
erreicht werden.

Gleichfalls gibt es ein gutes Bild, ob die vorzeitige Abschaltung der
Heizung so funktioniert, dass es keinen Komfort Einbuße gibt.

Die Heizkurvenkontrolle wurde in das normale System Logging integriert
und ist nun ein Standard Skript für den Heizkalender.

## Wöchentliche Log Dateien

Logdateien des Systemprotokolls können nun auch wöchentlich angelegt
werden. Täglich erschien mir zu granular und war in der letzten Zeit
unpraktisch.

## Anpassung der Nutzung der HK1-R-Liste

Bisher wurden in dieser Variable nur Ressourcen Ids (Kalendernummern)
eingetragen.

Für die Nutzung des Installationsprogrammes sind die Namen der
Ressourcen aber wichtig und informativ. Diese können nun mit meinen
neuen Init Skripts ausgelesen werden.

Auch der Skript-1 Varianten von mir nutzen diese Ressourcen.

Eine typische HK1-R-Liste (hier Churchdesk) sieht jetzt so aus:

**99694=Kirchenraum;99695=Gemeindesaal;99696=Konferenzraum;99697=Küche;99698=KleinerSaal;100175=Test**

Angepasst wurden Skript 1 (ChurchDesk,ChurchTools) Skript 2.

**Achtung: Die Varianten Google und iCal wurden bisher nicht angepasst
aus Zeitmangel. Diese Versionen gelten auch in meiner Version als
ungeprüft... und sind nicht im Installationsprogramm integriert (siehe
unten)**

Diese Namen werden nun auch in den Logs benutzt. Das ist besser
zuzuordnen als nur die Nummern.

**Diese Änderung ist Optional und vollkommen Rückwärtkompatibel.**

## Heizkalender Installer

## Anpassung der Vorheizzeit an aktuelle Raum Temperatur, Grundtemperatur und Wohlfühltemperatur

Die Vorheizzeit, die man individuell und in einer Raumvariable definiert
plus die Vorheizzeit, die wieder durch die Außentemperatur errechnet
wird, sollte ausreichen einen Raum von der Grundtemperatur auf die
Wohlfühltemperatur zu bringen.

Das funktioniert auch gut bei uns. Nur habe ich die Beobachtung gemacht,
dass durch äußere Umstände (vorhergehende Termine, angrenzend beheize
Räume, starke Sonneneinstrahlung) manche Räume schon eine höhere
Grundtemperatur haben als vorgegeben ist. Durch die angegebene und
errechnete Vorheizzeit ist der Raum in vielen Fällen viel zu früh
bereits auf Wohlfühltemperatur. D.h. die Vorheizzeit könnte verkürzt
werden. Das ist auch bei uns auch stark an Räumen zu beobachten, die von
anderen beheizten Räumen „profitieren". Unser kleiner Saal liegt am
großen Saal, der durch lange Vorheizzeiten, der Fußbodenheizung auch den
kleinen Saal „mitzieht".

Ich berechne in der neuen Version nun einen linearen Faktor aus der
Grundtemperatur und der Solltemperatur, wie aus der Ist-Temperatur und
der Solltemperatur. Um diesen Faktor wird die Vorheizzeit, die im Raum
definiert ist und auch die Vorheizzeit, die über die Außentemperatur
ermittelt wird, um diesen Faktor.

Beispiel:

- Ein Raum hat eine Grundtemperatur von normal 16°C

- Die Zieltemperatur soll 20°C sein.

<!-- -->

- Der Raum hat aber bereits eine Temperatur von 18°C

- Die Vorheizzeit errechnet aus der Außentemperatur ist 100min
  zusätzlich ist die definierte Vorheizzeit für den Raum 300min
  (Fußbodenheizung).

Der neue Algorithmus kürzt nun die die beiden Vorheizzeiten (in Summe
400min) auf 200min, da ja bereits die Hälfte Temperaturdifferenz zur
Zieltemperatur erreicht wurde.

Im Extremfall, beobachtet das System, dass ein Raum bereits auf der
Zieltemperatur liegt (vorhergehende Termine) und setzt die Vorheizzeit
auf 0.

Da alle 5min im Skript 2 eine neue Berechnung der Startzeit erfolgt,
werden auch sich weiter veränderte Temperaturen korrekt berücksichtigt,
d.h. weiter abkühlender Raum.

Liebe Grüße

Martin
