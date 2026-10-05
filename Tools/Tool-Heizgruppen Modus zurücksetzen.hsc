!// Alle Heizgruppen auf Auto Modus zu setzen
!//================================================================================================
!// Stand:    05.10.2026
!// Autor:    Martin Richter    (heizkalender@m-ri.de) http://blog.m-ri.de/
!// Projekt:  Helmut Diedrichs  (helmut@diedrichs.de) https://diedrichs.de
!//------------------------------------------------------------------------------------------------
!// Copyright (C) 2026 Martin Richter (xMRi-Software)
!// Dieser Teil des Heizkalenders ist freie Software und wird unter der GNU General Public License
!// Version 3 (GPLv3) oder neuer veröffentlicht.
!// Es besteht keinerlei Garantie oder Haftung. Nutzung auf eigene Verantwortung.
!//================================================================================================
!//
!// Skript kann einmal in der Nacht in einer ruhigen Nutzungsphase ausgeführt werden.
!//
!// Der Code wurde auf Basis des folgenden Samples entwickelt.
!//   https://github.com/jollyjinx/homematic/blob/master/ThermostatModeSwitch.hms
!//   https://homematic-forum.de/forum/viewtopic.php?f=26&t=86909
!//
!// TT:  2026-10-05 Sonderfall eTRV-Direktsteuerung in eigenes Skript ausgelagert
!//                 (Tool-Heizgruppen eTRV Modus setzen.hsc); Tippfehler und
!//                 englische Debug-Ausgaben korrigiert
!// MRi: 2026-01-31 Raumliste mit Wildcard eingebaut
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
!// Mit 0 wird die Einstellung aus der HKx-Logging übernommen
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
  if (loggingObj && loggingObj.State()){
    log = true;
  }
} else {
  !// Logging einschalten
  log = true;
}

!// Logging zwingend ausschalten, wenn keine Variable vorhanden ist
if (!logObj){
  log = false;
}

!// Wenn es keine Vorgabe gibt, dann benutzen wir die RaumListe. Gibt es die auch nicht, benutzen wir
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
!// Ausführung: ab hier nichts mehr verändern
!
if(debug){WriteLine("Automode:"#automode);}
if(debug){WriteLine("RaumListe:"#RaumListe);}
string  deviceid;
foreach(deviceid, dom.GetObject(ID_DEVICES).EnumUsedIDs())
{
  var device  = dom.GetObject(deviceid);
  if(debug){WriteLine("Device:"#device#" HssType="#device.HssType()#" (id:"#deviceid#")");}

  !// Prüfen ob welchen Modus wir wollen
  boolean newMode = automode;
  boolean skip = false;
  !// Haben wir einen RaumListen-Eintrag
  if (RaumListe!=""){
    !// Raum in der Raumliste suchen.
    integer iPos = RaumListe.Find(";"#device#"=");
    if (iPos<0){
      !// Keine Definition, also suchen wir einen Wildcard
      iPos = RaumListe.Find(";*=");
      if (iPos<0){
        !// Auch kein Wildcard, also stop
        skip=true;
      } else {
        !// Lade den Eintrag für den Wildcard.
        newMode = RaumListe.Substr(iPos+3,5).ToInteger()!=0;
        if(debug){WriteLine("\t Neuer Modus aus RaumListe="#newMode);}
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
        if(debug){WriteLine("\t Zustand vorher: "#currentstate);}

        !//modes:  0 = auto, 1 = manu, 2 = Urlaub
        boolean thermostatinautomode = false;
        if( 0==currentstate)
        {
          thermostatinautomode = true;
        }


        if( newMode != thermostatinautomode )
        {
          if(debug){WriteLine("\t Gerät nicht im gewünschten Modus. Neuer Modus="#newMode);}
          if( 0!=currentstate)
          {
            dom.GetObject(datapoint#".CONTROL_MODE").State(0);
            if(debug){WriteLine("\t Setze auf Auto");}
            if(log){logObj.State("Moduskorrektur für Heizgruppe:" # device # " auf \"Auto\" setzen!");}
          } elseif( 1!=currentstate )
          {
            dom.GetObject(datapoint#".CONTROL_MODE").State(1);
            if(debug){WriteLine("\t Setze auf Manuell");}
            if(log){logObj.State("Moduskorrektur für Heizgruppe:" # device # " auf \"Manuell\" setzen!");}
          }
        }
        !// Wir können aufhören weiter zu suchen, es gibt nur einen Heating Device Channel
        break;
      }
    }
  }

}
