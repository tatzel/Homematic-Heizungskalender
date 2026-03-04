!// Skript 1 um die Termine aus ChurchTools auszulesen (API)
!//================================================================================================
!// Stand:    26.02.2026
!// Autoren:  Lukas Helduser    (Youtube: https://www.youtube.com/LukasvandeHaag)
!//           Martin Richter    (heizkalender@m-ri.de) http://blog.m-ri.de/
!// Projekt:  Helmut Diedrichs  (helmut@diedrichs.de) https://diedrichs.de
!//------------------------------------------------------------------------------------------------
!// Copyright (C) 2026 by Team Heizkalender:
!//   Lukas Helduser, Martin Richter (xMRi-Software), Helmut Diedrichs
!// Dieser Teil des Heizkalenders ist freie Software und wird unter der GNU General Public License 
!// Version 3 (GPLv3) oder neuer veröffentlicht.
!// Es besteht keinerlei Garantie oder Haftung. Nutzung auf eigene Verantwortung.
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

!// MRi: 2026-02-19 Heizen mit Schalten eingebaut, Schaltliste umgebaut
!// MRI: 2026-02-05 Leere Raumzuordnung berücksichtigen
!// MRi: 2026-01-13 HK1-R-Liste erhält nun auch den Namen der Resource getrennt mit Gleichheitszeichen
!// MRi: 2026-01-01 Skript gegen fehlende Raumvariablen gesichert
!// MRi: 2025-12-10 Neue Sonderbefehle #GT# #NH# #NS#
!// MRi: 2025-12-08 Altes Skript komplett überarbeitet
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

!// Zeitfenster in dem nach Termine geschaut wird 
!// minus zeitNachlauf în Minuten (min = eingestellte Nachlaufzeit), 
!// plus zeitVorlauf (min = maximale Vorlaufzeit)
integer zeitVorlauf=12*60;		!// 12 Stunden (default=12h)
integer zeitNachlauf=30;		!// 30min Stunden (default = 120min)

!// Logging in "Log" mit 1 zwingend einschalten oder mit -1 zwingend Ausschalten
!// Mit 0 wird die Einstellunge aus der HKx-Logging übernommen
integer log=0;

!//################################################################################################
!//######------Skript Variablen und Skript Arbeitsteil. Vom Benutzer nicht zu verändern------######

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

if(log){logObj.State("Beginn ChurchTools-Skriptlauf=====================");}
WriteLine("Beginn ChurchTools-Skriptlauf");


!// Daten für den Zugriff setzen
string loginToken = dom.GetObject(vrp # "HK1-CT-Token").State();
string gemeindeName = dom.GetObject(vrp # "HK1-CT-Gemeindename").State();

!// Filter für Resourcen setzen
string RIdListe=dom.GetObject(vrp#"HK1-R-Liste").State();
string HKGListe=dom.GetObject(vrp#"HK2-HKG-Liste").State();

!// Suche alle Ids der Ressourcen. Achting es kann ein mit = abgetrennter Name vorhanden sein.
string filter="";
string RId;
foreach(RId,RIdListe.Split(";")){
  filter = filter # "&resource_ids[]=" # RId.StrValueByIndex("=",0);
}

if(DEBUG){
  WriteLine("HKGListe=" # HKGListe);
  WriteLine("RIdListe=" # RIdListe);
}

!// Start und Enddatum setzen, End Datum = 1+Tage
integer versatzGMT=(system.Date("%z").Substr(1,2)).ToInteger()*3600;
integer JETZT=system.Date().ToTime().ToInteger();
string startDatum = JETZT.ToTime().ToString("%F");
string endDatum = (JETZT+86400).ToTime().ToString("%F");

!// Zugriff auf ChurchTools API
string cmd = "wget --timeout=3 -O - 'https://" # gemeindeName # ".church.tools/api/bookings?login_token=" # loginToken #
                                        filter # "&from=" # startDatum # "&to=" # endDatum # "'";  
if(DEBUG){
  WriteLine("Cmd:" # cmd);
}
string stdout;
string stderr;
system.Exec(cmd, &stdout, &stderr);

if(DEBUG){
  WriteLine("stdout:" # stdout);
  WriteLine("stderr:" # stderr);
}

!//------------------------------------------------------------------------------------------------

!// Neue Schaltliste
string SLT="";

!// Ergebn is prüfen
if (stdout.Contains("\"meta\":{\"count\":0}")){
  !// Keine Termine
  if(DEBUG){
    WriteLine("Keine Termine vorhanden!");
  }	
}elseif(!stdout.StartsWith("{\"data\":[")){  
  if (log){ logObj.State("Fehler beim Lesen der Event-Daten von ChurchTools!"); }
  if(DEBUG){
    WriteLine("Fehler beim Lesen der Event-Daten von ChurchTools!");
  }	
  quit;
}else{
  !// Termine durchlesen
  string termine = stdout.Replace("\"booking\":{\"base\":{","\t");
  string termin;
  foreach(termin,termine){
    !// Schleife über all einzelnen Termine
    if(DEBUG){
      WriteLine("Termin Daten: " # termin);
    }
    integer iPos = termin.Find("\"resourceId\":");
    if (iPos<0){
      continue;
    }
    
    !// Id der Ressource bestimmen
    string resId=termin.Substr(iPos+13,10).ToInteger();

    !// Nun suchen wir über die Ressource Id den Raum Index und den Namen. Leider hat dieser auch einen 
    !// Raumnamen oprional, das gestaltet die Suche etwas schwieriger
    !// Wenn Variable nicht gefunden innerer Schleife für diesen Durchgang beeenden
    boolean bGefunden = false;
    integer raumIndex = 0;
    string RaumName="";
    string RIdEintrag;
    foreach(RIdEintrag,RIdListe.Split(";")){
      RaumName = RIdEintrag.StrValueByIndex("=",1);
      if (RIdEintrag.StrValueByIndex("=",0)==resId){
        bGefunden = true;
        break;
      }
      raumIndex = raumIndex+1;
    }
    
    if (!bGefunden){
      !// Raum nicht in unserer Liste (dürfte eigentlich nicht passieren, da wir einen
      !// Filter für Resourcen haben.
      if(DEBUG){
        WriteLine("Raum Resource Id konnte nicht gefunden werden!");
      }	
      continue;
    }
    
    !// In der Multiraumvariante haben wie mehere Raumeinträge durch + getrennt.
    !// MRi: Nach meinem Dafürhalten st diese Information in der Schaltliste redundant.   
    string RaumVarListe=HKGListe.StrValueByIndex(";",raumIndex);
    string RaumVar = RaumVarListe;
    if (!RaumVarListe){
      if (DEBUG) { WriteLine("Keine Raumzuordnung!"); }
      continue;
    }
    RaumVar=RaumVar.StrValueByIndex("+",0);
    if (RaumName==""){
      RaumName = RaumVarListe.Replace(vrp#"HKG-Raum-","");
    }

    object objVar = dom.GetObject(RaumVar);
    if ((!RaumVar) || (!objVar)){
      !// Raumvariable nicht vorhanden
      if(log) { logObj.State("Raumvariable " # RaumVar # " nicht vorhanden!"); }
      if (DEBUG) { WriteLine("Raumvariable " # RaumVar # " nicht vorhanden!"); }
      continue;
    }
    
    !// Dieses Flag ist eigentlich nicht nötig, aber wir platzieren es aus Gründen
    !// der Rückwärtskompatibilität. Früher wurde 0=Schalten/1=Heizen verwendet. ich
    !// übertrage jetz den originalen Parameter.
    string SchaltenHeizen=objVar.State().StrValueByIndex(";",1);
    
    !// Start und Enddatum holen.
    iPos = termin.Find("\"calculated\":{");
    string strTemp = termin.Substr(iPos+14,termin.Length()-iPos-14);
    startDatum=strTemp.Substr(strTemp.Find("\"startDate\":\"")+13,19).Replace("T"," ");
    if (DEBUG){
      WriteLine(startDatum);
    }
    startDatum=(startDatum.ToTime().ToInteger()+versatzGMT).ToString();
    
    endDatum=strTemp.Substr(strTemp.Find("\"endDate\":\"")+11,19).Replace("T"," ");
    if (DEBUG){
      WriteLine(endDatum);
    }
    endDatum=(endDatum.ToTime().ToInteger()+versatzGMT).ToString();
    
    !// Termine nur übernehmen wenn sie im Zeitrahmen liegen
    if ((startDatum.ToInteger()-(zeitVorlauf*60))>JETZT){
      !// Termin liegt in der Zukunft
      if (DEBUG){
        WriteLine("Termin liegt in der Zukunft");
      }
      continue;
    }
    if ((endDatum.ToInteger()+(zeitNachlauf*60))<=JETZT){
      !// Termin liegt in der Vergangenheit
      if (DEBUG){
        WriteLine("Termin liegt in der Vergangenheit");
      }
      continue;
    }

    if (DEBUG){
      WriteLine("Termin:\t" # resId # " / " # RaumName # "\t" # startDatum.ToInteger().ToTime() # "\t" # endDatum.ToInteger().ToTime());  
    }
    
    !// Beschreibung muss noch auf Tokens geprüft werden.
    !// #EIN#, #AUS#, #GT#, #NS#, #NH#, #NORMAL#, #RESET#, #<zahl><text>#
    string cap="0";
    iPos=termin.Find("\"description\":\"");
    if (iPos>=0){
      !// Ende der Beschreibung finden
      strTemp = termin.Substr(iPos+15);
      iPos = strTemp.Find("\",\"");
      if (iPos>=0) {
        strTemp = strTemp.Substr(0,iPos);
      }
      
      !// Sonderbefehl suchen
      iPos = strTemp.Find("#");
      if (iPos>=0){
        strTemp = strTemp.Substr(iPos+1,strTemp.Length()-iPos-1);
        iPos = strTemp.Find("#");
        !// Ende vorhanden?
        if (iPos>=0){
          !// Englische (Dezimalpunkt) Deutsche (Dezimalkomma) Konvertierung
          strTemp = strTemp.Substr(0,iPos).ToUpper();
          strTemp.Replace(",",".");
          if (strTemp=="EIN"){
            !// Dauer EIN
            cap = "-2";
          }elseif(strTemp=="AUS"){
            !// Dauer AUS
            cap = "-1";
          }elseif((strTemp=="NORMAL") || (strTemp=="RESET")){
            !// Zurücksetzen RESET AUS/EIN
            cap = "-3";
          }elseif((strTemp=="GT")){
            !// Grundtemperatur GT
            cap = dom.GetObject(vrp+"HK2-Grundtemperatur").State().ToFloat().ToString(1);
          }elseif((strTemp=="NH") || (strTemp=="NS")){
            !// Nicht schalten/heizen (Es wird kein Listeneintrag erzeugt)
            cap = "";
          }else{
            !// Nimm die Zahl, die hier kommt.
            cap = strTemp.ToFloat();
            if ((cap>0) && (cap<30)){
              cap = cap.ToString(1);
            }else{
              cap = "0";
            }
          }
        }
      }      
    }      
        
    !// Verhindern, dass doppelte Einträge erzeugt werden.
 	  string toadd = resId # ";" # startDatum.ToInteger().ToTime() # ";" # endDatum.ToInteger().ToTime() # ";" # cap # ";" # SchaltenHeizen # ";";
    !WriteLine(toadd);
    if (SLT.Find(toadd)<0){
      if (cap){
        !// Schalt Eintrag setzen
        SLT=SLT+toadd;	  
        if (log){
          cap = toadd.StrValueByIndex(";",3).ToInteger();
          if (toadd.StrValueByIndex(";",4).ToInteger()!=0){
            if (cap<0){
              cap = ("AUS;EIN;NORMAL").StrValueByIndex(";",(1+cap)*(-1));
            }elseif(cap==0){
              cap = "Normaltemperatur";
            }else{
              cap = toadd.StrValueByIndex(";",3).ToFloat().ToString(1);
            }
          }
            !// Klartext erzeugen
            strTemp = SchaltenHeizen;
            if (SchaltenHeizen=="H")  { strTemp = "Heizen"; }
            if (SchaltenHeizen=="S")  { strTemp = "Schalten"; }
            if (SchaltenHeizen=="HS") { strTemp = "Heizen/Schalten"; }
            logObj.State("Raum: " # RaumName # " ("+toadd.StrValueByIndex(";",0)+") - " # 
                       toadd.StrValueByIndex(";",1).ToTime().Format("%X") # " / " # 
                       toadd.StrValueByIndex(";",2).ToTime().Format("%X") # 
                       " Parameter: " # cap # " " # 
                       strTemp);
        }
      }else{
        !// Wir haben den Sonderbefehl NH/NS
        if (log){
          logObj.State("Raum: " # RaumName # " ("+toadd.StrValueByIndex(";",0)+") - " # 
                       toadd.StrValueByIndex(";",1).ToTime().Format("%X") # " / " # 
                       toadd.StrValueByIndex(";",2).ToTime().Format("%X") # 
                       " Nicht Heizen/Schalten (#NH#/#NS#)");
        }
      }
	  }	
  }
}

!// Geänderte Schaltliste schreiben, aber nur wenn nötig.

if(DEBUG){
  WriteLine("SLT=" # SLT);
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

!//------------------------------------------------------------------------------------------------

if(log){logObj.State("Ende ChurchTools-Skriptlauf=======================");}

WriteLine("Ende ChurchTools-Skriptlauf");