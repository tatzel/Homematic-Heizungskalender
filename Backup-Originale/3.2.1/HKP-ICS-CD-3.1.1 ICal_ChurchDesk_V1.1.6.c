!//Heizkalender per iCal Resssource aus Churchdesk Version 1.1.6 / 11.1.2025 Lukas Helduser (Youtube: https://www.youtube.com/LukasvandeHaag)
!//Teil 1 Skript zum auslesen der iCal Daten

!//Eingabe eines Namens Präfix
!//Dies ist nur erforderlich wenn die Namensvorgabe beim erstellen den Systemvariablen geändert wurde.
!//Wird hier ein Präfix eingeben so muss dieser in allen Skripten auch angegeben werden.
string vrp="";


!//Debug ein oder aus
boolean DEBUG=0;
!//Nehmen sie hier keine Veränderungen vor!
!//#######---Ende Variabler Bereich---#############################################################################################################
!//Stript Variablen. Von Benutzer nicht zu verändern !!!!
string data;
string datasort;
string error;
string aktevent;
string aktRaum;
integer frRID=0;
string datatemp;
string nSchaltliste;
string aktionFlag;
string temp;
string start;
string stop;
string cap="0";
!//Raumliste auslesen und Zeit Horizont setzen
string sRaumliste=dom.GetObject(vrp+"HK1-R-Liste").Value().ToUpper();
string sRaumVarListe=dom.GetObject(vrp+"HK2-HKG-Liste").Value();
integer horizon=system.Date().ToTime().ToInteger();



foreach(aktRaum,sRaumliste.Split(";")){
  !//Kopfdaten
  string endurl="wget --timeout=3 -O - 'https://api2.churchdesk.com/ical/resource/"+aktRaum+"/public?organizationId="+dom.GetObject(vrp+"HK1-ICS-CD-Churchdesk-ID").Value()+"'";
  if(DEBUG){WriteLine(endurl+"\n");}
  system.Exec(endurl, &data, &error);

  data=";"+data.Substr(data.Find("BEGIN:VEVENT"),data.Length()-data.Find("BEGIN:VEVENT"));
  data=data.Replace("\r\n",";");
  data=data.Replace("END:VEVENT","$");
  datasort="";

    !//Filter
    integer iTime;
    foreach(aktevent,data.Split("$")){
      if(aktevent.Contains("DTSTART")){
        temp=aktevent.Substr(aktevent.Find("DTSTART")+8,15);
        temp=temp.Substr(0,4)+"-"+temp.Substr(4,2)+"-"+temp.Substr(6,2)+" "+temp.Substr(9,2)+":"+temp.Substr(11,2)+":"+temp.Substr(13,2);
        iTime=(temp.ToTime().ToInteger())+((system.Date("%z").ToInteger()/100)*3600);
         if((iTime>horizon)&&(iTime<horizon+86400)){
              datasort=datasort+aktevent+"$";
         }
      }
    }
    aktevent="";
    datasort=datasort.ToUpper();

    if(DEBUG){
      WriteLine(sRaumliste+"\n");
      WriteLine(sRaumVarListe+"\n");
      WriteLine(error+"\n");
      WriteLine(data+"\n\n");
      WriteLine(datasort+"\n\n");
    }
    !//Eventlisting
    string eventdetail;
    if(datasort.Contains("BEGIN:VEVENT")){
      foreach(aktevent,datasort.Split("$")){
        !//temp=aktevent.Substr(aktevent.Find("SUMMARY")+8,aktevent.Length()-aktevent.Find("SUMMARY"));
        !//temp=temp.Substr(0,temp.Find(";"));
        if(dom.GetObject(dom.GetObject(sRaumVarListe.StrValueByIndex(";",frRID))).Value().StrValueByIndex(";",1)=="S"){aktionFlag="0;";};
        if(dom.GetObject(dom.GetObject(sRaumVarListe.StrValueByIndex(";",frRID))).Value().StrValueByIndex(";",1)=="H"){aktionFlag="1;";};
        foreach(eventdetail,aktevent.Split(";")){
          if(DEBUG){WriteLine("Eventdetail: "+eventdetail);}
          if(eventdetail.Contains("DTSTART")==true){
              start=eventdetail.Substr(8,4)+"-"+eventdetail.Substr(12,2)+"-"+eventdetail.Substr(14,2)+" ";
              start=start+eventdetail.Substr(17,2)+":"+eventdetail.Substr(19,2)+":"+eventdetail.Substr(21,2);
              start=((start.ToTime().ToInteger())+((system.Date("%z").ToInteger()/100)*3600)).ToString();
          }
          if(eventdetail.Contains("DTEND")==true){
              stop=eventdetail.Substr(6,4)+"-"+eventdetail.Substr(10,2)+"-"+eventdetail.Substr(12,2)+" ";
              stop=stop+eventdetail.Substr(15,2)+":"+eventdetail.Substr(17,2)+":"+eventdetail.Substr(19,2);
              stop=((stop.ToTime().ToInteger())+((system.Date("%z").ToInteger()/100)*3600)).ToString();
          }
        }
        nSchaltliste=nSchaltliste+aktRaum+";"+start+";"+stop+";"+cap+";"+aktionFlag;
      }
      cap="0";
    }else{
      if(DEBUG){WriteLine("Keine Termine!");}
    }
 frRID=frRID+1;
 if(DEBUG){WriteLine("------------------------------------------------");}
}


!//Variablen für den zusammebau und Sortierung der neuen Daten
string aliste=dom.GetObject(vrp+"HK1-Schaltliste").State();
string templiste="";
integer whileID=0;


while (true){
 if(aliste.StrValueByIndex(";",whileID)!=""){
   if(aliste.StrValueByIndex(";",whileID+1).ToInteger()<system.Date().ToTime().ToInteger()){
    if((aliste.StrValueByIndex(";",whileID+2).ToInteger()+7200)>system.Date().ToTime().ToInteger()){
        templiste=templiste+aliste.StrValueByIndex(";",whileID)+";"+aliste.StrValueByIndex(";",whileID+1)+
          ";"+aliste.StrValueByIndex(";",whileID+2)+";"+aliste.StrValueByIndex(";",whileID+3)+";"+aliste.StrValueByIndex(";",whileID+4)+";";
     }
   }
  whileID=whileID+5;
 }else{
   break;
 }
}



dom.GetObject(vrp+"HK1-Schaltliste").State(templiste+nSchaltliste);


!//Debugausgaben
if(DEBUG){
WriteLine(sRaumliste+"\n");
WriteLine("Neu: "+nSchaltliste+"\n");
WriteLine("Alt: "+aliste+"\n");
WriteLine("Temp: "+templiste+"\n");
}
