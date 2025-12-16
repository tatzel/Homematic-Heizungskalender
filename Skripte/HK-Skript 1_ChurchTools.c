!// Skript 1 um die Termine aus ChurchTools auszulesen
!//================================================================================================
<<<<<<< Updated upstream
!// Stand:    11.12.2025; 
=======
!// Stand:    16.12.2025; 
>>>>>>> Stashed changes
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
!//  HKP-CT-3.2.1 churchtools_Ressource_V2_8_8.c
!// Der ursprüngliche Code wurde geschrieben von:
!//   Lukas Helduser (Youtube: https://www.youtube.com/LukasvandeHaag) 
!// Ich (MRi) habe diesen Code dann erweitert, korrigiert und verbessert um sie an die Nutzung in
!// meiner Gemeinde anzupassen.
!//
!// Skript sollte alle 30min laufen
!//

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

!//Multiraum Variante, dies unterstützt eine Raumliste in der mehrere Räume mit einem + gemeinsm geschaltet werden können.
boolean multiRaumVariante=true;

!// Zeitfenster in dem nach Termine geschaut wird 
!// minus zeitNachlauf în Minuten (min = eingestellte Nachlaufzeit), 
!// plus zeitVorlauf (min = maximale Vorlaufzeit)
integer zeitVorlauf=12*60;		!// 12 Stunden (default=12h)
integer zeitNachlauf=30;		!// 30min Stunden (default = 120min)

!//Logging in "Log" mit 1 zwingend einschalten oder mit 0 Ausschalten
boolean log=0;

!//################################################################################################
!//######------Skript Variablen und Skript Arbeitsteil. Vom Benutzer nicht zu verändern------######

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
WriteLine("Beginn ChurchTools-Skriptlauf");


!// Daten für den Zugriff setzen
string loginToken = dom.GetObject(vrp # "HK1-CT-Token").State();
string gemeindeName = dom.GetObject(vrp # "HK1-CT-Gemeindename").State();

!// Filter für Resourcen setzen
string RIdListe=dom.GetObject(vrp#"HK1-R-Liste").State();
string HKGListe=dom.GetObject(vrp#"HK2-HKG-Liste").State();

!// Suche alle Ids der Ressourcen
string filter="";
string RId;
foreach(RId,RIdListe.Split(";")){
  filter = filter # "&resource_ids[]=" # RId;
}

!// Ab hier verwenden wir die RaumIdListe mit einem führenden und folgendem ";" um die Suche zu erleichtern
RIdListe = ";" # RIdListe # ";";
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
    
    !// Nun suchen wir den Raum Index mit einem Trick. Wir zählen einfach die
    !// vorhandenen Semikolons im SubSTr
    iPos = RIdListe.Find(";" # resId # ";");
    if (iPos<0){
      !// Raum nicht in unserer Liste (dürfte eigentlich nicht passieren, da wir einen
      !// Filter für Resourcen haben.
      if(DEBUG){
        WriteLine("Raum Resource Id konnte nicht gefunden werden!");
      }	
      continue;
    }
    integer raumIndex = (RIdListe.Substr(0,iPos).Length())-(RIdListe.Substr(0,iPos).Replace(";","").Length());
    
    !// Wir holen uns das Schalten/Heizen Flag nur aus dem ersten Raum, in der multiRaumVariante.
    !// In der Multiraumvariante haben wie mehere Raumeinträge durch + getrennt.
    !// MRi: Nach meinem Dafürhalten st diese Information in der Schaltliste redundant.   
    string RaumVarListe=HKGListe.StrValueByIndex(";",raumIndex);
    string RaumVar = RaumVarListe;
    if (multiRaumVariante){
      RaumVar=RaumVar.StrValueByIndex("+",0);
    }
    if (DEBUG){
      WriteLine("RaumVar:"+RaumVar+" "+dom.GetObject(RaumVar).State());
    }

    string SchaltenHeizen=dom.GetObject(RaumVar).State().StrValueByIndex(";",1);
    string SHFlag;
    if(SchaltenHeizen=="H"){
      SHFlag="1";
    }else{
      SHFlag="0";
    };
    
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
    if ((endDatum.ToInteger()+(zeitNachlauf*60))<JETZT){
      !// Termin liegt in der Vergangenheit
      if (DEBUG){
        WriteLine("Termin liegt in der Vergangenheit");
      }
      continue;
    }
    
    !// Beschreibung muss noch auf Tokens geprüft werden.
    !// #EIN#, #AUS#, #NORMAL#, #RESET#, #<zahl><text>#
    string cap="0";
    iPos=termin.Find("\"description\":\"");
    if (iPos>=0){
      !// Start und ende finden
      strTemp = termin.Substr(iPos+15,50);
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
            cap = strTemp.ToInteger();
            if ((cap>0) && (cap<30)){
              cap = cap.ToString();
            }else{
              cap = "0";
            }
          }
        }
      }      
    }      
        
    !// Verhindern, dass doppelte Einträge erzeugt werden.
 	  string toadd = resId # ";" # startDatum # ";" # endDatum # ";" # cap # ";" # SHFlag # ";";
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
          logObj.State("Raum: " # RaumVarListe.Replace(vrp#"HKG-Raum-","") # " ("+toadd.StrValueByIndex(";",0)+") - " # 
                       toadd.StrValueByIndex(";",1).ToInteger().ToTime().Format("%X") # " / " # 
                       toadd.StrValueByIndex(";",2).ToInteger().ToTime().Format("%X") # 
                       " Parameter: " # cap # " " # 
                       ("Schalten;Heizen").StrValueByIndex(";",toadd.StrValueByIndex(";",4).ToInteger()));
        }
      }else{
        !// Wir haben den Sonderbefehl NH/NS
        if (log){
          logObj.State("Raum: " # RaumVarListe.Replace(vrp#"HKG-Raum-","") # " ("+toadd.StrValueByIndex(";",0)+") - " # 
                       toadd.StrValueByIndex(";",1).ToInteger().ToTime().Format("%X") # " / " # 
                       toadd.StrValueByIndex(";",2).ToInteger().ToTime().Format("%X") # 
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

if(log){logObj.State("Ende ChurchTools-Skriptlauf");}
WriteLine("Ende ChurchTools-Skriptlauf");