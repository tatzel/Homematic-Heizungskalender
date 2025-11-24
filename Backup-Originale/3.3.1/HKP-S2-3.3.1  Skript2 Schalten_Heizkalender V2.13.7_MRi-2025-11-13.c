!//Schaltskript für Heizkalender Version MRi/2.13.7 02.10.2025 (MRi 13.11.2025) Lukas Helduser (Youtube: https://www.youtube.com/LukasvandeHaag)
!//Die Nutzung ist kostenlos, aber wir bitten sie zu melden an: helmut@diedrichs.de
!//Ergänzungen von Martin Richter (MRi)
!//Teil 2 Skript zum Schalten der Aktoren.

!//MRi: 2025-11-13	1. Auschaltzyklen Übersprungsicher gemacht! Das Programm muss aber alle 5min laufen
!//					2. Ebenfalls schalten wir alle Heizkörper auf Grundtemperatur zurück und setzen die
!//					Raumvariablen zurück wenn HK2-Hand-Grundtemp gesetzt ist
!//					3. Ist die Schaltliste leer prüfen wir ob noch ein Raum geschaltet ist.	
!//MRi: 2025-11-11	Logging über System Variablen HK2-Log (Text) und HK2-Logging (boolean) eingebaut.
!// 				Damit ds Logging korrekt arbeitet müssen beide Variablen vorhanden sein. Beide sollten
!//					protokolliert werden. Alle Einträge finden sich dann im System Protokoll.
!//MRi: 2025-11-10	Interpolation der Vorlaufzeit aus der Außentemperatur über die HK2-Kurve

!//Eingabe eines Namens Präfix
!//Dies ist nur erforderlich wenn die Namensvorgabe beim erstellen den Systemvariablen geändert wurde.
!//Wird hier ein Präfix eingeben so muss dieser in allen Skripten auch angegeben werden.
string vrp="";


!//Logging in "Log" mit 1 zwingend einschalten oder mit 0 Ausschalten
boolean log=0;

!//#######---Ende Variabler Bereich---####################################################################
!//Im Folgenden Hier keine Veränderungen vornehmen!

string  OffsetAT=dom.GetObject(vrp+"HK2-Kurve").State();
integer OffsetAus=dom.GetObject(vrp+"HK2-VorzeitAus").State();
integer OffsetEin=dom.GetObject(vrp+"HK2-Kurvenversatz").State();
string SListe=dom.GetObject(vrp+"HK1-Schaltliste").State();
string VarNamen=dom.GetObject(vrp+"HK2-HKG-Liste").State();
string RIDI=dom.GetObject(vrp+"HK1-R-Liste").State().ToUpper();
integer ATG=dom.GetObject(vrp+"HK2-A.Temp.Grenze").State();
boolean FW1=dom.GetObject(vrp+"HK2-Hand-Temp").State();
boolean FW2=dom.GetObject(vrp+"HK2-Hand-Grundtemp").State();
integer AT=dom.GetObject(vrp+"HK2-Aussentemperatur").State();
integer GT=dom.GetObject(vrp+"HK2-Grundtemperatur").State();
string AktSR;
boolean flagA=1;
boolean flagB=1;
boolean flagC=1;
integer SID_A=0;
integer SID_B=0;
integer SID_C=0;
integer EIN;
integer AUS;
string  RVN;
string  RVI;
string  AktAktor;
integer HSFlag=0;
boolean HF=0;
integer T=0;
string AGF;
string stdout;
string stderr;
string RTemp;
integer SDFlag;
string Param;
var ux=0.0;
var uy=0.0;
var lx=0.0;
var ly=0.0;
var offset=0.0;
var logObj=dom.GetObject(vrp+"HK2-Log");
var loggingObj=dom.GetObject(vrp+"HK2-Logging");

!// Prüfe logging erwartet wird
if (!log && loggingObj){
  if (loggingObj.State()!=0) {
	  log = true;
  }
}
	
!// Logging auschalten, wenn keine Variable vorhanden
if (!logObj){
	log = false;
}

!//Beginn aussere Schleife
!//Element von der Schaltliste nehmen und die dazu gehörige Raumvariablen suchen
!//Wird die Raumvariable nicht gefunden wird das Skript an der Stelle abgebrochen da ein Fehler in der Konfiguration vorliegt
if(log){logObj.State("Beginn Schaltskriptlauf");}
while(flagA){
  if(SListe.StrValueByIndex(";",SID_A)!=""){
     !//Listenelement auslesen und EIN und AUS Zeit den Offsetwert abziehen
     AktSR=SListe.StrValueByIndex(";",SID_A);
     HSFlag=SListe.StrValueByIndex(";",SID_A+4).ToInteger();
     if((HSFlag!=0)&&(HSFlag!=1)){HSFlag=0;}
     if(HSFlag){
      EIN=SListe.StrValueByIndex(";",SID_A+1).ToInteger()-(OffsetEin*60);
      AUS=SListe.StrValueByIndex(";",SID_A+2).ToInteger()-(OffsetAus*60);
      SDFlag=SListe.StrValueByIndex(";",SID_A+3).ToInteger();
      RTemp=SListe.StrValueByIndex(";",SID_A+3);
      if(log){logObj.State(AktSR +" Heizen - Ein: "+EIN.ToTime().ToString()+" Aus: "+AUS.ToTime().ToString()+"  R-Temp "+RTemp);}
      }else{
      EIN=SListe.StrValueByIndex(";",SID_A+1).ToInteger();
      AUS=SListe.StrValueByIndex(";",SID_A+2).ToInteger();
      SDFlag=SListe.StrValueByIndex(";",SID_A+3).ToInteger();
      RTemp=SListe.StrValueByIndex(";",SID_A+3);
      if(log){logObj.State(AktSR +" Schalten - Ein: "+EIN.ToTime().ToString()+" Aus: "+AUS.ToTime().ToString());}
      }

  !//Beginn innere Schleife
     !//Zum Listenelement passende Raumvariable suchen
     !// Liste der Raumvariablen durchgehen bis Raumvariable gleich der Variable AktSR ist und Variable RVN (Raumvariablennamen) und RVI (Raumvariable Inhalt) fühlen,
     !//Wenn Variable gefunden innerer Schleife für diesen Durchgang beeenden
      while (flagB){
          !//if(log){logObj.State("Gruppenvariable suchen");}
          if(RIDI.StrValueByIndex(";",SID_B)!=""){
            if(AktSR==RIDI.StrValueByIndex(";",SID_B)){
              RVN=VarNamen.StrValueByIndex(";",SID_B);
              RVI=dom.GetObject(RVN).State();
              if(log){logObj.State(AktSR +" Gruppenvariable: "+RVN+" Inhalt: "+RVI);}
              flagB=0;
              }
          SID_B=SID_B+1;
          }else{
            !//Wird keine Raumvariable gefunden innerer und äussere Schleife abbrechen
            flagB=0;
            flagA=0;
            WriteLine("ABBRUCH Raumvariable nicht gefunden!");
            if(log){logObj.State(AktSR +" Gruppenvariable nicht gefunden, Abbruch Skript !!");}
          }
      }!//while
      !//if(log){logObj.State(AktSR +" Ende Gruppenvariablen Suche");}
    !//innere Schleife für nächsten Durchgang in Grundstellung bringen
     SID_B=0;
     flagB=1;

   !//Aktoren und Raumtemp setzen
   AGF=RVI.StrValueByIndex(";",2);
   EIN=EIN-(RVI.StrValueByIndex(";",4).ToInteger()*60);
   AUS=AUS-(RVI.StrValueByIndex(";",5).ToInteger()*60);
   if(log){logObj.State(AktSR +" berechne Ein/Auszeit mit Raum-Zeitversatz "+EIN.ToTime().ToString()+"/ "+AUS.ToTime().ToString());}
   if(RTemp.ToInteger()<=1){
     RTemp=RVI.StrValueByIndex(";",3);
     if((log)&&(RTemp.ToInteger()==-3)){logObj.State(AktSR +" Sonderfunktion erkannt \"Gruppen normalisierung\": "+RTemp);}
     if((log)&&(RTemp.ToInteger()==-1)){logObj.State(AktSR +" Sonderfunktion erkannt \"Gruppen generell AUS\": "+RTemp);}
     if((log)&&(RTemp.ToInteger()==-2)){logObj.State(AktSR +" Sonderfunktion erkannt \"Gruppen generell EIN\": "+RTemp);}
   }

  !//Nach aktueller Aussentemperatur den Offsetwert berechnen und am Einschaltzeitpunkt abziehen und Aktor Paramter setzen
  if (HSFlag==1){
  if(AT<-5.0){
	  lx=-10.0;	  
	  ly = OffsetAT.StrValueByIndex(";",0).ToFloat();
	  ux=-5;
	  uy = OffsetAT.StrValueByIndex(";",1).ToFloat();
  }elseif(AT<0.0){
  	  lx=-5.0;	  
	  ly = OffsetAT.StrValueByIndex(";",1).ToFloat();
	  ux=0.0;
	  uy = OffsetAT.StrValueByIndex(";",2).ToFloat();  
  }elseif(AT<8.0){
  	  lx=0.0;	  
	  ly = OffsetAT.StrValueByIndex(";",2).ToFloat();
	  ux=8.0;
	  uy = OffsetAT.StrValueByIndex(";",3).ToFloat();  
  }elseif(AT<10.0){
  	  lx=8.0;	  
	  ly = OffsetAT.StrValueByIndex(";",3).ToFloat();
	  ux=10.0;
	  uy = OffsetAT.StrValueByIndex(";",4).ToFloat();  
  }elseif(AT<12.0){
  	  lx=10.0;	  
	  ly = OffsetAT.StrValueByIndex(";",4).ToFloat();
	  ux=12.0;
	  uy = OffsetAT.StrValueByIndex(";",5).ToFloat();  
  }elseif(AT<15.0){
  	  lx=12.0;	  
	  ly = OffsetAT.StrValueByIndex(";",5).ToFloat();
	  ux=15.0;
	  uy = OffsetAT.StrValueByIndex(";",6).ToFloat();  
  }elseif(AT<17.5){
  	  lx=15.0;	  
	  ly = OffsetAT.StrValueByIndex(";",6).ToFloat();
	  ux=17.5;
	  uy = OffsetAT.StrValueByIndex(";",7).ToFloat();  
  }else{
	  lx=0.0;
	  ly=0.0;
  }
  !// Interpolieren
  if((lx!=0)||(ly!=0)){	
    offset = (((uy-ly)/(ux-lx))*(AT-lx))+ly;
  }else{
	offset = 0;
  }
  
  EIN=EIN-offset.ToInteger()*60;
    
  if(AGF=="RT"){Param="SET_TEMPERATURE";}
  if(AGF=="TC"){Param="SETPOINT";}
  if(AGF=="IP"){Param="SET_POINT_TEMPERATURE";}
  if(AGF=="IT"){Param="SET_TEMPERATURE";}
  if(log){logObj.State(AktSR +" berechne Startpunkt mit Temp-Zeitversatz "+EIN.ToTime().ToString()+" Parameter: "+AGF);}
  }else{
    Param="STATE";
    if(log){logObj.State(AktSR +"  Parameter: "+AGF);}
  }


  !//Ausschalten generell
  if((EIN<=system.Date("%s").ToInteger())&&(RVI.StrValueByIndex(";",0).ToInteger()!=2)&&(SDFlag==-1)){
    foreach(AktAktor,RVI.Split(";")){
        if (AktAktor.Length()>4){
          if(HSFlag){
                dom.GetObject(AktAktor).DPByHssDP(Param).State(GT);
                if(log){logObj.State(AktAktor+" dauerhaft ausgeschaltet auf Temp: "+GT.ToString());}
          }else{
                dom.GetObject(AktAktor).DPByHssDP(Param).State(0);
                if(log){logObj.State(AktAktor+" dauerhaft ausgeschaltet");}
          }
        }
      }
     dom.GetObject(RVN).State("2;"+RVI.Substr(2,RVI.Length()-2));
     if(log){logObj.State(" Einschaltenmerker "+AktSR+" "+dom.GetObject(RVN).State().StrValueByIndex(";",0));}
    }

  !//Einschalten generell
  if((EIN<=system.Date("%s").ToInteger())&&(RVI.StrValueByIndex(";",0).ToInteger()!=3)&&(SDFlag==-2)){
      foreach(AktAktor,RVI.Split(";")){
        if (AktAktor.Length()>4){
          if(HSFlag){
              dom.GetObject(AktAktor).DPByHssDP(Param).State(RTemp);
              if(log){logObj.State(AktAktor +" dauerhaft eingeschaltet auf Temp: "+RTemp);}
          }else{
              dom.GetObject(AktAktor).DPByHssDP(Param).State(1);
              if(log){logObj.State(AktAktor +" dauerhaft eingeschaltet");}
          }
        }
      }
    dom.GetObject(RVN).State("3;"+RVI.Substr(2,RVI.Length()-2));
    if(log){logObj.State("Einschaltenmerker +"+AktSR+" "+dom.GetObject(RVN).State().StrValueByIndex(";",0));}
  }

  !//Rückstellung
  if((EIN<=system.Date("%s").ToInteger())&&(RVI.StrValueByIndex(";",0).ToInteger()!=4)&&(SDFlag==-3)){
      foreach(AktAktor,RVI.Split(";")){
        if (AktAktor.Length()>4){
          if(HSFlag){
                dom.GetObject(AktAktor).DPByHssDP(Param).State(GT);
                if(log){logObj.State(AktAktor +" Rückstellung aus dauerhaft auf Temp: "+GT.ToString());}
          }else{
                dom.GetObject(AktAktor).DPByHssDP(Param).State(0);
                if(log){logObj.State(AktAktor +" Rückstellung aus dauerhaft");}
          }
        }
      }
      dom.GetObject(RVN).State("0;"+RVI.Substr(2,RVI.Length()-2));
      if(log){logObj.State("Einschaltenmerker +"+AktSR+" "+dom.GetObject(RVN).State().StrValueByIndex(";",0));}
      EIN=EIN+50000;
  }


  !//Ausschalten
  !//Liegt der Ausschaltzeitpunkt des aktuellen Schaltlistenelement in der Vergangenheit dann Raumvariable durchgehen und Aktoren auf Grundtemp bringen WENN Heizung
  !//noch nicht ausgeschaltet ist.
  !//Der Ausschaltzeitpunkt wird 160sec in Zukunft und Vergangenheit (320sec) geprüft. Damit wird ein 5min (300sec) Interval abgedeckt.
  !// Das Skript sollte alle 5m,in laufen.
  if(((AUS-160)<system.Date("%s").ToInteger())&&(RVI.StrValueByIndex(";",0).ToInteger())&&((AUS+160)>system.Date("%s").ToInteger())==1){
      foreach(AktAktor,RVI.Split(";")){
          if (AktAktor.Length()>4){
            if(HSFlag){
              if(FW1==true){
                    if(dom.GetObject(AktAktor).DPByHssDP(Param).State()==RTemp.ToInteger()){
                      dom.GetObject(AktAktor).DPByHssDP(Param).State(GT);
                       !WriteLine("Schalten aus "+AktAktor+" "+Param+" "+GT.ToString());
                       if(log){logObj.State(AktAktor+" Ausschalten (mit Funktion Reglervorrang) auf Temp: "+GT.ToString()+" Parameter: "+Param);}
                     }else{
                       if(log){logObj.State(AktAktor+" Reglervorrang bei AUS (Regeler Haendisch verstellt auf: ("+dom.GetObject(AktAktor).DPByHssDP(Param).State().ToString());}
                     }
                }else{
                    dom.GetObject(AktAktor).DPByHssDP(Param).State(GT);
                     !WriteLine("Schalten aus "+AktAktor+" "+Param+" "+GT.ToString());
                      if(log){logObj.State(AktAktor+" Ausschalten (ohne Funktion Reglervorrang) auf Temp: "+GT.ToString()+" Parameter: "+Param);}
                    }
              }else{
              dom.GetObject(AktAktor).DPByHssDP(Param).State(0);
               !WriteLine("Schalten aus "+AktAktor+" "+Param);
              if(log){logObj.State(AktAktor+" Ausschalten "+Param);}
            }
          }
        }
       dom.GetObject(RVN).State("0;"+RVI.Substr(2,RVI.Length()-2));
       if(log){logObj.State("Einschaltenmerker "+AktSR+" "+dom.GetObject(RVN).State().StrValueByIndex(";",0));}
      }else{
        if(log){logObj.State(AktSR+" Ausschaltpunkt nicht erreicht oder Gruppen schon geschaltet");}
      }

  !//Einschalten
  !//Liegt der Einschaltzeitpunkt in der Vergangenheit UND Ausschaltzeitpunkt in der Zukunft Heizung einschalten WENN diese noch nicht eingeschaltet ist.
  if((EIN<=system.Date("%s").ToInteger())&&(AUS>system.Date("%s").ToInteger())&&(RVI.StrValueByIndex(";",0).ToInteger()==0)){
        foreach(AktAktor,RVI.Split(";")){
          if (AktAktor.Length()>4){
            if(HSFlag){
              if(AT<ATG){
                if(FW1==1){
                    if(dom.GetObject(AktAktor).DPByHssDP(Param).State()==GT){
                        dom.GetObject(AktAktor).DPByHssDP(Param).State(RTemp);
                       !WriteLine("Schalten ein "+AktAktor+" "+Param+" "+RTemp);
                       if(log){logObj.State(AktAktor+" Einschalten (mit Funktion Reglervorrang) auf Temp: "+RTemp+" Parameter: "+Param);}
                    }else{
                        if(log){logObj.State(AktAktor+" Reglervorrang bei EIN (Regeler Haendisch verstellt auf: ("+dom.GetObject(AktAktor).DPByHssDP(Param).State().ToString());}
                        }
                 }else{
                      dom.GetObject(AktAktor).DPByHssDP(Param).State(RTemp);
                       !WriteLine("Schalten ein "+AktAktor+" "+Param+" "+RTemp);
                         if(log){logObj.State(AktAktor+" Einschalten (ohne Funktion Reglervorrang) auf Temp: "+RTemp+" Parameter: "+Param);}
                        }
              }else{
                if(log){logObj.State(AktAktor+" Heizen abgebrochen Aussentemperatur größer Grenzwert "+AT.ToString());}
              }
            }else{
              dom.GetObject(AktAktor).DPByHssDP(Param).State(1);
               !WriteLine("Schalten ein "+AktAktor+" "+Param);
               if(log){logObj.State(AktAktor+" Einschalten "+Param);}
            }
          }
        }
      dom.GetObject(RVN).State("1;"+RVI.Substr(2,RVI.Length()-2));
      if(log){logObj.State("Einschaltenmerker "+AktSR+" "+dom.GetObject(RVN).State().StrValueByIndex(";",0));}
    }else{
      if(log){logObj.State(AktSR+" Einschaltpunkt nicht erreicht oder Gruppen schon geschaltet");}
    }
  !//Debugausgaben
  !WriteLine("Debug");
  !WriteLine(AktSR);
  !WriteLine(EIN);
  !WriteLine(AUS);
  !WriteLine(RVI);
  !WriteLine(RVN);
  !WriteLine(AGF);
  !WriteLine(RTemp);
  !WriteLine("####");

  !//Schleifen ID setzen
     SID_A=SID_A+5;
  }else{
    !//Wenn Schaltliste komplett durchgegangen ist äussere Schleife beeenden
    flagA=0;
    !WriteLine(AT);
  }

}

!// Code für Prüfung der Nachschaltung immer um 01:00 Uhr!
if((system.Date("%H%M").ToInteger()>56)&&(system.Date("%H%M").ToInteger()<104)&&(FW2==true)){
  if(log){logObj.State("Beginn Nachtabschaltung");}
  flagC=true;
  SID_C=0;
  while(flagC){
    if(VarNamen.StrValueByIndex(";",SID_C)!=""){
      RVN=VarNamen.StrValueByIndex(";",SID_C);
      if(log){logObj.State("Gruppe:"+RVN);}
      RVI=dom.GetObject(RVN).State();
      AGF=RVI.StrValueByIndex(";",2);
	  if(AGF=="RT"){Param="SET_TEMPERATURE";}
	  if(AGF=="TC"){Param="SETPOINT";}
	  if(AGF=="IP"){Param="SET_POINT_TEMPERATURE";}
	  if(AGF=="SW"){Param="STATE";}
	  if(AGF=="IT"){Param="SET_TEMPERATURE";}
	  if(RVI.StrValueByIndex(";",1)=="H"){HSFlag=1;}
      if(RVI.StrValueByIndex(";",1)=="S"){HSFlag=0;}
	  !// Heizung oder Schaltung in jedem Fall zurücksetzen.
	  if(RVI.StrValueByIndex(";",0).ToInteger()<=1){
	    foreach(AktAktor,RVI.Split(";")){
		  if ((AktAktor.Length()>4)){
		    if(HSFlag){
		      dom.GetObject(AktAktor).DPByHssDP(Param).State(GT);
		      if(log){logObj.State(AktAktor+" Ausschalten auf Temp: "+GT.ToString()+"Parameter: "+Param);}
		    }else{
		      dom.GetObject(AktAktor).DPByHssDP(Param).State(0);
		      if(log){logObj.State(AktAktor+" Ausschalten "+Param);}
		    }
		  }		   
		}
			 
		!// Reset der Heizvariablen von 1 auf 0, sollten wir in einem Heizzyklus sein,
		!// dann wird dies im nächsten Schaltzyklus wieder geändert. Aber durch diesen Trick beheben
		!// wir eine fälschlich auf 1 verblieben Raum Variable.
		if (RVI.StrValueByIndex(";",0).ToInteger()!=0){
		  dom.GetObject(RVN).State("0;"+RVI.Substr(2,RVI.Length()-2));
		}
	  }
	}else{
	  flagC=0;
	}
	SID_C=SID_C+1;
  }
  if(log){logObj.State("Ende Nachtabschaltung");}
}

!// Code für die Prüfung Schaltliste. Ist die Schaltliste leer, prüfen wir alle Räume,
!// die sich sich im Schaltzustand befinden. Diese werden dann zurückgestellt.
if((system.Date("%H%M").ToInteger()>56)&&(system.Date("%H%M").ToInteger()<104)&&(FW2==true)){
  flagC=true;
  SID_C=0;
  while(flagC){
    if(VarNamen.StrValueByIndex(";",SID_C)!=""){
      RVN=VarNamen.StrValueByIndex(";",SID_C);
      RVI=dom.GetObject(RVN).State();
      AGF=RVI.StrValueByIndex(";",2);
	  if(AGF=="RT"){Param="SET_TEMPERATURE";}
	  if(AGF=="TC"){Param="SETPOINT";}
	  if(AGF=="IP"){Param="SET_POINT_TEMPERATURE";}
	  if(AGF=="SW"){Param="STATE";}
	  if(AGF=="IT"){Param="SET_TEMPERATURE";}
	  if(RVI.StrValueByIndex(";",1)=="H"){HSFlag=1;}
      if(RVI.StrValueByIndex(";",1)=="S"){HSFlag=0;}
	  !// Heizung oder Schaltung in zurücksetzen, wenn diese sich immer noch im Heizen-/Schaltenzustand
	  !// befindet. Evtl. Wurde ein Ausschaltpunkt versäumt
	  if(RVI.StrValueByIndex(";",0).ToInteger()==1){
	    foreach(AktAktor,RVI.Split(";")){
		  if ((AktAktor.Length()>4)){
		    if(HSFlag){
		      dom.GetObject(AktAktor).DPByHssDP(Param).State(GT);
		      if(log){logObj.State("Schaltliste ist leer! Korrektur - "+AktAktor+" Ausschalten auf Temp: "+GT.ToString()+"Parameter: "+Param);}
		    }else{
		      dom.GetObject(AktAktor).DPByHssDP(Param).State(0);
		      if(log){logObj.State("Schaltliste ist leer! Korrektur - "+AktAktor+" Ausschalten "+Param);}
		    }
		  }		   
		}
			 
		!// Reset der Heizvariablen von 1 auf 0. Da die Heizliste leer ist, sollte der Eintrag
		!// in der Raumliste auch auf 0 stehen.
		dom.GetObject(RVN).State("0;"+RVI.Substr(2,RVI.Length()-2));
		if(log){logObj.State("Schaltliste ist leer! Korrektur der Raumliste auf: "+dom.GetObject(RVN).State());}	   
	  }
	}else{
	  flagC=0;
	}
	SID_C=SID_C+1;
  }
}

if(log){logObj.State("Ende Schaltskriptlauf");}
