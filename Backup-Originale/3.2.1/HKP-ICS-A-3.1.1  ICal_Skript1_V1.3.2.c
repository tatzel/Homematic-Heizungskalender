!//Heizkalender per iCal Version 1.3.2 / 13.10.2025 Lukas Helduser (Youtube: https://www.youtube.com/LukasvandeHaag)
!//Teil 1 Skript zum auslesen der iCal Daten

!//Eingabe eines Namens Präfix
!//Dies ist nur erforderlich wenn die Namensvorgabe beim erstellen den Systemvariablen geändert wurde.
!//Wird hier ein Präfix eingeben so muss dieser in allen Skripten auch angegeben werden.
string vrp="";



!//Nehmen sie hier keine Veränderungen vor!
!//#######---Ende Variabler Bereich---#############################################################################################################
!//Stript Variablen. Von Benutzer nicht zu verändern !!!!
string data;
string datasort;
string error;
integer horizon;
string globaldate=system.Date();
string aktevent;
string sRaumliste;
string sRaumVarListe;
string aktRaum;
integer frRID=0;
string datatemp;
string nSchaltliste;
string aktionFlag;
string temp;
string start;
string stop;
string cap="0";

!//Debug ein oder aus
boolean DEBUG=0;


!//Kopfdaten
string endurl="wget --timeout=3 -O - '"+dom.GetObject(vrp+"HK1-ICS-Url").Value()+"'";

horizon=system.Date().ToTime().ToInteger();


system.Exec(endurl, &data, &error);

data=";"+data.Substr(data.Find("BEGIN:VEVENT"),data.Length()-data.Find("BEGIN:VEVENT"));
data=data.Replace("\r\n",";");
data=data.Replace("END:VEVENT","$");

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


  !//Raumliste auslesen
  sRaumliste=dom.GetObject(vrp+"HK1-R-Liste").Value().ToUpper();
  sRaumVarListe=dom.GetObject(vrp+"HK2-HKG-Liste").Value();

  if(DEBUG){
    WriteLine(sRaumliste);
    WriteLine(sRaumVarListe);
  }
  string eventdetail;
  if(datasort.Contains("BEGIN:VEVENT")){
      foreach(aktRaum,sRaumliste.Split(";")){
          if(aktRaum!=""){
              foreach(aktevent,datasort.Split("$")){
                temp=aktevent.Substr(aktevent.Find("SUMMARY")+8,aktevent.Length()-aktevent.Find("SUMMARY"));
                temp=temp.StrValueByIndex(";",1);
                if(temp.Contains(aktevent)){
                  if(dom.GetObject(dom.GetObject(sRaumVarListe.StrValueByIndex(";",frRID))).Value().StrValueByIndex(";",1)=="S"){aktionFlag="0;";};
                  if(dom.GetObject(dom.GetObject(sRaumVarListe.StrValueByIndex(";",frRID))).Value().StrValueByIndex(";",1)=="H"){aktionFlag="1;";};
                  foreach(eventdetail,aktevent.Split(";")){
                    if(DEBUG){WriteLine(eventdetail);}
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
                    if(eventdetail.Contains("DESCRIPTION")==true){
                        cap=eventdetail.Substr(12,eventdetail.Length()-12);
                          if (cap.Contains("#")==true){
                            cap=cap.Replace("#","").Replace(",","").Replace("G","").Replace("D","").Replace(".","").Replace("\"","").Replace("V","");
                            cap=cap.Replace("D","").Replace("E","").Replace(" ","").Replace("R","").Replace("A","").Replace("C","").Replace("°","");
                            if(cap.Length()==3){cap=cap.Substr(0,2)+".5";}
                            if(cap=="US"){cap="-1";}
                            if(cap=="IN"){cap="-2";}
                            if(cap=="NOML"){cap="-3";}
                          }
                     }
                  }
                  nSchaltliste=nSchaltliste+aktRaum+";"+start+";"+stop+";"+cap+";"+aktionFlag;
                  cap="0";
                }
              }

           }
       frRID=frRID+1;
      }
      frRID=0;
   }else{
    if(DEBUG){WriteLine("Keine Termine!");}
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
WriteLine("\n"+endurl+"\n");
WriteLine(error+"\n");
WriteLine(data+"\n\n");
WriteLine(datasort+"\n\n");
}
