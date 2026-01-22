!// UNGETESTET!!! Skript 1 um die Termine aus iCal auszulesen
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
!//  HKP-ICS-A-3.1.1  ICal_Skript1_V1.3.2
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

!//Debug Ausgaben Ein und Aus schalten. 0 = Aus, 1 = Ein
boolean DEBUG=0;

!//Multiraum Variante, dies unterstützt eine Raumliste in der mehrere Räume mit einem + gemeinsm geschaltet werden können.
boolean multiRaumVariante=true;

!// Zeitfenster in dem nach Termine geschaut wird 
!// minus zeitNachlauf în Minuten (min = eingestellte Nachlaufzeit), 
!// plus zeitVorlauf (min = maximale Vorlaufzeit)
integer zeitVorlauf=8*60;		!// 8 Stunden (default=12h)
integer zeitNachlauf=30;		!// 30min Stunden (default = 120min)

!// Logging in "Log" mit 1 zwingend einschalten oder mit -1 zwingend Ausschalten
!// Mit 0 wird die Einstellunge aus der HKx-Logging übernommen
integer log=0;

!//#######---Ende Variabler Bereich---#############################################################################################################
!//Stript Variablen. Von Benutzer nicht zu verändern !!!!
string data;
string datasort;
string error;
integer horizon;
string aktevent;
string sRaumliste;
string sRaumVarListe;
string sRaumVar;
string aktRaum;
integer frRID=0;
string SLA=dom.GetObject(vrp+"HK1-Schaltliste").State();
string SLN;
string SLT;
string toadd;
string SHFlag;
string temp;
string start;
string stop;
string cap="0";

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

if(log){logObj.State("Beginn iCal-Skriptlauf");}

!//Kopfdaten
string endurl="wget --timeout=3 -O - '"+dom.GetObject(vrp+"HK1-ICS-Url").Value()+"'";

horizon=system.Date().ToTime().ToInteger();

system.Exec(endurl, &data, &error);

!// Irgendwas muss gelesen worden sein
if (data==""){
  if (log){ logObj.State("Fehler beim Lesen der iCal Termin-Daten!"); }
  if(DEBUG){
    WriteLine("Fehler beim Lesen der iCal Termin-Daten!");
  }	
  quit;
}

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
          cap="0";
          if(temp.Contains(aktevent)){                  
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
            !// MRi: Wir fügen diesen Termin nur hinzu, wenn er nicht schon in der Liste vorhanden ist
            toadd=aktRaum+";"+start+";"+stop+";"+cap+";"+SHFlag;
            if (SLN.Find(toadd)<0){
              SLN=SLN+toadd;	  
            }	
          }
        }
      }
      frRID=frRID+1;
    }
    frRID=0;
  }else{
    if(DEBUG){WriteLine("Keine Termine!");}
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
}

!//------------------------------------------------------------------------------------------------

if(log){logObj.State("Ende ChurchTools-Skriptlauf");}

!//Debugausgaben
if(DEBUG){
  WriteLine(sRaumliste+"\n");
  WriteLine("Neu: "+SLN+"\n");
  WriteLine("Alt: "+SLA+"\n");
  WriteLine("Temp: "+SLT+"\n");
  WriteLine("\n"+endurl+"\n");
  WriteLine(error+"\n");
  WriteLine(data+"\n\n");
  WriteLine(datasort+"\n\n");
}
