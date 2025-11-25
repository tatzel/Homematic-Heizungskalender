!// Alle Heizgruppen auf Auto Modus zu setzen
!//================================================================================================
!// Stand:    25.11.2025; 
!// Autor:    Martin Richter    (heizkalender@m-ri.de)
!// Projekt:  Helmut Diedrichs
!//================================================================================================
!// Dieser Code wurde im Rahmen der Heizkalender-Implementierung der Baptisten Gemeinde Hanau 
!// entwicklet.
!// Die Nutzung ist kostenlos, aber wir bitten die Nutzung an einer der obigen Email Adressen zu 
!// melden.
!//================================================================================================
!//
!// Skript kann einmal in der Nacht in einer ruhigen Nutzungsphase ausgeführt werden.
!// 
!// Der Code wurde auf Basis des folgenden Samples entwicklet.
!//   https://github.com/jollyjinx/homematic/blob/master/ThermostatModeSwitch.hms
!//   https://homematic-forum.de/forum/viewtopic.php?f=26&t=86909
!//

string vrp="";

boolean automode                = true;                                 !.Set to true to set all Thermostats to automode, if false all thermostats
                                                                        !.will be set to manu mode and off.
                                                                        !.I actually use the state of a hardware switch to set automode  like
                                                                        !. dom.GetObject("BidCos-RF.KEQ0768205:4.STATE").Value();

boolean debug                   = false;                                !.if set, output what is done

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

!
!------ Execution nothing needs to be changed below this line -----------
!
                                                                                                    if(debug){WriteLine("Automode:"#automode);}

string  deviceid;
foreach(deviceid, dom.GetObject(ID_DEVICES).EnumUsedIDs()) 
{
  var device  = dom.GetObject(deviceid);                                                            if(debug){WriteLine("Device:"#device#" (id:"#deviceid#")");}
                                                  
  !// MRi: Nur meine Heizgruppen
  if("HmIP-HEATING" == device.HssType())
  {   
    string channelid;                                                                                                           
    foreach(channelid,device.Channels().EnumUsedIDs())
    {
      var channel = dom.GetObject(channelid);                                                       if(debug){WriteLine("\t Channel:"#channel#" (id:"#channelid#")");}
      
      !// MRi: Meine Channels
      !WriteLine(channel.HssType());
      if("HEATING_CLIMATECONTROL_TRANSCEIVER" == channel.HssType())
      {
        var     interface   =   dom.GetObject(channel.Interface());
        var     datapoint   =   interface#"."#channel.Address();                                    if(debug){WriteLine("\t Datapoint:"#datapoint);}
        integer currentstate=   dom.GetObject(datapoint#".SET_POINT_MODE").Value();                 if(debug){WriteLine("\t State before:"#currentstate);}
        
        !.modes:    0 = auto, 1 = manu, 2 = Urlaub
        boolean thermostatinautomode = false;
        if( 0==currentstate)
        {
          thermostatinautomode = true;
        }
        
        if( automode != thermostatinautomode )
        {                                                                                           if(debug){WriteLine("\t Device not in suggested mode");}
          if( 0!=currentstate)
          {
            dom.GetObject(datapoint#".CONTROL_MODE").State(0);                                      if(debug){WriteLine("\t Setting to auto mode");}
            if(log){logObj.State("Heizgruppe:" # device # " auf \"Auto\" setzen!");}
          }
      
          if( 1!=currentstate )
          {
            dom.GetObject(datapoint#".CONTROL_MODE").State(1);                                      if(debug){WriteLine("\t Setting to manual mode");}
            if(log){logObj.State("Heizgruppe:" # device # " auf  \"Manuell\" setzen!");}
          }
        }
      }
    }
  }
}
