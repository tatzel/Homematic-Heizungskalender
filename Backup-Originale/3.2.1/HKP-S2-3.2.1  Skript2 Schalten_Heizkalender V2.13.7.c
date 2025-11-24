!//Schaltskript für Heizkalender Version 2.13.7 02.10.2025 Lukas Helduser (Youtube: https://www.youtube.com/LukasvandeHaag)
!//Die Nutzung ist kostenlos, aber wir bitten sie zu melden an: helmut@diedrichs.de
!//Teil 2 Skript zum Schalten der Aktoren.

!//Eingabe eines Namens Präfix
!//Dies ist nur erforderlich wenn die Namensvorgabe beim erstellen den Systemvariablen geändert wurde.
!//Wird hier ein Präfix eingeben so muss dieser in allen Skripten auch angegeben werden.
string vrp="";


!//Logging in "Log" mit 1 einschalten oder mit 0 Ausschalten
boolean log=0;
!//Pfadangabe zur Logdatei MIT (!) Pfad, Dateinamen und Endung angeben.
!//Beispiel: /media/usb1/HK-Logging.log
string pfad="";


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
boolean offsetflag;
integer T=0;
string AGF;
string stdout;
string stderr;
string RTemp;
integer SDFlag;
string Param;

!//Beginn aussere Schleife
!//Element von der Schaltliste nehmen und die dazu gehörige Raumvariablen suchen
!//Wird die Raumvariable nicht gefunden wird das Skript an der Stelle abgebrochen da ein Fehler in der Konfiguration vorliegt
if(log){system.Exec("echo "+system.Date("%Y.%m.%d %T")+" Beginn Schaltskriptlauf >> "+pfad,&stdout,&stdeer);}
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
      if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktSR +"(Heizen) Ein: "+EIN.ToString()+" Aus: "+AUS.ToString()+"  R-Temp "+RTemp+ "' >> "+pfad,&stdout,&stdeer);}
      }else{
      EIN=SListe.StrValueByIndex(";",SID_A+1).ToInteger();
      AUS=SListe.StrValueByIndex(";",SID_A+2).ToInteger();
      SDFlag=SListe.StrValueByIndex(";",SID_A+3).ToInteger();
      RTemp=SListe.StrValueByIndex(";",SID_A+3);
      if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktSR +"(Schalten) Ein: "+EIN.ToString()+" Aus: "+AUS.ToString()+"'  >> "+pfad,&stdout,&stdeer);}
      }

  !//Beginn innere Schleife
     !//Zum Listenelement passende Raumvariable suchen
     !// Liste der Raumvariablen durchgehen bis Raumvariable gleich der Variable AktSR ist und Variable RVN (Raumvariablennamen) und RVI (Raumvariable Inhalt) fühlen,
     !//Wenn Variable gefunden innerer Schleife für diesen Durchgang beeenden
      while (flagB){
          if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" Gruppenvariable suchen' >> "+pfad,&stdout,&stdeer);}
          if(RIDI.StrValueByIndex(";",SID_B)!=""){
            if(AktSR==RIDI.StrValueByIndex(";",SID_B)){
              RVN=VarNamen.StrValueByIndex(";",SID_B);
              RVI=dom.GetObject(RVN).State();
              if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktSR +" Gruppenvariable: "+RVN+" Inhalt: "+RVI+"' >> "+pfad,&stdout,&stdeer);}
              flagB=0;
              }
          SID_B=SID_B+1;
          }else{
            !//Wird keine Raumvariable gefunden innerer und äussere Schleife abbrechen
            flagB=0;
            flagA=0;
            WriteLine("ABBRUCH Raumvariable nicht gefunden!");
            if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktSR +" Gruppenvariable nicht gefunden, Abbruch Skript !!' >> "+pfad,&stdout,&stdeer);}
          }
      }!//while
      if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktSR +" Ende Gruppenvariablen Suche' >> "+pfad,&stdout,&stdeer);}
    !//innere Schleife für nächsten Durchgang in Grundstellung bringen
     SID_B=0;
     flagB=1;

   !//Aktoren und Raumtemp setzen
   AGF=RVI.StrValueByIndex(";",2);
   EIN=EIN-(RVI.StrValueByIndex(";",4).ToInteger()*60);
   AUS=AUS-(RVI.StrValueByIndex(";",5).ToInteger()*60);
   if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktSR +" berechne Ein/Auszeit mit Raum-Zeitversatz "+EIN.ToString()+"/ "+AUS.ToString()+"' >> "+pfad,&stdout,&stdeer);}
   if(RTemp.ToInteger()<=1){
     RTemp=RVI.StrValueByIndex(";",3);
     if((log)&&(RTemp.ToInteger()==-3)){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktSR +" Sonderfunktion erkannt \"Gruppen normalisierung\": "+RTemp+"' >> "+pfad,&stdout,&stdeer);}
     if((log)&&(RTemp.ToInteger()==-1)){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktSR +" Sonderfunktion erkannt \"Gruppen generell AUS\": "+RTemp+"' >> "+pfad,&stdout,&stdeer);}
     if((log)&&(RTemp.ToInteger()==-2)){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktSR +" Sonderfunktion erkannt \"Gruppen generell EIN\": "+RTemp+"' >> "+pfad,&stdout,&stdeer);}
   }

  !//Nach aktueller Aussentemperatur den Offsetwert berechnen und am Einschaltzeitpunkt abziehen und Aktor Paramter setzen
  if (HSFlag==1){
  offsetflag = 1;
  if(AT<-10){EIN=EIN-(OffsetAT.StrValueByIndex(";",0).ToInteger()*60);offsetflag=0;}
  if((AT<-5)&&(offsetflag)){EIN=EIN-(OffsetAT.StrValueByIndex(";",1).ToInteger()*60);offsetflag=0;}
  if((AT<0)&&(offsetflag)){EIN=EIN-(OffsetAT.StrValueByIndex(";",2).ToInteger()*60);offsetflag=0;}
  if((AT<8)&&(offsetflag)){EIN=EIN-(OffsetAT.StrValueByIndex(";",3).ToInteger()*60);offsetflag=0;}
  if((AT<10)&&(offsetflag)){EIN=EIN-(OffsetAT.StrValueByIndex(";",4).ToInteger()*60);offsetflag=0;}
  if((AT<12)&&(offsetflag)){EIN=EIN-(OffsetAT.StrValueByIndex(";",5).ToInteger()*60);offsetflag=0;}
  if((AT<15)&&(offsetflag)){EIN=EIN-(OffsetAT.StrValueByIndex(";",6).ToInteger()*60);offsetflag=0;}
  if((AT<17.5)&&(offsetflag)){EIN=EIN-(OffsetAT.StrValueByIndex(";",7).ToInteger()*60);offsetflag=0;}

  if(AGF=="RT"){Param="SET_TEMPERATURE";}
  if(AGF=="TC"){Param="SETPOINT";}
  if(AGF=="IP"){Param="SET_POINT_TEMPERATURE";}
  if(AGF=="IT"){Param="SET_TEMPERATURE";}
  if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktSR +" berechne Startpunkt mit Temp-Zeitversatz "+EIN.ToString()+" Parameter: "+AGF+"' >> "+pfad,&stdout,&stdeer);}
  }else{
    Param="STATE";
    if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktSR +"  Parameter: "+AGF+"' >> "+pfad,&stdout,&stdeer);}
  }


  !//Ausschalten generell
  if((EIN<=system.Date("%s").ToInteger())&&(RVI.StrValueByIndex(";",0).ToInteger()!=2)&&(SDFlag==-1)){
    foreach(AktAktor,RVI.Split(";")){
        if (AktAktor.Length()>4){
          if(HSFlag){
                dom.GetObject(AktAktor).DPByHssDP(Param).State(GT);
                if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktAktor+" dauerhaft ausgeschaltet auf Temp: "+GT.ToString()+"' >> "+pfad,&stdout,&stdeer);}
          }else{
                dom.GetObject(AktAktor).DPByHssDP(Param).State(0);
                if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktAktor+" dauerhaft ausgeschaltet' >> "+pfad,&stdout,&stdeer);}
          }
        }
      }
     dom.GetObject(RVN).State("2;"+RVI.Substr(2,RVI.Length()-2));
     if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" Einschaltenmerker "+AktSR+" "+dom.GetObject(RVN).State().StrValueByIndex(";",0)+"' >> "+pfad,&stdout,&stdeer);}
    }

  !//Einschalten generell
  if((EIN<=system.Date("%s").ToInteger())&&(RVI.StrValueByIndex(";",0).ToInteger()!=3)&&(SDFlag==-2)){
      foreach(AktAktor,RVI.Split(";")){
        if (AktAktor.Length()>4){
          if(HSFlag){
              dom.GetObject(AktAktor).DPByHssDP(Param).State(RTemp);
              if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktAktor +" dauerhaft eingeschaltet auf Temp: "+RTemp+"' >> "+pfad,&stdout,&stdeer);}
          }else{
              dom.GetObject(AktAktor).DPByHssDP(Param).State(1);
              if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktAktor +" dauerhaft eingeschaltet' >> "+pfad,&stdout,&stdeer);}
          }
        }
      }
    dom.GetObject(RVN).State("3;"+RVI.Substr(2,RVI.Length()-2));
    if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" Einschaltenmerker +"+AktSR+" "+dom.GetObject(RVN).State().StrValueByIndex(";",0)+"' >> "+pfad,&stdout,&stdeer);}
  }

  !//Rückstellung
  if((EIN<=system.Date("%s").ToInteger())&&(RVI.StrValueByIndex(";",0).ToInteger()!=4)&&(SDFlag==-3)){
      foreach(AktAktor,RVI.Split(";")){
        if (AktAktor.Length()>4){
          if(HSFlag){
                dom.GetObject(AktAktor).DPByHssDP(Param).State(GT);
                if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktAktor +" Rückstellung aus dauerhaft auf Temp: "+GT.ToString()+"' >> "+pfad,&stdout,&stdeer);}
          }else{
                dom.GetObject(AktAktor).DPByHssDP(Param).State(0);
                if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktAktor +" Rückstellung aus dauerhaft'  >> "+pfad,&stdout,&stdeer);}
          }
        }
      }
      dom.GetObject(RVN).State("0;"+RVI.Substr(2,RVI.Length()-2));
      if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" Einschaltenmerker +"+AktSR+" "+dom.GetObject(RVN).State().StrValueByIndex(";",0)+"' >> "+pfad,&stdout,&stdeer);}
      EIN=EIN+50000;
  }



  !//Ausschalten
  !//Liegt der Ausschaltzeitpunkt des aktuellen Schaltlistenelement in der Vergangenheit dann Raumvariable durchgehen und Aktoren auf Grundtemp bringen WENN Heizung
  !//noch nicht ausgeschaltet ist
  if(((AUS-10)<system.Date("%s").ToInteger())&&(RVI.StrValueByIndex(";",0).ToInteger())&&((AUS+180)>system.Date("%s").ToInteger())==1){
      foreach(AktAktor,RVI.Split(";")){
          if (AktAktor.Length()>4){
            if(HSFlag){
              if(FW1==true){
                    if(dom.GetObject(AktAktor).DPByHssDP(Param).State()==RTemp.ToInteger()){
                      dom.GetObject(AktAktor).DPByHssDP(Param).State(GT);
                       !WriteLine("Schalten aus "+AktAktor+" "+Param+" "+GT.ToString());
                       if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktAktor+" Ausschalten (mit Funktion Reglervorrang) auf Temp: "+GT.ToString()+" Parameter: "+Param+"' >> "+pfad,&stdout,&stdeer);}
                     }else{
                       if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktAktor+" Reglervorrang bei AUS (Regeler Haendisch verstellt auf: ("+dom.GetObject(AktAktor).DPByHssDP(Param).State().ToString()+")' >> "+pfad,&stdout,&stdeer);}
                     }
                }else{
                    dom.GetObject(AktAktor).DPByHssDP(Param).State(GT);
                     !WriteLine("Schalten aus "+AktAktor+" "+Param+" "+GT.ToString());
                      if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktAktor+" Ausschalten (ohne Funktion Reglervorrang) auf Temp: "+GT.ToString()+" Parameter: "+Param+"' >> "+pfad,&stdout,&stdeer);}
                    }
              }else{
              dom.GetObject(AktAktor).DPByHssDP(Param).State(0);
               !WriteLine("Schalten aus "+AktAktor+" "+Param);
              if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktAktor+" Ausschalten "+Param+"' >> "+pfad,&stdout,&stdeer);}
            }
          }
        }
       dom.GetObject(RVN).State("0;"+RVI.Substr(2,RVI.Length()-2));
       if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" Einschaltenmerker "+AktSR+" "+dom.GetObject(RVN).State().StrValueByIndex(";",0)+"' >> "+pfad,&stdout,&stdeer);}
      }else{
        if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktSR+" Ausschaltpunkt nicht erreicht oder Gruppen schon geschaltet' >> "+pfad,&stdout,&stdeer);}
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
                       if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktAktor+" Einschalten (mit Funktion Reglervorrang) auf Temp: "+RTemp+" Parameter: "+Param+"' >> "+pfad,&stdout,&stdeer);}
                    }else{
                        if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktAktor+" Reglervorrang bei EIN (Regeler Haendisch verstellt auf: ("+dom.GetObject(AktAktor).DPByHssDP(Param).State().ToString()+")' >> "+pfad,&stdout,&stdeer);}
                        }
                 }else{
                      dom.GetObject(AktAktor).DPByHssDP(Param).State(RTemp);
                       !WriteLine("Schalten ein "+AktAktor+" "+Param+" "+RTemp);
                         if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktAktor+" Einschalten (ohne Funktion Reglervorrang) auf Temp: "+RTemp+" Parameter: "+Param+"' >> "+pfad,&stdout,&stdeer);}
                        }
              }else{
                if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktAktor+" Heizen abgebrochen Aussentemperatur größer Grenzwert "+AT.ToString()+" ' >> "+pfad,&stdout,&stdeer);}
              }
            }else{
              dom.GetObject(AktAktor).DPByHssDP(Param).State(1);
               !WriteLine("Schalten ein "+AktAktor+" "+Param);
               if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktAktor+" Einschalten "+Param+"' >> "+pfad,&stdout,&stdeer);}
            }
          }
        }
      dom.GetObject(RVN).State("1;"+RVI.Substr(2,RVI.Length()-2));
      if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" Einschaltenmerker "+AktSR+" "+dom.GetObject(RVN).State().StrValueByIndex(";",0)+"' >> "+pfad,&stdout,&stdeer);}
    }else{
      if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktSR+" Einschaltpunkt nicht erreicht oder Gruppen schon geschaltet' >> "+pfad,&stdout,&stdeer);}
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

if((system.Date("%H%M").ToInteger()>100)&&(system.Date("%H%M").ToInteger()<108)&&(FW2==true)){
if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" Beginn Nachtabschaltung' >> "+pfad,&stdout,&stdeer);}
  while(flagC){
     if(VarNamen.StrValueByIndex(";",SID_C)!=""){
       RVN=VarNamen.StrValueByIndex(";",SID_C);
       RVI=dom.GetObject(RVN).State();
       AGF=RVI.StrValueByIndex(";",2);
       if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" Gruppe:"+RVN+"' >> "+pfad,&stdout,&stdeer);}
       if(AGF=="RT"){Param="SET_TEMPERATURE";}
       if(AGF=="TC"){Param="SETPOINT";}
       if(AGF=="IP"){Param="SET_POINT_TEMPERATURE";}
       if(AGF=="SW"){Param="STATE";}
       if(AGF=="IT"){Param="SET_TEMPERATURE";}
       if(RVI.StrValueByIndex(";",1)=="H"){HSFlag=1;}
       if(RVI.StrValueByIndex(";",1)=="S"){HSFlag=0;}
       if(RVI.StrValueByIndex(";",0).ToInteger()==0){
         foreach(AktAktor,RVI.Split(";")){
              if ((AktAktor.Length()>4)&&((RVI.StrValueByIndex(";",0).ToInteger())>=0)){
                if(HSFlag){
                  dom.GetObject(AktAktor).DPByHssDP(Param).State(GT);
                  if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktAktor+" Ausschalten auf Temp: "+GT.ToString()+"Parameter: "+Param+"' >> "+pfad,&stdout,&stdeer);}
                }else{
                  dom.GetObject(AktAktor).DPByHssDP(Param).State(0);
                  if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" "+AktAktor+" Ausschalten "+Param+"' >> "+pfad,&stdout,&stdeer);}
                }
              }else{
                if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" Gruppe:"+RVN+" ausgelassen (Sonderstellung)' >> "+pfad,&stdout,&stdeer);}
              }
         }
       }
     }else{
       flagC=0;
       if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" Ende Nachtabschaltung' >> "+pfad,&stdout,&stdeer);}
     }
    SID_C=SID_C+1;
  }
}
if(log){system.Exec("echo '"+system.Date("%Y.%m.%d %T")+" Ende Schaltskriptlauf' >> "+pfad,&stdout,&stdeer);}
