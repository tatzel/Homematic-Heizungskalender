!// Skript 1 um die Termine aus ChurchTools auszulesen
!//================================================================================================
!// Stand:    29.11.2025; 
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
!//  HKP-CT-3.2.1 churchtools_Ressource_V2_8_8.c
!// Der ursprüngliche Code wurde geschrieben von:
!//   Lukas Helduser (Youtube: https://www.youtube.com/LukasvandeHaag) 
!// Ich (MRi) habe diesen Code dann erweitert, korrigiert und verbessert um sie an die Nutzung in
!// meiner Gemeinde anzupassen.
!//
!// Skript sollte alle 30min laufen
!//

!// MRi: 2025-11-29 Alte Schaltliste wird nicht mehr übernommen um Schalttermine abbrechen zu können.
!// MRi: 2025-11-26 HK1-R-ListeNamen fest eingebaut für verbesseters Logging
!// MRi: 2025-11-24	Anpassung Doku. Doppelte Schaltlisteneinträge
!// MRi: 2025-11-16	Fehlerbehandlung eingebaut
!// MRi: 2025-11-14	MultiRaumVariante, damit lassen sich mehrere Räume einer Ressource zuordnen.
!//					        korrektur nochmal für doppelte Schaltlisteneinträge
!// MRi: 2025-11-12	Optimierung um doppelte Zeiteinträge in der Schaltliste zu verhindern
!// MRi: 2025-11-11	Logging über System Variablen HK1-Log (Text) und HK1-Logging (boolean) eingebaut.
!// 				        Damit das Logging korrekt arbeitet müssen beide Variablen vorhanden sein. Beide sollten
!//					        protokolliert werden. Alle Einträge finden sich dann im System Protokoll.

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

!//Logging in "Log" mit 1 zwingend einschalten oder mit 0 Ausschalten
boolean log=0;

!//################################################################################################################################################
!//##################-------Stript Variablen und Script Arbeitsteil. Von Benutzer nicht zu verändern !!!!-------##########################################################

string aktID;
string tempID;
string urlID;
string SLN;
string SLT;
string value;
string IDL=dom.GetObject(vrp+"HK1-R-Liste").State();
string urlwget ="wget --timeout=3 -O - '";
string urlR="https://"+dom.GetObject(vrp+"HK1-CT-Gemeindename").State()+".church.tools/api/bookings?login_token=";
string urlM="https://"+dom.GetObject(vrp+"HK1-CT-Gemeindename").State()+".church.tools/api/resource/masterdata";
string token=dom.GetObject(vrp+"HK1-CT-Token").State();
string from=system.Date("%F");
string to=(((from.ToTime().ToInteger())+86400).ToTime().ToString()).Substr(0,10);
integer GMT=(system.Date("%z").Substr(2,1)).ToInteger()*3600;
integer NOW=system.Date().ToTime().ToInteger();
string SRaumVarListe=dom.GetObject(vrp+"HK2-HKG-Liste").State();
string RaumVarListe;
string RaumVar;
string AktRaum;
string cap;
string SchaltenHeizen;
string SHFlag;
string start;
string end;
string id;
string cmd;
string toadd;
string Data;
string Feed1;
string Feed2;
integer i=0;

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

if(log){logObj.State("Beginn ChurchTools-Skriptlauf");}

cmd = urlwget+urlM+"?login_token="+token+"'";
if(DEBUG){
  WriteLine(cmd);
}
system.Exec(cmd, &Data, &Feed1);
if(DEBUG){
  WriteLine(Data);
}
Data=Data.ToUpper();

!// Suche ressourcen
integer pos = Data.Find("RESOURCES\":");

!// Wenn Schlüsselwort nicht vorhanden Abbruch
if(pos<0){
  if (log){ logObj.State("Fehler beim Lesen der Ressource-Daten von Churchtools!"); }
  if(DEBUG){
    WriteLine("Fehler beim Lesen der Ressource-Daten von Churchtools!");
  }	
  quit;
}

!// Suche alle Ressourcen
Data=Data.Substr(pos,Data.Length()-pos);
foreach(aktID,IDL.Split(";")){
  if(Data.Contains("\"ID\":"+aktID)){
    urlID=urlID+"resource_ids%5B%5D="+aktID+"&";
    tempID=tempID+aktID+";";
  }
}

cmd = urlwget+urlR+token+"&"+urlID+"&from="+from+"&to="+to+"'";
if(DEBUG){
  WriteLine(cmd);
}
system.Exec(cmd, &Data, &Feed2);
if(DEBUG){
  WriteLine(Data);
}
Data=Data.ToUpper();

!// Schlüsselwort muss vorhanden sein
if (!Data.Contains("COUNT\":")){
  if (log){ logObj.State("Fehler beim Lesen der Termin-Daten von Churchtools!"); }
  if(DEBUG){
    WriteLine("Fehler beim Lesen der Termin-Daten von Churchtools!");
  }	
  quit;
}else{
  !// Daten durchsuchen nach Terminen
  while(true){
    Data=Data.Substr(Data.Find("BOOKING\":")+12,Data.Length()-7);
    cap=Data.Substr(Data.Find("DESCRIPTION")+14,6);
    !WriteLine(cap+"\n");
    !WriteLine(Data+"\n");
    if (cap.Contains("#")==true){
      cap=cap.Replace("#","").Replace(",","").Replace("G","").Replace("D","").Replace(".","").Replace("\"","").Replace("V","");
      cap=cap.Replace("D","").Replace("E","").Replace(" ","").Replace("R","").Replace("A","").Replace("C","").Replace("°","").Replace("\r\n","");
      !WriteLine(cap);
      if(cap.Length()==3){cap=cap.Substr(0,2)+".5";}
      if(cap=="US"){cap="-1";}
      if(cap=="IN"){cap="-2";}
      if(cap=="NOML"){cap="-3";}
    }else{
      cap="0";
    }
    cap=cap+";";
    start=Data.Substr(Data.Find("CALCULATED\":{\"")+26,19).Replace("T"," ");
    start=((start.ToTime().ToInteger())+GMT).ToString()+";";

    end=Data.Substr(Data.Find("CALCULATED\":{\"")+59,19).Replace("T"," ");
    end=((end.ToTime().ToInteger())+GMT).ToString()+";";

    id=Data.Substr(Data.Find("RESOURCE\":{\"ID")+16,4)+";";
    id=id.Replace(",","").Replace("\"","").Replace("N","");

    !// Für den aktuellen Raum Schalten/Heizen bestimmen
    i=0;
    foreach(AktRaum,IDL.Split(";")){
      if(AktRaum!=""){
        if(AktRaum.ToInteger()==id.ToInteger()){
	        !// Wir holen uns das Schalten/Heizen Flag nur aus dem ersten Raum, in der multiRaumVariante.
    		  !// In der Multiraumvariante haben wie mehere Raumeinträge durch + getrennt.
    		  !// MRI: Nach meinem Dafürhalten st diese Information in der Schaltliste redundant.   
	        !WriteLine("Raum:"+SRaumVarListe.StrValueByIndex(";",i)); 
	        RaumVarListe=SRaumVarListe.StrValueByIndex(";",i);
          RaumVar = RaumVarListe;
	        if (multiRaumVariante){
       			RaumVar=RaumVar.StrValueByIndex("+",0);
	        }
	        !WriteLine("RaumVar:"+RaumVar+" "+dom.GetObject(RaumVar).State());
	  
	        SchaltenHeizen=dom.GetObject(RaumVar).State().StrValueByIndex(";",1);
          if(SchaltenHeizen=="H"){
      			SHFlag="1;";
	        }else{
	        	SHFlag="0;";
	        };
        }
      }
      i=i+1;
    }

    !// MRi: Wir fügen diesen Termin nur hinzu, wenn er nicht schon in der Liste vorhanden ist
	  toadd = id+start+end+cap+SHFlag+RaumVarListe;
    if (SLN.Find(toadd)<0){
      if (SLN!=""){
        SLN=SLN+"\t";
      }
	    SLN=SLN+toadd;	  
	  }	
    if(Data.Contains("BOOKING\":")==false){break;}
  }

!//------------------------------------------------------------------------------------------------
!// Ab hier haben wir Standard Code. Die Variablen SLN muss belegt werden. SLT ist leer.
!// Der Rest ist in allen Skripten vom Typ1 gleich. 
!// SLT enthält unser gewünschtes Ergebnis
  
  !// Neue Liste SLN nach noch gültigen Einträgen durchsuchen und übernehmen
  foreach(value,SLN) {
    !// zeitVorlauf min vor Einschalttermin in Schaltliste aufnehmen
    if((value.StrValueByIndex(";",2).ToInteger()+(zeitNachlauf*60))>NOW){
      if(value.StrValueByIndex(";",1).ToInteger()<(NOW+(zeitVorlauf*60))){
        toadd = value.StrValueByIndex(";",0)+";"+value.StrValueByIndex(";",1)+";"+value.StrValueByIndex(";",2)+";"+value.StrValueByIndex(";",3)+";"+value.StrValueByIndex(";",4)+";";
        !// MRi: Wir fügen diesen Termin nur hinzu, wenn er nicht schon in der Liste vorhanden ist
        if(DEBUG){
          WriteLine("add: "+toadd);
        }
        if (SLT.Find(toadd)<0){
          SLT=SLT+toadd;
          if (log){
            logObj.State("Raum: "+value.StrValueByIndex(";",5)+" ("+toadd.StrValueByIndex(";",0)+") - "+toadd.StrValueByIndex(";",1).ToInteger().ToTime().Format("%X")+" / "+toadd.StrValueByIndex(";",2).ToInteger().ToTime().Format("%X")+" Heizen/Schalten: "+toadd.StrValueByIndex(";",4));
          }
        }  
      }
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
}

!//------------------------------------------------------------------------------------------------

if(log){logObj.State("Ende ChurchTools-Skriptlauf");}