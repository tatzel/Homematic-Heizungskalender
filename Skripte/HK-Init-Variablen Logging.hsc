!// Skript zum Erstellen der Systemvariablen des Heizkalender für das Logging
!//================================================================================================
!// Stand:    23.01.2026
!// Autor:    Martin Richter    (heizkalender@m-ri.de) http://blog.m-ri.de/
!// Projekt:  Helmut Diedrichs  (helmut@diedrichs.de) https://diedrichs.de
!//------------------------------------------------------------------------------------------------
!// Copyright (C) 2026 Martin Richter (xMRi-Software)
!// Dieser Teil des Heizkalenders ist freie Software und wird unter der GNU General Public License 
!// Version 3 (GPLv3) oder neuer veröffentlicht.
!// Es besteht keinerlei Garantie oder Haftung. Nutzung auf eigene Verantwortung.
!//================================================================================================
!// Der Heizkalender ist eine Idee von Helmut W. Diedrichs und wurde erstmals 2019 in der
!// Stadtmission Arheilgen angewendet Lukas Helduser entwickelte 2023 auf der Bais von Homematic
!// das Heizkalender-Programm für die Allgemeinheit, inkl, Varianten.
!// Dank an die seitherigen Anwender für ihre Verbesserungsvorschläge, insbesondere an die Pilot-
!// Gemeinden. Dieser Code wurde im Rahmen der Heizkalender-Implementierung der Baptisten Gemeinde
!// Hanau von Martin Richter optimiert.
!// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
!// Das Heizkalender-Team freut sich, dass Sie den kostenlosen Heizkalender anwenden und somit einen
!// Beitrag zum Umweltschutz leisten. Es wäre schön, wenn Sie die Nutzung per E-Mail anzeigen an:
!// >>>>> info@heizkalender.de <<<<<
!// Dadurch ergäbe ich eine Übersicht und die Möglichkeit auf Änderungen hinzuweisen. Bitte
!// berichten auch Sie über Ihre Erfahrung mit dem Heizkalender.
!//================================================================================================
!//
!// Anleitung um das Skript auszuführen:
!// - Menü WebUI der CCU
!// - Kopieren sie den Inhalt dieses Datei komplett und unverändert in das Fenster:
!//   => Programm und Verknüpfungen => Skripte Testen
!// - Betätigen sie den Button "Ausführen", im Ausgabe Fenster sehen sie ob und welche Variablen
!//   angelegt wurden (als Info).
!// - Beim erscheinen des Text "alles erledigt" wurde das Skript komplett ausgeführt.
!// Sie können das Fenster dann wieder schließen.
!// im Menü Status und Bedienung => Systemvariable die Systemvariablen kontrollieren
!// Werte können nur über HQ-WebUI, oder den CloudMatic Zugang geesetzt werden

!// Die Variablen HK1-Logging;HK1-Log;HK2-Logging;HK2-Log werden für das interne Logging in Skript1 und
!// Skript2 verwenden in der MRI.
!// HK-SystemProtokollSichern wird verwendet um das System Protokoll regelmässig auf einen
!// USB Stick zu speichern.

!// Code stammt von:
!//   https://www.blogging-it.com/code-snippet-homematic-systemvariablen-ueber-ein-script-automatisch-erzeugen/hausautomatisierung-smart-home/homematic-ip/homematic-script.html

!//Eingabe eines Namens Präfix
!//Dies ist nur erforderlich wenn die Namensvorgabe abgeändert werden soll.
!//Wird hier ein Präfix eingeben so muss dieser in allen Skripten auch angegeben werden.
string vrp="";

!//################################################################################################
!//######------Skript Variablen und Skript Arbeitsteil. Vom Benutzer nicht zu verändern------######

!// Namen der Variablen, die angelegt werden sollen
string nm=  "HK-Logging;" #
            "HK-Log;" #
            "HK1-Logging;" #
            "HK1-Log;" #
            "HK2-Logging;" #
            "HK2-Log;" #
            "HK-LoggingHeizkurvenkontrolle;" #
            "HK-LogHeizkurvenkontrolle;" #
            "HK-RäumeHeizkurvenkontrolle;";

!// Beschreibungstexte
string be=  "Flag um das Logging für Tools ein- und auszuschalten;" #
            "Variable um ein einfaches Log im System Protokoll zu erzeugen;" #
            "Flag um das Logging für Skript 1 ein- und auszuschalten;" #
            "Variable um ein einfaches Log im System Protokoll zu erzeugen;" #
            "Flag um das Logging für Skript 2 ein- und auszuschalten;" #
            "Variable um ein einfaches Log im System Protokoll zu erzeugen;" #
            "Flag um das Logging für die Heizkurvenkontrolle ein- und auszuschalten;" #
            "Variable um ein einfaches Log im System Protokoll zu erzeugen;" #
            "Aktuelle Raumdaten in der Heizkurvenkontrlle;";

!// Typen
string tp=  "boolean;" #
            "string;" #
            "boolean;" #
            "string;" #
            "boolean;" #
            "string;" #
            "boolean;" #
            "string;" #
            "string;";

!// Vorgabe Werte
string vl=  "1;" #
            ";" #
            "1;" #
            ";" #
            "1;" #
            ";" #
            "1;" #
            ";" #
            ";";

!// Einheit (z.B. °C)
string vu=  "";

!// Zusätzliche Info Texte für
!//   - Boolean   - Werte für eine Variable durch + getrennt
!//   - Numerisch - min/max durch + getrennt
string wr=  "Logging ausgeschaltet+Logging eingeschaltet;" #
            ";" #
            "Logging ausgeschaltet+Logging eingeschaltet;" #
            ";" #
            "Logging ausgeschaltet+Logging eingeschaltet;" #
            ";" #
            "Logging ausgeschaltet+Logging eingeschaltet;" #
            ";" #
            ";";


!// Protokollierungs Flags
string pr=  "1;" #
            "1;" #
            "1;" #
            "1;" #
            "1;" #
            "1;" #
            "1;" #
            "1;" #
            "1;";

!//------------------------------------------------------------------------------------------------
!// Ab hier Standard Code zum erzeugen von Variablen
integer i=0;

WriteLine("Start ...!");
while(true){
  if (nm.StrValueByIndex(";",i)==""){
    break;
  }
  string name=vrp+nm.StrValueByIndex(";",i);
  object  svObj  = dom.GetObject(name);
  if (!svObj){
    !// Variable existiert nicht
    object svObjects = dom.GetObject(ID_SYSTEM_VARIABLES);
    svObj = dom.CreateObject(OT_VARDP);
    svObjects.Add(svObj.ID());

    svObj.Name(name);
    svObj.DPInfo(be.StrValueByIndex(";",i));
    svObj.ValueUnit(vu.StrValueByIndex(";",i));
    svObj.DPArchive(pr.StrValueByIndex(";",i).ToInteger()!=0);
    svObj.Internal(false);
    svObj.Visible(true);

    !// Nach typ optionale Werte setzen
    string typ = tp.StrValueByIndex(";",i);
    if(typ=="string"){
      svObj.ValueType(ivtString);
      svObj.ValueSubType(istChar8859);
      svObj.State(vl.StrValueByIndex(";",i));
    }elseif(typ=="integer"){
      svObj.ValueType(ivtFloat);
      svObj.ValueSubType(istGeneric);
      svObj.State(vl.StrValueByIndex(";",i).ToFloat());
      svObj.ValueMin(wr.StrValueByIndex(";",i).StrValueByIndex("+",0).ToFloat());
      svObj.ValueMax(wr.StrValueByIndex(";",i).StrValueByIndex("+",1).ToFloat());
    }elseif(typ=="boolean"){
      svObj.ValueType(ivtBinary);
      svObj.ValueSubType(istBool);
      svObj.ValueName0(wr.StrValueByIndex(";",i).StrValueByIndex("+",0));
      svObj.ValueName1(wr.StrValueByIndex(";",i).StrValueByIndex("+",1));
      svObj.State(vl.StrValueByIndex(";",i).ToInteger());
    }else{
      quit;
    }

    WriteLine("Variable " # name # " angelegt")
  }else{
    WriteLine("Variable " # name # " existiert schon!")
  }
  i=i+1;
}

!// Update der ReGa Skript Engine. Siehe:
!//     https://homematic-forum.de/forum/viewtopic.php?t=45380
dom.RTUpdate(0);

WriteLine("Alles erledigt...!");
