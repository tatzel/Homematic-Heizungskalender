!// UNGETESTET!!! Skript 1 um die Termine aus ChurchDesk auszulesen
!//================================================================================================
!// Stand:    25.11.2025; 
!// Autor:    Lukas Helduser
!//           Martin Richter    (heizkalender@m-ri.de)
!// Projekt:  Helmut Diedrichs  (helmut@diedrichs.de)
!//================================================================================================
!// Dieser Code wurde im Rahmen der Heizkalender-Implementierung der Baptisten Gemeinde Hanau 
!// entwicklet.
!// Die Nutzung ist kostenlos, aber wir bitten die Nutzung an einer der obigen Email Adressen zu 
!// melden.
!//================================================================================================
!//
!// Der Code basiert in großen Teilen auf der Datei: 
!//  HKP-ICS-CD-3.1.1 ICal_ChurchDesk_V1.1.6.c
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

!//Logging in "Log" mit 1 zwingend einschalten oder mit 0 Ausschalten
boolean log=0;

!// Zeitfenster in dem nach Termine geschaut wird 
!// minus zeitNachlauf în Minuten (min = eingestellte Nachlaufzeit), 
!// plus zeitVorlauf (min = maximale Vorlaufzeit)
integer zeitVorlauf=8*60;		!// 8 Stunden (default=12h)
integer zeitNachlauf=30;		!// 30min Stunden (default = 120min)

!//#######---Ende Variabler Bereich---#############################################################################################################
!//Stript Variablen. Von Benutzer nicht zu verändern !!!!
string datasort;
string data;
string error;
string aktevent;
string aktRaum;
integer frRID=0;
string SLA=dom.GetObject(vrp+"HK1-Schaltliste").State();
string SLN;
string SLT;
string SHFlag;
string temp;
string start;
string stop;
string cap="0";
!//Raumliste auslesen und Zeit Horizont setzen
string sRaumliste=dom.GetObject(vrp+"HK1-R-Liste").Value().ToUpper();
string sRaumVarListe=dom.GetObject(vrp+"HK2-HKG-Liste").Value();
string sRaumVar;
integer horizon=system.Date().ToTime().ToInteger();
string toadd;

!// Logging vorbereiten
var logObj=dom.GetObject(vrp+"HK1-Log");
var loggingObj=dom.GetObject(vrp+"HK1-Logging");

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

if(log){logObj.State("Beginn ChurchDesk-Skriptlauf");}

frRID = 0;
foreach(aktRaum,sRaumliste.Split(";")){
  !//Kopfdaten
  string endurl="wget --timeout=3 -O - 'https://api2.churchdesk.com/ical/resource/"+aktRaum+"/public?organizationId="+dom.GetObject(vrp+"HK1-ICS-CD-Churchdesk-ID").Value()+"'";
  if(DEBUG){WriteLine(endurl+"\n");}
  system.Exec(endurl, &data, &error);

  !// Irgendwas muss gelesen worden sein
  if (data==""){
    if (log){ logObj.State("Fehler beim Lesen der Termin-Daten von ChurchDesk!"); }
    if(DEBUG){
      WriteLine("Fehler beim Lesen der Termin-Daten von ChurchDesk!");
    }	
    quit;
  }

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
      
      sRaumVar = sRaumVarListe.StrValueByIndex(";",frRID);
      if (multiRaumVariante){
          sRaumVar=sRaumVar.StrValueByIndex("+",0);
      }	        
      if(dom.GetObject(dom.GetObject(sRaumVar)).Value().StrValueByIndex(";",1)=="S"){
        SHFlag="0;";
      }else{
        SHFlag="1;";
      };
      
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

      !// MRi: Wir fügen diesen Termin nur hinzu, wenn er nicht schon in der Liste vorhanden ist
      toadd = aktRaum+";"+start+";"+stop+";"+cap+";"+SHFlag;
      if (SLN.Find(toadd)<0){
        SLN=SLN+toadd;
        if (log){
          logObj.State("Raum "+toadd.StrValueByIndex(";",0)+": "+toadd.StrValueByIndex(";",1).ToInteger().ToTime().Format("%X")+" / "+toadd.StrValueByIndex(";",2).ToInteger().ToTime().Format("%X")+" Heizen/Schalten: "+toadd.StrValueByIndex(";",4));
        }
      }
      cap = "0";
    }
  }else{
    if(DEBUG){WriteLine("Keine Termine!");}
  }
  
  frRID=frRID+1;
  if(DEBUG){WriteLine("------------------------------------------------");}
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

!//Debugausgaben
if(DEBUG){
  WriteLine("SLN: " +SLN);
  WriteLine("SLA: " +SLA);
  WriteLine("SLT: " +SLT);
}

!//------------------------------------------------------------------------------------------------

if(log){logObj.State("Ende ChurchDesk-Skriptlauf");}