!// Skript 2 für das Schalten der Heizgruppen
!//================================================================================================
!// Stand:    26.11.2025; 
!// Autor:    Lukas Helduser
!//           Martin Richter    (heizkalender@m-ri.de)
!// Projekt:  Helmut Diedrichs  (helmut@diedrichs.de)
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
!// Der Code basiert in großen Teilen auf der Datei: 
!//  HKP-S2-3.2.1  Skript2 Schalten_Heizkalender V2.13.7.c
!// Der ursprüngliche Code wurde geschrieben von:
!//   Lukas Helduser (Youtube: https://www.youtube.com/LukasvandeHaag) 
!// Ich (MRi) habe diesen Code dann erweitert, korrigiert und verbessert um sie an die Nutzung in
!// meiner Gemeinde anzupassen.
!//
!// Skript sollte alle 5min laufen
!//
!// MRi: 2025-11-26 HK1-R-ListeNamen fest eingebaut für verbesseters Logging
!// MRi: 2025-11-21 Kein Einschalten, wenn Schaltezit <5min oder Ausschalktzeitpunkt vor Einschlatzeitpunkt liegt
!// MRi: 2025-11-20 Logging verbessert. 
!// MRi: 2025-11-18 Logging verbessert, Behandlung der Grenztemperatur fürs Heizen Übersteuerung geändert 
!// MRi: 2025-11-14 MultiRaumVariante, damit lassen sich mehrere Räume einer Ressource zuordnen.
!// MRi: 2025-11-13	Log-Ausgaben verbessert und präzisiert.
!// MRi: 2025-11-13	1. Auschaltzyklen Übersprungsicher gemacht! Das Programm muss aber alle 5min laufen
!//					        2. Ebenfalls schalten wir alle Heizkörper auf Grundtemperatur zurück und setzen die
!//					           Raumvariablen zurück wenn HK2-Hand-Grundtemp gesetzt ist
!//					        3. Ist die Schaltliste leer prüfen wir ob noch ein Raum geschaltet ist.	
!// MRi: 2025-11-11	Logging über System Variablen HK2-Log (Text) und HK2-Logging (boolean) eingebaut.
!// 				        Damit das Logging korrekt arbeitet müssen beide Variablen vorhanden sein. Beide sollten
!//					        protokolliert werden. Alle Einträge finden sich dann im System Protokoll.
!// MRi: 2025-11-10	Interpolation der Vorlaufzeit aus der Außentemperatur über die HK2-Kurve

!//Eingabe eines Namens Präfix
!//Dies ist nur erforderlich wenn die Namensvorgabe beim erstellen den Systemvariablen geändert wurde.
!//Wird hier ein Präfix eingeben so muss dieser in allen Skripten auch angegeben werden.
string vrp="";

!//Multiraum Variante, dies unterstützt eine Raumliste in der mehrere Räume mit einem + gemeinsm geschaltet werden können.
boolean multiRaumVariante=true;

!//Logging in "Log" mit 1 zwingend einschalten oder mit 0 Ausschalten
boolean log=0;

!//#######---Ende Variabler Bereich---#############################################################
!//Im Folgenden Hier keine Veränderungen vornehmen!

string  OffsetAT=dom.GetObject(vrp+"HK2-Kurve").State();
integer OffsetAus=dom.GetObject(vrp+"HK2-VorzeitAus").State();
integer OffsetEin=dom.GetObject(vrp+"HK2-Kurvenversatz").State();
string SListe=dom.GetObject(vrp+"HK1-Schaltliste").State();
string VarNamen=dom.GetObject(vrp+"HK2-HKG-Liste").State();
string RIDI=dom.GetObject(vrp+"HK1-R-Liste").State().ToUpper();
var obj=dom.GetObject(vrp+"HK1-R-ListeNamen");
string RIDINamen="";
if (obj){
  RIDINamen = obj.State();
}
integer ATG=dom.GetObject(vrp+"HK2-A.Temp.Grenze").State().ToFloat();
boolean FW1=dom.GetObject(vrp+"HK2-Hand-Temp").State();
boolean FW2=dom.GetObject(vrp+"HK2-Hand-Grundtemp").State();
integer AT=dom.GetObject(vrp+"HK2-Aussentemperatur").State().ToFloat();
integer GT=dom.GetObject(vrp+"HK2-Grundtemperatur").State();
string AktSR;
string AktSRName;
integer SID_SListe=0;
integer SID_RListe=0;
integer EIN;
integer AUS;
string  RVN;
string  RVNListe;
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
integer NOW=system.Date("%s").ToInteger();
var ux=0.0;
var uy=0.0;
var lx=0.0;
var ly=0.0;
var offset=0.0;
var logObj=dom.GetObject(vrp+"HK2-Log");
var loggingObj=dom.GetObject(vrp+"HK2-Logging");

!// Prüfe logging erwartet wird
if ((!log) && loggingObj){
  if (loggingObj.State()!=0){
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
if(SListe!=""){
  !// Log nur, wenn es auch was zu tun gibt
  if(log){logObj.State("Vollständige Raumliste: "+VarNamen);}	
}
SID_SListe=0;
while(true){
  if(SListe.StrValueByIndex(";",SID_SListe)!=""){
    !//Listenelement auslesen und EIN und AUS Zeit den Offsetwert abziehen
    AktSR=SListe.StrValueByIndex(";",SID_SListe);
    HSFlag=SListe.StrValueByIndex(";",SID_SListe+4).ToInteger();
    if((HSFlag!=0)&&(HSFlag!=1)){HSFlag=0;}
    if(HSFlag){
      EIN=SListe.StrValueByIndex(";",SID_SListe+1).ToInteger()-(OffsetEin*60);
      AUS=SListe.StrValueByIndex(";",SID_SListe+2).ToInteger()-(OffsetAus*60);
      SDFlag=SListe.StrValueByIndex(";",SID_SListe+3).ToInteger();
      RTemp=SListe.StrValueByIndex(";",SID_SListe+3);
      if(log){logObj.State("Schaltlisteneintrag " # AktSR # " Heizen: " # EIN.ToTime().Format("%X") # " / " # AUS.ToTime().Format("%X") # "  R-Temp " # RTemp);}
    }else{
      EIN=SListe.StrValueByIndex(";",SID_SListe+1).ToInteger();
      AUS=SListe.StrValueByIndex(";",SID_SListe+2).ToInteger();
      SDFlag=SListe.StrValueByIndex(";",SID_SListe+3).ToInteger();
      RTemp=SListe.StrValueByIndex(";",SID_SListe+3);
      if(log){logObj.State("Schaltlisteneintrag " # AktSR # " Schalten: " # EIN.ToTime().Format("%X") # " - " # AUS.ToTime().Format("%X"));}
    }

    !//Beginn innere Schleife
    !//Zum Listenelement passende Raumvariable suchen
    !// Liste der Raumvariablen durchgehen bis Raumvariable gleich der Variable AktSR ist und Variable RVN (Raumvariablennamen) und RVI (Raumvariable Inhalt) fühlen,
    !//Wenn Variable gefunden innerer Schleife für diesen Durchgang beeenden
    
    SID_RListe=0;
    while (true){
      if(RIDI.StrValueByIndex(";",SID_RListe)!=""){
        if(AktSR==RIDI.StrValueByIndex(";",SID_RListe)){
          RVNListe=VarNamen.StrValueByIndex(";",SID_RListe);
          AktSRName=RIDINamen.StrValueByIndex(";",SID_RListe);
          if(AktSRName==""){
            AktSRName = AktSR;
          }
		      break;
        }else{
          SID_RListe=SID_RListe+1;
		    }
      }else{
        !//Wird keine Raumvariable gefunden wir brechen das Script komplett ab
        WriteLine("ABBRUCH Raumvariable nicht gefunden!");
        if(log){logObj.State(AktSRName # " Raumvariable nicht gefunden, Abbruch Skript !!");}
		    quit;
      }
    }!//while
	
	  !// Wir haben nun einen Raum, oder in der multiRaumVariante eine Raumliste dirch + getrennt.    
    if(multiRaumVariante){
      if(log){logObj.State(AktSRName # " Raumliste: "+RVNListe);}
	    RVNListe=RVNListe.Split("+");
	  }
	
	  !// In der MultiRaumVariante haben wir eine Liste durch + getrennt, sonst nur einen Namen
	  foreach(RVN,RVNListe){
  	  RVI=dom.GetObject(RVN).State();
      if(log){logObj.State(AktSRName # " Raumvariable: "+RVN+"="+RVI);}
				
	    !//Aktoren und Raumtemp setzen, und bestimmen ob wi Heizen oder Schalten
	    if (RVI.StrValueByIndex(";",1)=="H"){
		    HSFlag=1;
	    }else{
		    HSFlag=0;
	    }

	    !// Wir benötigen die Ein und Aussschaltzeit frisch, weil diese hier manipuliert wird.
	    !// Sie wird frisch aus der Schaltliste bezogen!
      if(HSFlag){
        EIN=SListe.StrValueByIndex(";",SID_SListe+1).ToInteger()-(OffsetEin*60);
        AUS=SListe.StrValueByIndex(";",SID_SListe+2).ToInteger()-(OffsetAus*60);
		    RTemp=SListe.StrValueByIndex(";",SID_SListe+3);
	    }else{
        EIN=SListe.StrValueByIndex(";",SID_SListe+1).ToInteger();
        AUS=SListe.StrValueByIndex(";",SID_SListe+2).ToInteger();
		    RTemp=SListe.StrValueByIndex(";",SID_SListe+3);
	    }

      !// Abdere Werte bestimmen
	    AGF=RVI.StrValueByIndex(";",2);
	    EIN=EIN-(RVI.StrValueByIndex(";",4).ToInteger()*60);
	    AUS=AUS-(RVI.StrValueByIndex(";",5).ToInteger()*60);
	    if(log){logObj.State(AktSRName # " berechne Zeiten mit Raum-Zeitversatz "+EIN.ToTime().Format("%X")+" / "+AUS.ToTime().Format("%X"));}
	    if(RTemp.ToInteger()<=1){
	      RTemp=RVI.StrValueByIndex(";",3);
	      if((log)&&(RTemp.ToInteger()==-3)){logObj.State(AktSRName # " Sonderfunktion erkannt \"Gruppen normalisierung\": "+RTemp);}
	      if((log)&&(RTemp.ToInteger()==-1)){logObj.State(AktSRName # " Sonderfunktion erkannt \"Gruppen generell AUS\": "+RTemp);}
	      if((log)&&(RTemp.ToInteger()==-2)){logObj.State(AktSRName # " Sonderfunktion erkannt \"Gruppen generell EIN\": "+RTemp);}
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
        if (offset<0){
          offset = 0;
        }
			
		  
      EIN=EIN-offset.ToInteger()*60;
        
      if(AGF=="RT"){Param="SET_TEMPERATURE";}
      if(AGF=="TC"){Param="SETPOINT";}
      if(AGF=="IP"){Param="SET_POINT_TEMPERATURE";}
      if(AGF=="IT"){Param="SET_TEMPERATURE";}
      if(log){logObj.State(AktSRName # " berechne Startpunkt mit Temp-Zeitversatz "+EIN.ToTime().Format("%X")+" Parameter: "+AGF);}
      }else{
        Param="STATE";
        if(log){logObj.State(AktSRName # "  Parameter: "+AGF);}
      }
      
      
      !//Ausschalten generell
      if((EIN<=NOW)&&(RVI.StrValueByIndex(";",0).ToInteger()!=2)&&(SDFlag==-1)){
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
        if(log){logObj.State(AktSRName # " Schaltstatus setzen "+AktSR+" "+dom.GetObject(RVN).State().StrValueByIndex(";",0));}
      }
      
      !//Einschalten generell
      if((EIN<=NOW)&&(RVI.StrValueByIndex(";",0).ToInteger()!=3)&&(SDFlag==-2)){
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
        if(log){logObj.State(AktSRName # " Schaltstatus setzen "+AktSR+" "+dom.GetObject(RVN).State().StrValueByIndex(";",0));}
      }
      
      !//Rückstellung
      if((EIN<=NOW)&&(RVI.StrValueByIndex(";",0).ToInteger()!=4)&&(SDFlag==-3)){
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
        if(log){logObj.State(AktSRName # " Schaltstatus setzen "+AktSR+" "+dom.GetObject(RVN).State().StrValueByIndex(";",0));}
	    }


	    !//Ausschalten
	    !//Liegt der Ausschaltzeitpunkt des aktuellen Schaltlistenelement in der Vergangenheit dann Raumvariable durchgehen und Aktoren auf Grundtemp bringen WENN Heizung
	    !//noch nicht ausgeschaltet ist.
	    if((SDFlag==0) && (RVI.StrValueByIndex(";",0).ToInteger())){
	      !// Der Ausschaltzeitpunkt wird 160sec in Zukunft und Vergangenheit (320sec) geprüft. Damit wird ein 5min (300sec) Interval abgedeckt.
	      !// Das Skript sollte alle 5m,in laufen.
	      if(((AUS-160)<NOW) && ((AUS+160)>NOW)){
		      foreach(AktAktor,RVI.Split(";")){
		  	    if (AktAktor.Length()>4){
			        if(HSFlag){
		            if(FW1==true){
				          if(dom.GetObject(AktAktor).DPByHssDP(Param).State()==RTemp.ToInteger()){
				            dom.GetObject(AktAktor).DPByHssDP(Param).State(GT);
					          !WriteLine("Schalten aus "+AktAktor+" "+Param+" "+GT.ToString());
					          if(log){logObj.State(AktAktor+" Ausschalten (mit Funktion Reglervorrang) auf Temp: "+GT.ToString()+" Parameter: "+Param);}
				          }else{
					          if(log){logObj.State(AktAktor+" Reglervorrang bei AUS (Regeler händisch verstellt auf: ("+dom.GetObject(AktAktor).DPByHssDP(Param).State().ToString());}
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
		      if(log){logObj.State(AktSRName # " Schaltstatus setzen "+AktSR+" "+dom.GetObject(RVN).State().StrValueByIndex(";",0));}
		    }else{
 	        if(log){
			    !//Ausschalten nicht protoollieren, wenn nicht eingeschaltet
            if (EIN>NOW){
              if (AUS>NOW){
                logObj.State(AktSRName # " Ausschaltpunkt noch nicht erreicht");
              }else{
                logObj.State(AktSRName # " Ausschaltpunkt wurde bereits erreicht");
              }
			      }
          }
		    }
	    }else{
	      if(log){logObj.State(AktSRName # " Gruppe " # RVN # " ist ausgeschaltet");}
	    }

      !//Einschalten
      !//Liegt der Einschaltzeitpunkt in der Vergangenheit UND Ausschaltzeitpunkt in der Zukunft Heizung einschalten WENN diese noch nicht eingeschaltet ist.
      if((SDFlag==0) && (RVI.StrValueByIndex(";",0).ToInteger()==0)){
        if((AT<ATG) || (HSFlag==0)){
          !// Und wir schalten nur ein wenn die Einschaltzeit mehr als 5min beträgt
          if(((EIN+300)<AUS) && (EIN<=NOW) && (AUS>NOW)){
            foreach(AktAktor,RVI.Split(";")){
              if (AktAktor.Length()>4){
                if(HSFlag){
                    if(FW1==1){
                      if(dom.GetObject(AktAktor).DPByHssDP(Param).State()==GT){
                        dom.GetObject(AktAktor).DPByHssDP(Param).State(RTemp);
                        !WriteLine("Schalten ein "+AktAktor+" "+Param+" "+RTemp);
                        if(log){logObj.State(AktAktor+" Einschalten (mit Funktion Reglervorrang) auf Temp: "+RTemp+" Parameter: "+Param);}
                      }else{
                        if(log){logObj.State(AktAktor+" Reglervorrang bei EIN (Regeler händisch verstellt auf: ("+dom.GetObject(AktAktor).DPByHssDP(Param).State().ToString());}
                      }
                    }else{
                      dom.GetObject(AktAktor).DPByHssDP(Param).State(RTemp);
                      !WriteLine("Schalten ein "+AktAktor+" "+Param+" "+RTemp);
                      if(log){logObj.State(AktAktor+" Einschalten (ohne Funktion Reglervorrang) auf Temp: "+RTemp+" Parameter: "+Param);}
                    }
                }else{
                  dom.GetObject(AktAktor).DPByHssDP(Param).State(1);
                  !WriteLine("Schalten ein "+AktAktor+" "+Param);
                  if(log){logObj.State(AktAktor+" Einschalten "+Param);}
                }
              }
            }
            dom.GetObject(RVN).State("1;"+RVI.Substr(2,RVI.Length()-2));
            if(log){logObj.State(AktSRName # " Schaltstatus setzen "+AktSR+" "+dom.GetObject(RVN).State().StrValueByIndex(";",0));}
          }else{
            if(log){
            !// Einschalten nicht protokollieren, wenn bereits ausgeschaltet
              if (AUS>NOW){
                if (EIN>NOW){
                  logObj.State(AktSRName # " Einschaltpunkt noch nicht erreicht");
                }else{
                  logObj.State(AktSRName # " Einschaltpunkt wurde bereits erreicht");
                }
              }
            }
          }
        }else{
          if(log){logObj.State(AktSRName # " Heizen abgebrochen Aussentemperatur " # AT # " größer Grenzwert "+ATG.ToString());}
        }
	    }else{
        !// Sollte eben ausgeschaltet worden sein, loggen wir nicht
        if(((AUS-160)>=NOW) || ((AUS+160)<=NOW)){
	        if(log){logObj.State(AktSRName # " Gruppe " # RVN # " ist eingeschaltet");}
        }
	    }
    }
	
    !// Schleifen ID für Schaltliste weitersetzen
    SID_SListe=SID_SListe+5;
  }else{
    !//Wenn Schaltliste komplett durchgegangen ist äussere Schleife beeenden
    !WriteLine(AT);
	  break;
  }
}

!// Code für Prüfung der Nachschaltung immer um 01:00 Uhr!
if((system.Date("%H%M").ToInteger()>56)&&(system.Date("%H%M").ToInteger()<104)&&(FW2==true)){
  if(log){logObj.State("Beginn Nachtabschaltung");}
  
  !// Wandle die Raumliste um, sodass auch die multiRaumVariante berücksichtigt wird
  RVNListe = VarNamen;
  if(multiRaumVariante){
	  RVNListe=RVNListe.Replace("+",";");
  }
  RVNListe=RVNListe.Split("+");  
  !// Laufe über alle Räume
  foreach(RVN,RVNListe) {    
    if(log){logObj.State("Gruppe:"+RVN);}
    RVI=dom.GetObject(RVN).State();
    AGF=RVI.StrValueByIndex(";",2);
    if(AGF=="RT"){Param="SET_TEMPERATURE";}
    if(AGF=="TC"){Param="SETPOINT";}
    if(AGF=="IP"){Param="SET_POINT_TEMPERATURE";}
    if(AGF=="SW"){Param="STATE";}
    if(AGF=="IT"){Param="SET_TEMPERATURE";}
    if(RVI.StrValueByIndex(";",1)=="H"){
      HSFlag=1;
    }else{
      HSFlag=0;
    }

    !// Heizung oder Schaltung in jedem Fall zurücksetzen, wenn kein Sonderfall wie Dauer-An vorliegt
    !// Wir schalten auch aus, wenn der Raum auf heizen steht. Sollten wir in einem Heizzyklus sein
    !// wird die  nächste EINSCHALTEN Prüfung wieder schalten.
    if(RVI.StrValueByIndex(";",0).ToInteger()<=1){
      foreach(AktAktor,RVI.Split(";")){
        if ((AktAktor.Length()>4)){
          if(HSFlag){
            dom.GetObject(AktAktor).DPByHssDP(Param).State(GT);
            if(log){logObj.State(AktAktor+" Ausschalten auf Temp: "+GT.ToString()+" Parameter: "+Param);}
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
  }
  if(log){logObj.State("Ende Nachtabschaltung");}
}

!// Code für die Prüfung Schaltliste. Ist die Schaltliste leer, prüfen wir alle Räume,
!// die sich sich im Schaltzustand EIN befinden. Diese werden dann zurückgestellt.
if(SListe==""){
  !// Wandle die Raumliste um, sodass auch die multiRaumVariante berücksichtigt wird
  RVNListe = VarNamen;
  if(multiRaumVariante){
	  RVNListe=RVNListe.Replace("+",";");
  }
  RVNListe=RVNListe.Split(";");  
  !// Laufe über alle Räume
  foreach(RVN,RVNListe) {    
    RVI=dom.GetObject(RVN).State();
    AGF=RVI.StrValueByIndex(";",2);
    if(AGF=="RT"){Param="SET_TEMPERATURE";}
    if(AGF=="TC"){Param="SETPOINT";}
    if(AGF=="IP"){Param="SET_POINT_TEMPERATURE";}
    if(AGF=="SW"){Param="STATE";}
    if(AGF=="IT"){Param="SET_TEMPERATURE";}
    if(RVI.StrValueByIndex(";",1)=="H"){
      HSFlag=1;
    }else{
      HSFlag=0;
    }
    
    !// Heizung oder Schaltung in zurücksetzen, wenn diese sich immer noch im Heizen-/Schaltenzustand
    !// befindet. Evtl. wurde ein Ausschaltpunkt versäumt.
    if(RVI.StrValueByIndex(";",0).ToInteger()==1){
      !// Reset der Heizvariablen von 1 auf 0. Da die Heizliste leer ist, sollte der Eintrag
      !// in der Raumliste auch auf 0 stehen.
      dom.GetObject(RVN).State("0;"+RVI.Substr(2,RVI.Length()-2));
      if(log){logObj.State("Schaltliste ist leer! Korrektur der Raumliste auf: "+dom.GetObject(RVN).State());}	   
      !// Aktoren zurücksetzen
      foreach(AktAktor,RVI.Split(";")){
        if ((AktAktor.Length()>4)){
          if(HSFlag){
            dom.GetObject(AktAktor).DPByHssDP(Param).State(GT);
            if(log){logObj.State("Schaltliste ist leer! Heizung - "+AktAktor+" ausschalten auf Temp: "+GT.ToString()+" Parameter: "+Param);}
          }else{
            dom.GetObject(AktAktor).DPByHssDP(Param).State(0);
            if(log){logObj.State("Schaltliste ist leer! Schalter - "+AktAktor+" ausschalten "+Param);}
          }
        }		   
      }
    }
  }
}

if(log){logObj.State("Ende Schaltskriptlauf");}
