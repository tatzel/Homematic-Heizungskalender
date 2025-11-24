!//Event orientiertes Heizen/Schalten per ChurchTools / Skript 1 / Version 2.8.8 / 02.10.2025
!//von Lukas Helduser (YouTube: https://www.youtube.com/LukasvandeHaag)
!//Teil 1 des Heizkalenders: Skript zum Auslesen der Ressourcen in ChurchTools
!//Die Nutzung ist kostenlos, jedoch bitten wir sie an helmut@diedrichs.de mitzuteilen (hhtps://diedrichs.de)

!//Eingabe eines Namens Präfix
!//Dies ist nur erforderlich wenn die Namensvorgabe beim erstellen den Systemvariablen geändert wurde.
!//Wird hier ein Präfix eingeben so muss dieser in allen Skripten auch angegeben werden.
string vrp="";

!//Debug Ausgaben Ein und Aus schalten. 0 = Aus, 1 = Ein
boolean DEBUG=0;


!//################################################################################################################################################
!//##################-------Stript Variablen und Script Arbeitsteil. Von Benutzer nicht zu verändern !!!!-------##########################################################

string SLA=dom.GetObject(vrp+"HK1-Schaltliste").State();
string token=dom.GetObject(vrp+"HK1-CT-Token").State();
string from=system.Date("%F");
string to=(((from.ToTime().ToInteger())+86400).ToTime().ToString()).Substr(0,10);
integer GMT=(system.Date("%z").Substr(2,1)).ToInteger()*3600;
string Data;
string Feed1;
string Feed2;
string aktID;
string tempID;
string urlID;
string SLN;
string SLT;
string IDL=dom.GetObject(vrp+"HK1-R-Liste").State();
string urlwget ="wget --timeout=3 -O - '";
string urlR="https://"+dom.GetObject(vrp+"HK1-CT-Gemeindename").State()+".church.tools/api/bookings?login_token=";
string urlM="https://"+dom.GetObject(vrp+"HK1-CT-Gemeindename").State()+".church.tools/api/resource/masterdata";
string SRaumVarListe=dom.GetObject(vrp+"HK2-HKG-Liste").Value();
string AktRaum;
string cap;
string start;
string SHFlag;
string end;
string id;
integer frID=0;


system.Exec(urlwget+urlM+"?login_token="+token+"'", &Data, &Feed1);
Data=Data.ToUpper();

Data=Data.Substr(Data.Find("RESOURCES\":"),Data.Length()-Data.Find("RESOURCES\":"));
foreach(aktID,IDL.Split(";")){
        if(Data.Contains("\"ID\":"+aktID)){
        urlID=urlID+"resource_ids%5B%5D="+aktID+"&";
        tempID=tempID+aktID+";";}
        }



system.Exec(urlwget+urlR+token+"&"+urlID+"&from="+from+"&to="+to+"'", &Data, &Feed2)
Data=Data.ToUpper();


if(Data.Contains("COUNT\":0")){
    Data="Abbruch";
}else{

  while(true){
    Data=Data.Substr(Data.Find("BOOKING\":")+12,Data.Length()-7);
    cap=Data.Substr(Data.Find("DESCRIPTION")+14,6);
    WriteLine(cap+"\n");
    WriteLine(Data+"\n");
    if (cap.Contains("#")==true){
      cap=cap.Replace("#","").Replace(",","").Replace("G","").Replace("D","").Replace(".","").Replace("\"","").Replace("V","");
      cap=cap.Replace("D","").Replace("E","").Replace(" ","").Replace("R","").Replace("A","").Replace("C","").Replace("°","").Replace("\r\n","");
      !WriteLine(cap);
      if(cap.Length()==3){cap=cap.Substr(0,2)+".5";}
      if(cap=="US"){cap="-1";}
      if(cap=="IN"){cap="-2";}
      if(cap=="NOML"){cap="-3";}
    }else{cap="0";}



    start=Data.Substr(Data.Find("CALCULATED\":{\"")+26,19).Replace("T"," ");
    start=((start.ToTime().ToInteger())+GMT).ToString()+";";

    end=Data.Substr(Data.Find("CALCULATED\":{\"")+59,19).Replace("T"," ");
    end=((end.ToTime().ToInteger())+GMT).ToString()+";";

    id=Data.Substr(Data.Find("RESOURCE\":{\"ID")+16,4)+";";
    id=id.Replace(",","").Replace("\"","").Replace("N","");

    foreach(AktRaum,IDL.Split(";")){
      if(AktRaum!=""){
        if(AktRaum.ToInteger()==id.ToInteger()){
            if(dom.GetObject(dom.GetObject(SRaumVarListe.StrValueByIndex(";",frID))).Value().StrValueByIndex(";",1)=="S"){SHFlag=";0;";};
            if(dom.GetObject(dom.GetObject(SRaumVarListe.StrValueByIndex(";",frID))).Value().StrValueByIndex(";",1)=="H"){SHFlag=";1;";};
          }
        }
        frID=frID+1;
    }
    frID=0;


    SLN=SLN+id+start+end+cap+SHFlag;
    if(Data.Contains("BOOKING\":")==false){break;}
  }


  integer whileID=0;
  while (true){
    if(SLA.StrValueByIndex(";",whileID)!=""){
      !WriteLine("???? "+SLA.StrValueByIndex(";",whileID+2)+"  "+system.Date().ToTime().ToInteger().ToString()+"  "+SLA.StrValueByIndex(";",whileID+1).ToInteger().ToString());
      if((SLA.StrValueByIndex(";",whileID+2).ToInteger()+7200)>system.Date().ToTime().ToInteger()){
        if(SLA.StrValueByIndex(";",whileID+1).ToInteger()<system.Date().ToTime().ToInteger()){
          SLT=SLT+SLA.StrValueByIndex(";",whileID)+";"+SLA.StrValueByIndex(";",whileID+1)+";"+SLA.StrValueByIndex(";",whileID+2)+";";
          SLT=SLT+SLA.StrValueByIndex(";",whileID+3)+";"+SLA.StrValueByIndex(";",whileID+4)+";";
       }
      }
     whileID=whileID+5;
    }else{
      break;
    }
  }



  whileID=0;
  while (true){
    if(SLN.StrValueByIndex(";",whileID)!=""){
      if(SLN.StrValueByIndex(";",whileID+1).ToInteger()>(system.Date().ToTime().ToInteger())-180){
        if(SLN.StrValueByIndex(";",whileID+1).ToInteger()<(system.Date().ToTime().ToInteger())+42200){
          SLT=SLT+SLN.StrValueByIndex(";",whileID)+";"+SLN.StrValueByIndex(";",whileID+1)+";"+SLN.StrValueByIndex(";",whileID+2)+";";
          SLT=SLT+SLN.StrValueByIndex(";",whileID+3)+";"+SLN.StrValueByIndex(";",whileID+4)+";";
       }
      }
     whileID=whileID+5;
    }else{
      break;
    }
  }

dom.GetObject(vrp+"HK1-Schaltliste").State(SLT);

}

!//Debugausgaben
if(DEBUG){
WriteLine("DEBUG########");
WriteLine("IDL: " + IDL);
WriteLine("tempID: " + tempID);
WriteLine("###########");
WriteLine("Data: " + Data+"\n");
WriteLine("Error/Feed1: " + Feed1+"\n");
WriteLine("Error/Feed2: " + Feed2+"\n");
WriteLine("  ---  ");
WriteLine("SLN: " +SLN);
WriteLine("SLT: " +SLT);
WriteLine("SLA: " + SLA);
WriteLine("SLT: " + SLT);
}
