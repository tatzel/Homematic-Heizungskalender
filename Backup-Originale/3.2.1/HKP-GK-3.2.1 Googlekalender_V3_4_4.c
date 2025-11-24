!//Heizkalender per Google Kalender Version 3.4.4 / 02.10.2025 Lukas Helduser (Youtube: https://www.youtube.com/LukasvandeHaag)
!//Teil 1 Skript zum auslesen des Google Kalender

!//Eingabe eines Namens Präfix
!//Dies ist nur erforderlich wenn die Namensvorgabe beim erstellen den Systemvariablen geändert wurde.
!//Wird hier ein Präfix eingeben so muss dieser in allen Skripten auch angegeben werden.
string vrp="";

!//Debug Ausgaben Ein und Aus schalten. 0 = Aus, 1 = Ein
boolean DEBUG=0;

!//Nehmen sie hier keine Veränderungen vor!
!//#######---Ende Variabler Bereich---#############################################################################################################
!//Stript Variablen. Von Benutzer nicht zu verändern !!!!
string url1 ="https://www.googleapis.com/calendar/v3/calendars/";
string urlwget ="wget --timeout=3 -O - '";
string url;
string TimemaxT2="%3A00%3A00-00%3A00";
string Data;
string DataTemp;
string error="non";
integer EventPosID=0;
string SRaumliste;
string SRaumVarListe;
string NSchaltliste;
string AktRaum;
string start;
string stop;
string EventID;
integer frID=0;
string AktionFlag;
string cap;


!//AUfbau der gesamt URL
string globaldate=system.Date();
string to=(((globaldate.ToTime().ToInteger())+25200).ToTime().ToString().Substr(0,10)+"T"+((globaldate.ToTime().ToInteger())+25200).ToTime().ToString().Substr(11,8)+"Z");
string from=(((globaldate.ToTime().ToInteger())).ToTime().ToString().Substr(0,10)+"T"+((globaldate.ToTime().ToInteger())).ToTime().ToString().Substr(11,8)+"Z");
url=urlwget+url1+dom.GetObject(vrp+"HK1-GK-Kalender-ID").Value()+"@group.calendar.google.com/events?orderBy=startTime&singleEvents=true&timeMax="+to;
url=url+"&timeMin="+from+"&key="+dom.GetObject(vrp+"HK1-GK-API-Key").Value()+"'";

!//URL abfragen
system.Exec(url,&Data,&error);
Data=Data.ToUpper();

!//Raumliste auslesen
SRaumliste=dom.GetObject(vrp+"HK1-R-Liste").Value().ToUpper();
SRaumVarListe=dom.GetObject(vrp+"HK2-HKG-Liste").Value();

if(Data.Find("CALENDAR#EVENT")>-1){


  !//DIe Raumliste druchgehen und Prüfen ob für den jeweiligen Raum ein Termin vorliegt.
  !//Liegt ein Termin vor dann die Daten des Termin aufbereiten und auf die NEUE Schaltliste setzen.
  foreach(AktRaum,SRaumliste.Split(";")){
    DataTemp=Data;
    if(AktRaum!=""){
      EventPosID=DataTemp.Find(AktRaum);
      if(EventPosID>-1){
       if(dom.GetObject(dom.GetObject(SRaumVarListe.StrValueByIndex(";",frID))).Value().StrValueByIndex(";",1)=="S"){AktionFlag="0;";};
       if(dom.GetObject(dom.GetObject(SRaumVarListe.StrValueByIndex(";",frID))).Value().StrValueByIndex(";",1)=="H"){AktionFlag="1;";};
       DataTemp=DataTemp.Substr(EventPosID,DataTemp.Length()-EventPosID);

       cap=DataTemp.Substr(DataTemp.Find("DESCRIP")+15,6);
       if (cap.Contains("#")==true){
          cap=cap.Replace("#","").Replace(",","").Replace("G","").Replace("D","").Replace(".","").Replace("\"","").Replace("V","");
          cap=cap.Replace("D","").Replace("E","").Replace(" ","").Replace("R","").Replace("A","").Replace("C","").Replace("°","").Replace("\r\n","");
          if(cap.Length()==3){cap=cap.Substr(0,2)+".5;";}
          if(cap=="US"){cap="-1;";}
          if(cap=="IN"){cap="-2;";}
          if(cap=="NOML"){cap="-3;";}
        }else{
          cap="0;";
        }


       start=DataTemp.Substr(DataTemp.Find("START\":")+27,19);
       start=start.Replace("T"," ");
       stop=DataTemp.Substr(DataTemp.Find("END\":")+25,19);
       stop=stop.Replace("T"," ");
       NSchaltliste=NSchaltliste+AktRaum+";"+(start.ToTime().ToInteger().ToString())+";"+(stop.ToTime().ToInteger().ToString())+";"+cap+AktionFlag;
       }
    }
    frID=frID+1;
  }
  frID=0;
}


!//Variablen für den zusammebau und Sortierung der neuen Daten
string Aliste=dom.GetObject(vrp+"HK1-Schaltliste").State();
string Templiste="";
integer whileID=0;

!//Die ALTE Schaltliste durch gehen und prüfen ob veraltetet Termine enthalten sind. Noch nötige Termine zwischen Speichern.
!//Ein Termin ist veraltet wenn die Ausschaltzeit in der Vergangenheit liegt.
!//Termine die in der Zukunft liegen und von Google nicht mehr geliefert werden aus der alten Liste übernehmen.
while (true){
   if(Aliste.StrValueByIndex(";",whileID)!=""){
     if(Aliste.StrValueByIndex(";",whileID+1).ToInteger()<system.Date().ToTime().ToInteger()){
      if((Aliste.StrValueByIndex(";",whileID+2).ToInteger()+7200)>system.Date().ToTime().ToInteger()){
       if(NSchaltliste.Find(Aliste.StrValueByIndex(";",whileID)+";"+Aliste.StrValueByIndex(";",whileID+1)+";"+Aliste.StrValueByIndex(";",whileID+2))==-1){
          Templiste=Templiste+Aliste.StrValueByIndex(";",whileID)+";"+Aliste.StrValueByIndex(";",whileID+1)+
            ";"+Aliste.StrValueByIndex(";",whileID+2)+";"+Aliste.StrValueByIndex(";",whileID+3)+";"+Aliste.StrValueByIndex(";",whileID+4)+";";
       }
      }
     }
     whileID=whileID+5;
    }else{
      break;
   }
}



dom.GetObject(vrp+"HK1-Schaltliste").State(Templiste+NSchaltliste);


!Debugausgaben
if(DEBUG){
WriteLine(SRaumliste+"\n");
WriteLine(NSchaltliste+"\n");
WriteLine(whileID);
WriteLine("\n"+EventID);
WriteLine("\n"+url+"\n");
WriteLine(error+"\n");
WriteLine(Data);
}
