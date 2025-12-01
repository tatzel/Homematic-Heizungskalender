!// Alle Heizgruppen auf Auto Modus zu setzen
!//================================================================================================
!// Stand:    27.11.2025; 
!// Autor:    Martin Richter    (heizkalender@m-ri.de)
!// Projekt:  Helmut Diedrichs
!//================================================================================================
!// Dieser Code wurde im Rahmen der Heizkalender-Implementierung der Baptisten Gemeinde Hanau 
!// entwickelt.
!// Das Heizkalender-Team freut sich, dass Sie den kostenlosen Heizkalender anwenden und somit 
!// einen Beitrag zum Umweltschutz leisten. Und es wäre schön, wenn Sie die Nutzung per E-Mail an 
!// Info@Heizkalender.de melden. Dadurch ergäbe ich eine Übersicht und zudem die Möglichkeit auf 
!// wichtige Änderungen hinzuweisen. Gerne können Sie auch über Ihre Erfahrung mit dem Heizkalender 
!// berichten.
!//================================================================================================
!//
!// Skript kann einmal in der Nacht in einer ruhigen Nutzungsphase ausgeführt werden.
!// 
!// Der Code wurde auf Basis des folgenden Samples entwicklet.
!//   https://github.com/jollyjinx/homematic/blob/master/ThermostatModeSwitch.hms
!//   https://homematic-forum.de/forum/viewtopic.php?f=26&t=86909
!//
!// MRi: 2025-11-27 Individuelles Schalten eingebaut. Urlaubsmodus wurd enicht korrekt berücksichtigt

string vrp="";

!// RaumListe Syntax: "Heizgruppe=0/1;..", ist diese Liste leer wird die Systemvariable 
!// Tool-Heizgruppen-Modus-Zurücksetzen verwendet. 
string RaumListe                = "";          
!// Ist keine RaumListe über die Varibale oder als Vorgabe definiert setzen wir alle 
!// Thermostate pauschal auf true (Automodus), false (Manuell)
boolean automode                = false;        
!// Debug Ausgabe ein oder aus
boolean debug                   = false;       

!//Logging in "Log" mit 1 zwingend einschalten oder mit 0 Ausschalten
boolean log=0;

!//#######---Ende Variabler Bereich---#############################################################
!//Im Folgenden Hier keine Veränderungen vornehmen!

var logObj=dom.GetObject(vrp+"HK-Log");
var loggingObj=dom.GetObject(vrp+"HK-Logging");

!// Prüfe logging erwartet wird
if ((!log) && loggingObj){
  if (loggingObj.State()!=0){
	  log = true;
  }
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
  !// Haben wir einen RaumListen-Einrtrag
  if (RaumListe!=""){
    !// Raum in der Raumliste suchen.
    integer iPos = RaumListe.Find(";"#device#"=");
    if (iPos<0){
      !// Keine Definiton also stop                                                                                                  
      skip=true;                                                                                          
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
            if(log){logObj.State("Heizgruppe:" # device # " auf \"Auto\" setzen!");}
          }
      
          if( 1!=currentstate )
          {
            dom.GetObject(datapoint#".CONTROL_MODE").State(1);                                      
            if(debug){WriteLine("\t Setting to manual mode");}
            if(log){logObj.State("Heizgruppe:" # device # " auf  \"Manuell\" setzen!");}
          }
        }
        !// Wir können aufhören weiter zu sichen, es gibt nur einen Heating Device Channel
        break;
      }
    }
  }
}
