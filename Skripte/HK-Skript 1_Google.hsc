!// UNGETESTET!!! Skript 1 um die Termine aus Google auszulesen
!//================================================================================================
!// Stand:    23.01.2026
!// Autoren:  Lukas Helduser    (Youtube: https://www.youtube.com/LukasvandeHaag)
!//           Martin Richter    (heizkalender@m-ri.de) http://blog.m-ri.de/
!// Projekt:  Helmut Diedrichs  (helmut@diedrichs.de) https://diedrichs.de
!//================================================================================================
!// Der Heizkalender ist eine Idee von Helmut W. Diedrichs und wurde erstmals 2019 in der 
!// Stadtmission Arheilgen angewendet Lukas Helduser entwickelte 2023 auf der Bais von Homematic 
!// das Heizkalender-Programm für die Allgemeinheit, inkl, Varianten. 
!// Dank an die seitherigen Anwender für ihre Verbesserungsvorschläge, insbesondere an die Pilot-
!// Gemeinden. Dieser Code wurde im Rahmen der Heizkalender-Implementierung der Baptisten Gemeinde 
!// Hanau von Martin Richter optimiert.
!// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
!// Das Heizkalender-Team freut sich, dass Sie den kostenlosen Heizkalender anwenden und somit einen 
!// Beitrag zum Umweltschutz leisten. Es wäre schön, wenn Sie die Nutzung per E-Mail anzeigen an:
!// >>>>> info@heizkalender.de <<<<<
!// Dadurch ergäbe ich eine Übersicht und die Möglichkeit auf Änderungen hinzuweisen. Bitte 
!// berichten auch Sie über Ihre Erfahrung mit dem Heizkalender.
!//================================================================================================
!//
!// Der Code basiert in großen Teilen auf der Datei: 
!//  HKP-GK-3.2.1 Googlekalender_V3_4_4.c
!// Der ursprüngliche Code wurde geschrieben von:
!//   Lukas Helduser (Youtube: https://www.youtube.com/LukasvandeHaag) 
!// Ich (MRi) habe diesen Code dann erweitert, korrigiert und verbessert um sie an die Nutzung in
!// meiner Gemeinde anzupassen.
!//
!// Skript sollte alle 30min laufen
!//

!// MRi: 2025-11-24	MultiRaumVariante, damit lassen sich mehrere Räume einer Ressource zuordnen.
!//					        korrektur nochmal für doppelte Schaltlisteneinträge

!//Eingabe eines Namens Präfix
!//Dies ist nur erforderlich wenn die Namensvorgabe beim erstellen den Systemvariablen geändert wurde.
!//Wird hier ein Präfix eingeben so muss dieser in allen Skripten auch angegeben werden.
string vrp="";

!//Debug ein oder aus
boolean DEBUG=0;

!//Multiraum Variante, dies unterstützt eine Raumliste in der mehrere Räume mit einem + gemeinsm geschaltet werden können.
boolean multiRaumVariante=true;

!// Logging in "Log" mit 1 zwingend einschalten oder mit -1 zwingend Ausschalten
!// Mit 0 wird die Einstellunge aus der HKx-Logging übernommen
integer log=0;

!// Zeitfenster in dem nach Termine geschaut wird 
!// minus zeitNachlauf în Minuten (min = eingestellte Nachlaufzeit), 
!// plus zeitVorlauf (min = maximale Vorlaufzeit)
integer zeitVorlauf=8*60;		!// 8 Stunden (default=12h)
integer zeitNachlauf=30;		!// 30min Stunden (default = 120min)


!//#######---Ende Variabler Bereich---#############################################################################################################
!//Stript Variablen. Von Benutzer nicht zu verändern !!!!
string url;
string TimemaxT2="%3A00%3A00-00%3A00";
string data;
string dataTemp;
string error="non";
integer EventPosID=0;
string sRaumliste;
string sRaumVarListe;
string sRaumVar;
string SLA=dom.GetObject(vrp+"HK1-Schaltliste").State();
string SLN;
string SLT;
string AktRaum;
string start;
string stop;
string EventID;
integer frRID=0;
string SHFlag;
string cap;
string toadd;

!// Logging vorbereiten
var logObj=dom.GetObject(vrp+"HK1-Log");
var loggingObj=dom.GetObject(vrp+"HK1-Logging");

!// Prüfe ob logging erwartet wird
if (log<0) {
  !// Zwingend kein logging
  log = false;
} elseif (log==0) {
!// Einstellung der Logging Variable prüfen
  if (loggingObj && loggingObj.State()!=0){
    log = true;
  }
} else {
  !// Logging einschalten
  log = true;
}

!// Logging zwinged auschalten, wenn keine Variable vorhanden ist
if (!logObj){
  log = false;
}

if(log){logObj.State("Beginn Google-Skriptlauf");}

!//Aufbau der gesamt URL
string globaldate=system.Date();
string to=(((globaldate.ToTime().ToInteger())+25200).ToTime().ToString().Substr(0,10)+"T"+((globaldate.ToTime().ToInteger())+25200).ToTime().ToString().Substr(11,8)+"Z");
string from=(((globaldate.ToTime().ToInteger())).ToTime().ToString().Substr(0,10)+"T"+((globaldate.ToTime().ToInteger())).ToTime().ToString().Substr(11,8)+"Z");
url="wget --timeout=3 -O - 'https://www.googleapis.com/calendar/v3/calendars/" #
        dom.GetObject(vrp+"HK1-GK-Kalender-ID").Value() # 
        "@group.calendar.google.com/events?orderBy=startTime&singleEvents=true&timeMax="+to #
        "&timeMin="+from+"&key="+dom.GetObject(vrp+"HK1-GK-API-Key").Value()+"'";

!//URL abfragen
system.Exec(url,&data,&error);
data=data.ToUpper();

!// Irgendwas muss gelesen worden sein
if(data==""){
  if (log){ logObj.State("Fehler beim Lesen der Termin-Daten von Google!"); }
  if(DEBUG){
    WriteLine("Fehler beim Lesen der Termin-Daten von Google!");
  }	
  quit;
}

!//Raumliste auslesen
sRaumliste=dom.GetObject(vrp+"HK1-R-Liste").Value().ToUpper();
sRaumVarListe=dom.GetObject(vrp+"HK2-HKG-Liste").Value();

if(data.Find("CALENDAR#EVENT")>-1){

  !//Die Raumliste druchgehen und Prüfen ob für den jeweiligen Raum ein Termin vorliegt.
  !//Liegt ein Termin vor dann die Daten des Termin aufbereiten und auf die NEUE Schaltliste setzen.
  foreach(AktRaum,sRaumliste.Split(";")){
    dataTemp=data;
    if(AktRaum!=""){
      EventPosID=dataTemp.Find(AktRaum);
      if(EventPosID>-1){
        
        sRaumVar = sRaumVarListe.StrValueByIndex(";",frRID);
        if (multiRaumVariante){
            sRaumVar=sRaumVar.StrValueByIndex("+",0);
        }	        
        if(dom.GetObject(dom.GetObject(sRaumVar)).Value().StrValueByIndex(";",1)=="S"){
          SHFlag="0;";
        }else{
          SHFlag="1;";
        };
        
        dataTemp=dataTemp.Substr(EventPosID,dataTemp.Length()-EventPosID);

       cap=dataTemp.Substr(dataTemp.Find("DESCRIP")+15,6);
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

        start=dataTemp.Substr(dataTemp.Find("START\":")+27,19);
        start=start.Replace("T"," ");
        stop=dataTemp.Substr(dataTemp.Find("END\":")+25,19);
        stop=stop.Replace("T"," ");
        toadd=AktRaum+";"+(start.ToTime().ToInteger().ToString())+";"+(stop.ToTime().ToInteger().ToString())+";"+cap+SHFlag;

        !// MRi: Wir fügen diesen Termin nur hinzu, wenn er nicht schon in der Liste vorhanden ist        
        if (SLN.Find(toadd)<0){
          SLN=SLN+toadd;
          if (log){
            logObj.State("Raum "+toadd.StrValueByIndex(";",0)+": "+toadd.StrValueByIndex(";",1).ToInteger().ToTime().Format("%X")+" / "+toadd.StrValueByIndex(";",2).ToInteger().ToTime().Format("%X")+" Heizen/Schalten: "+toadd.StrValueByIndex(";",4));
          }
        }
      }
    }
    frRID=frRID+1;
  }
  frRID=0;
}

!//------------------------------------------------------------------------------------------------
!// Ab hier haben wir Standard Code. Die Variablen SLN, SLA und SLT müssen belegt werden.
!// Der Rest ist in allen Skripten vom Typ1 gleich. 
!// SLT enthält unser gewünschtes Ergebnis

  !// Alte Liste SLN nach noch gültigen Einträgen durchsuchen und übernehmen
  integer whileID=0;
  while (true){
    if(SLA.StrValueByIndex(";",whileID)!=""){
	  !//noch zeitNachlauf min nach Ausschaltezeit in der Liste lassen, wegen Nachlaufzeit.
      !WriteLine("???? "+SLA.StrValueByIndex(";",whileID+2)+"  "+system.Date().ToTime().ToInteger().ToString()+"  "+SLA.StrValueByIndex(";",whileID+1).ToInteger().ToString());
      if((SLA.StrValueByIndex(";",whileID+2).ToInteger()+(zeitNachlauf*60))>system.Date().ToTime().ToInteger()){
        if(SLA.StrValueByIndex(";",whileID+1).ToInteger()<system.Date().ToTime().ToInteger()){
	        toadd = SLA.StrValueByIndex(";",whileID)+";"+SLA.StrValueByIndex(";",whileID+1)+";"+SLA.StrValueByIndex(";",whileID+2)+";"+SLA.StrValueByIndex(";",whileID+3)+";"+SLA.StrValueByIndex(";",whileID+4)+";";
          !// MRi: Wir fügen diesen Termin nur hinzu, wenn er nicht schon in der Liste vorhanden ist
		      if(DEBUG){
			      WriteLine("add: "+toadd);
		      }
		      if (SLT.Find(toadd)<0){
            SLT=SLT+toadd;
		      	if (log){
			        logObj.State("Raum "+toadd.StrValueByIndex(";",0)+": "+toadd.StrValueByIndex(";",1).ToInteger().ToTime().Format("%X")+" / "+toadd.StrValueByIndex(";",2).ToInteger().ToTime().Format("%X")+" Heizen/Schalten: "+toadd.StrValueByIndex(";",4));
	  	      }
          }  
		    }
      }
      whileID=whileID+5;
    }else{
      break;
    }
  }

  !// Neue Liste SLN nach noch gültigen Einträgen durchsuchen und übernehmen
  whileID=0;
  while (true){
    if(SLN.StrValueByIndex(";",whileID)!=""){
	    !// zeitVorlauf min vor Einschalttermin in Schaltliste aufnehmen
      if(SLN.StrValueByIndex(";",whileID+1).ToInteger()>(system.Date().ToTime().ToInteger()-300)){
        if(SLN.StrValueByIndex(";",whileID+1).ToInteger()<(system.Date().ToTime().ToInteger()+(zeitVorlauf*60))){
          toadd = SLN.StrValueByIndex(";",whileID)+";"+SLN.StrValueByIndex(";",whileID+1)+";"+SLN.StrValueByIndex(";",whileID+2)+";"+SLN.StrValueByIndex(";",whileID+3)+";"+SLN.StrValueByIndex(";",whileID+4)+";";
          !// MRi: Wir fügen diesen Termin nur hinzu, wenn er nicht schon in der Liste vorhanden ist
		      if(DEBUG){
			      WriteLine("add: "+toadd);
		      }
	     	  if (SLT.Find(toadd)<0){
            SLT=SLT+toadd;
	      		if (log){
			        logObj.State("Raum "+toadd.StrValueByIndex(";",0)+": "+toadd.StrValueByIndex(";",1).ToInteger().ToTime().Format("%X")+" / "+toadd.StrValueByIndex(";",2).ToInteger().ToTime().Format("%X")+" Heizen/Schalten: "+toadd.StrValueByIndex(";",4));
	  	      }
          }  
        }
      }
      whileID=whileID+5;
    }else{
      break;
    }
  }

  if (dom.GetObject(vrp+"HK1-Schaltliste").State()!=SLT){
    dom.GetObject(vrp+"HK1-Schaltliste").State(SLT);
    if (SLT==""){
	    if (log){ logObj.State("Neue Schaltliste: Keine Termine"); }
    }else{
      if (log){ logObj.State("Neue Schaltliste: "+SLT); }
    }
  } else {
    if (log){ logObj.State("Schaltliste unverändert"); }
  }

!Debugausgaben
if(DEBUG){
  WriteLine(sRaumliste+"\n");
  WriteLine("\n"+EventID);
  WriteLine("\n"+url+"\n");
  WriteLine(error+"\n");
  WriteLine(data);
  WriteLine("SLN: " +SLN);
  WriteLine("SLA: " +SLA);
  WriteLine("SLT: " +SLT);
}

!//------------------------------------------------------------------------------------------------

if(log){logObj.State("Ende Google-Skriptlauf");}
