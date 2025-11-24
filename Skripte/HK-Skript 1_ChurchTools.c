!// Skript 1 um die Termine aus ChurchTools auszulesen
!//================================================================================================
!// Stand:    23.11.2025; 
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
!//  HKP-CT-3.2.1 churchtools_Ressource_V2_8_8.c
!// Dieser ursprüngliche Code wurde geschrieben von:
!//   Lukas Helduser (Youtube: https://www.youtube.com/LukasvandeHaag) 
!// Ich (MRi) habe diesen Code dann erweitert, korrigiert und verbessert um sie an die Nutzung in
!// meiner Gemeinde anzupassen.
!//
!// Skript sollte alle 5min laufen
!//

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
string SchaltenHeizen;
string RaumVar;
string SHFlag;
string end;
string id;
string cmd;
string toadd;
integer frID=0;
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
    start=Data.Substr(Data.Find("CALCULATED\":{\"")+26,19).Replace("T"," ");
    start=((start.ToTime().ToInteger())+GMT).ToString()+";";

    end=Data.Substr(Data.Find("CALCULATED\":{\"")+59,19).Replace("T"," ");
    end=((end.ToTime().ToInteger())+GMT).ToString()+";";

    id=Data.Substr(Data.Find("RESOURCE\":{\"ID")+16,4)+";";
    id=id.Replace(",","").Replace("\"","").Replace("N","");

    foreach(AktRaum,IDL.Split(";")){
      if(AktRaum!=""){
        if(AktRaum.ToInteger()==id.ToInteger()){
	        !// Wir holen uns das Schalten/Heizen Flag nur aus dem ersten Raum, in der multiRaumVariante.
    		  !// In der Multiraumvariante haben wie mehere Raumeinträge durch + getrennt.
    		  !// MRI: Nach meinem Dafürhalten st diese Information in der Schaltliste redundant.   
	        !WriteLine("Raum:"+SRaumVarListe.StrValueByIndex(";",frID)); 
	        RaumVar=SRaumVarListe.StrValueByIndex(";",frID);
	        if (multiRaumVariante){
       			RaumVar=RaumVar.StrValueByIndex("+",0);
	        }
	        !WriteLine("RaumVar:"+RaumVar+" "+dom.GetObject(RaumVar).State());
	  
	        SchaltenHeizen=dom.GetObject(RaumVar).State().StrValueByIndex(";",1);
          if(SchaltenHeizen=="H"){
      			SHFlag=";1;";
	        }else{
	        	SHFlag=";0;";
	        };
        }
      }
      frID=frID+1;
    }
    frID=0;

    !// MRi: Wir fügen diesen Termin nur hinzu, wenn er nicht schon in der Liste vorhanden ist
	  toadd = id+start+end+cap+SHFlag;
    if (SLT.Find(toadd)<0){
	    SLN=SLN+toadd;	  
	  }	
    if(Data.Contains("BOOKING\":")==false){break;}
  }

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
  WriteLine("SLA: " +SLA);
  WriteLine("SLT: " +SLT);
}

if(log){logObj.State("Ende ChurchTools-Skriptlauf");}