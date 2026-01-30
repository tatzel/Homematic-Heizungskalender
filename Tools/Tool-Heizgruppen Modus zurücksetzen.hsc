!// Alle Heizgruppen auf Auto Modus zu setzen
!//================================================================================================
!// Stand:    30.01.2026
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
!// Skript kann einmal in der Nacht in einer ruhigen Nutzungsphase ausgeführt werden.
!//
!// Der Code wurde auf Basis des folgenden Samples entwicklet.
!//   https://github.com/jollyjinx/homematic/blob/master/ThermostatModeSwitch.hms
!//   https://homematic-forum.de/forum/viewtopic.php?f=26&t=86909
!//
!// MRi: 2025-11-27 Individuelles Schalten eingebaut. Urlaubsmodus wurde nicht korrekt berücksichtigt

string vrp="";

!// RaumListe Syntax: "Heizgruppe=0/1;..", ist diese Liste leer wird die Systemvariable
!// Tool-Heizgruppen-Modus-Zurücksetzen verwendet.
string RaumListe                = "";
!// Ist keine RaumListe über die Variable oder als Vorgabe definiert setzen wir alle
!// Thermostate pauschal auf true (Automodus), false (Manuell)
boolean automode                = false;
!// Debug Ausgabe ein oder aus
boolean debug                   = false;

!// Logging in "Log" mit 1 zwingend einschalten oder mit -1 zwingend Ausschalten
!// Mit 0 wird die Einstellunge aus der HKx-Logging übernommen
integer log=0;

!//#######---Ende Variabler Bereich---#############################################################
!//Im Folgenden Hier keine Veränderungen vornehmen!

var logObj=dom.GetObject(vrp+"HK-Log");
var loggingObj=dom.GetObject(vrp+"HK-Logging");

!// Prüfe ob logging erwartet wird
if (log<0) {
  !// Zwingend kein logging
  log = false;
} elseif (log==0) {
!// Einstellung der Logging Variable prüfen
  if (loggingObj && loggingObj.State()!=0){
    log = true;
  }
} else {
  !// Logging einschalten
  log = true;
}

!// Logging zwinged auschalten, wenn keine Variable vorhanden ist
if (!logObj){
  log = false;
}

!// Wenn es keine Vorgabe gibt, dann benutzen wir de RaumListe. Gibt es di auch nicht benutzen wir
!// die automode Vorgabe.
if (RaumListe==""){
  var objVar=dom.GetObject(vrp+"Tool-Heizgruppen-Modus-Zurücksetzen");
  if (objVar){
    RaumListe=objVar.State();
  }
}
if (RaumListe!=""){
  !// Um die Suche zu vereinfachen Semikolon am Anfang und am Ende setzen
  RaumListe = ";" # RaumListe # ";";
}
!
!------ Execution nothing needs to be changed below this line -----------
!
if(debug){WriteLine("Automode:"#automode);}
if(debug){WriteLine("RaumListe:"#RaumListe);}
string  deviceid;
foreach(deviceid, dom.GetObject(ID_DEVICES).EnumUsedIDs())
{
  var device  = dom.GetObject(deviceid);
  if(debug){WriteLine("Device:"#device#" (id:"#deviceid#")");}

  !// Prüfen ob welchen Modus wir wollen
  boolean newMode = automode;
  boolean skip = false;
  !// Haben wir einen RaumListen-Eintrag
  if (RaumListe!=""){
    !// Raum in der Raumliste suchen.
    integer iPos = RaumListe.Find(";"#device#"=");
    if (iPos<0){
      !// Keine Definiton also also suchen wir einen Wildcard
      iPos = RaumListe.Find(";*=");
      if (iPos<0){
        !// Auch kein Wildcard, also stop
        skip=true;
      } else {
        !// Lade den Eintrag für den Wildcard.
        newMode = RaumListe.Substr(iPos+3,5).ToInteger()!=0;      
      }
    }else{
      newMode = RaumListe.Substr(iPos+1+device.Name().Length()+1,5).ToInteger()!=0;
      if(debug){WriteLine("\t Neuer Modus aus RaumListe="#newMode);}
    }
  }

  if (skip){
    if(debug){WriteLine("\t Name ist nicht in der Raumliste und wird übersprungen");}
  }

  !// MRi: Nur meine Heizgruppen
  if(("HmIP-HEATING" == device.HssType()) && (!skip))
  {
    string channelid;
    foreach(channelid,device.Channels().EnumUsedIDs())
    {
      var channel = dom.GetObject(channelid);
      if(debug){WriteLine("\t Channel:"#channel#" (id:"#channelid#")");}

      !// MRi: Meine Channels
      !WriteLine(channel.HssType());
      if("HEATING_CLIMATECONTROL_TRANSCEIVER" == channel.HssType())
      {
        var     interface   =   dom.GetObject(channel.Interface());
        var     datapoint   =   interface#"."#channel.Address();
        if(debug){WriteLine("\t Datapoint:"#datapoint);}
        integer currentstate=   dom.GetObject(datapoint#".SET_POINT_MODE").Value();
        if(debug){WriteLine("\t State before:"#currentstate);}

        !//modes:  0 = auto, 1 = manu, 2 = Urlaub
        boolean thermostatinautomode = false;
        if( 0==currentstate)
        {
          thermostatinautomode = true;
        }


        if( newMode != thermostatinautomode )
        {
          if(debug){WriteLine("\t Device not in suggested mode. New mode="#newMode);}
          if( 0!=currentstate)
          {
            dom.GetObject(datapoint#".CONTROL_MODE").State(0);
            if(debug){WriteLine("\t Setting to auto mode");}
            if(log){logObj.State("Moduskorrektur für Heizgruppe:" # device # " auf \"Auto\" setzen!");}
          }

          if( 1!=currentstate )
          {
            dom.GetObject(datapoint#".CONTROL_MODE").State(1);
            if(debug){WriteLine("\t Setting to manual mode");}
            if(log){logObj.State("Moduskorrektur für Heizgruppe:" # device # " auf  \"Manuell\" setzen!");}
          }
        }
        !// Wir können aufhören weiter zu sichen, es gibt nur einen Heating Device Channel
        break;
      }
    }
  }
}
