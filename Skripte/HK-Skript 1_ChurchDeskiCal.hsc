!// Skript 1 um die Termine aus ChurchDesk auszulesen (iCal)
!//================================================================================================
!// Stand:    13.01.2026 
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
!//  HKP-ICS-CD-3.1.1 ICal_ChurchDesk_V1.1.6.c
!// Der ursprüngliche Code wurde geschrieben von:
!//   Lukas Helduser (Youtube: https://www.youtube.com/LukasvandeHaag)
!// Ich (MRi) habe diesen Code dann erweitert, korrigiert und verbessert um sie an die Nutzung in
!// meiner Gemeinde anzupassen.
!//
!// Skript sollte alle 30min laufen
!//

!// MRi: 2026-01-13 HK1-R-Liste erhält nun auch den Namen der Resource getrennt mit Gleichheitszeichen
!// MRi: 2026-01-01 Skript gegen fehlende Raumvariablen gesichert
!// MRi: 2025-12-09 Anpassung an ChurchDesk API
!// MRi: 2025-11-24 MultiRaumVariante, damit lassen sich mehrere Räume einer Ressource zuordnen.
!//                 korrektur nochmal für doppelte Schaltlisteneinträge

!//Eingabe eines Namens Präfix
!//Dies ist nur erforderlich wenn die Namensvorgabe beim erstellen den Systemvariablen geändert wurde.
!//Wird hier ein Präfix eingeben so muss dieser in allen Skripten auch angegeben werden.
string vrp="";

!//Debug ein oder aus
boolean DEBUG=0;

!//Multiraum Variante, dies unterstützt eine Raumliste in der mehrere Räume mit einem + gemeinsm geschaltet werden können.
boolean multiRaumVariante=true;

!// Zeitfenster in dem nach Termine geschaut wird
!// minus zeitNachlauf în Minuten (min = eingestellte Nachlaufzeit),
!// plus zeitVorlauf (min = maximale Vorlaufzeit)
integer zeitVorlauf=8*60;   !// 8 Stunden (default=12h)
integer zeitNachlauf=30;    !// 30min Stunden (default = 120min)

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

if(log){logObj.State("Beginn ChurchDesk-Skriptlauf");}
WriteLine("Beginn ChurchDesk-Skriptlauf");

!// Daten für den Zugriff setzen
string apiToken = dom.GetObject(vrp#"HK1-CD-Token").State();
string organizationId = dom.GetObject(vrp#"HK1-CD-OrganisationsId").State();

!// Filter für Resourcen setzen
string RIdListe=dom.GetObject(vrp#"HK1-R-Liste").State();
string HKGListe=dom.GetObject(vrp#"HK2-HKG-Liste").State();

!// Suche alle Ids der Ressourcen. Achtung es kann ein mit = abgetrennter Name vorhanden sein.
string filter="";
string RId;
foreach(RId,RIdListe.Split(";")){
  if (filter){
    filter = filter # "|";
  }
  filter = filter # RId.StrValueByIndex("=",0);
}

if(DEBUG){
  WriteLine("HKGListe=" # HKGListe);
  WriteLine("RIdListe=" # RIdListe);
}


!// Start und Enddatum setzen, End Datum = 2+Tage
!// Start und Enddatum setzen, End Datum = 1+Tage
integer versatzGMT=(system.Date("%z").Substr(1,2)).ToInteger()*3600;
integer JETZT=system.Date().ToTime().ToInteger();
string startDatum = JETZT.ToTime().ToString("%F");
string endDatum = (JETZT+172800).ToTime().ToString("%F");

!// Neue Schaltliste
string SLT="";

string RIdEintrag;
integer raumIndex = 0;
foreach(RIdEintrag,RIdListe.Split(";")){
  !// Nun suchen wir über die Ressource Id den Raum Index und den Namen. Leider hat dieser auch einen 
  !// Raumnamen oprional, das gestaltet die Suche etwas schwieriger
  string RId = RIdEintrag.StrValueByIndex("=",0);
  string RaumName=RIdEintrag.StrValueByIndex("=",1);
    
  !// Zugriff auf ChurchDesk iCal  
  string cmd = "wget --timeout=5 -O - 'https://api2.churchdesk.com/ical/resource/" # RId # "/public?organizationId="# organizationId #"'";
  if(DEBUG){
    WriteLine("---------------------------------------------------------------------------------------");
    WriteLine("Cmd:" # cmd);
  }
  string stdout;
  string stderr;
  system.Exec(cmd, &stdout, &stderr);

  if(DEBUG){
    !WriteLine("stdout:" # stdout);
    !WriteLine("stderr:" # stderr);
  }

  !// Tabs entfernen, sollten welche drin sein. Newlines setzen
  stdout = stdout.Replace("\t"," ").Replace("\r\n","\n");

  if (!stdout.StartsWith("BEGIN:VCALENDAR")){
    if (log){ logObj.State("Fehler beim Lesen der Event-Daten von ChurchDesk!"); }
    if(DEBUG){
      WriteLine("Fehler beim Lesen der Event-Daten von ChurchDesk!");
    }
  }else{
    !// Zeitzone abschneiden
    integer iPos = stdout.Find("\nBEGIN:VEVENT\n");
    if (iPos<0){
      !// Termindaten fehlerhaft
      continue;
    }

    !// Wir holen uns das Schalten/Heizen Flag nur aus dem ersten Raum, in der multiRaumVariante.
    !// In der Multiraumvariante haben wie mehere Raumeinträge durch + getrennt.
    !// MRi: Nach meinem Dafürhalten st diese Information in der Schaltliste redundant.
    string RaumVarListe=HKGListe.StrValueByIndex(";",raumIndex);
    string RaumVar = RaumVarListe;
    if (multiRaumVariante){
      RaumVar=RaumVar.StrValueByIndex("+",0);
    }
    if (RaumName==""){
      RaumName = RaumVarListe.Replace(vrp#"HKG-Raum-","");
    }

    object objVar = dom.GetObject(RaumVar);
    if (!objVar){
      !// Raumvariable nicht vorhanden
      if(log) { logObj.State("Raumvariable " # RaumVar # " nicht vorhanden!"); }
      if (DEBUG) { WriteLine("Raumvariable " # RaumVar # " nicht vorhanden!"); }
      continue;
    }

    string SchaltenHeizen=objVar.State().StrValueByIndex(";",1);
    string SHFlag;
    if(SchaltenHeizen=="H"){
      SHFlag="1";
    }else{
      SHFlag="0";
    };

    !// Laufe über alle Termine
    string termine = stdout.Substr(iPos,stdout.Length()-iPos).Replace("\nEND:VEVENT\n","\t");
    string termin;
    foreach(termin,termine){
      !// Schleife über all einzelnen Termine
      if(DEBUG){
        !WriteLine("Termin Daten: " # termin);
      }

      !// Start und Enddatum holen.
      iPos = termin.Find("\nDTSTART:");
      if (iPos<0){
        !// Kein Termineintrag mehr
        continue;
      }
      startDatum=termin.Substr(iPos+9,13).Replace("T"," ");
      startDatum=startDatum.Substr(0,4)#"-"#startDatum.Substr(4,2)#"-"#startDatum.Substr(6,2)#" "#startDatum.Substr(9,2)#":"#startDatum.Substr(11,2);
      if (DEBUG){
        WriteLine(startDatum);
      }
      !WriteLine(startDatum.ToTime());
      startDatum=(startDatum.ToTime().ToInteger()+versatzGMT).ToString();
      !WriteLine(startDatum.ToInteger().ToTime());

      iPos = termin.Find("\nDTEND:");
      endDatum=termin.Substr(iPos+7,13).Replace("T"," ");
      endDatum=endDatum.Substr(0,4)#"-"#endDatum.Substr(4,2)#"-"#endDatum.Substr(6,2)#" "#endDatum.Substr(9,2)#":"#endDatum.Substr(11,2);
      if (DEBUG){
        WriteLine(endDatum);
      }
      !WriteLine(endDatum.ToTime());
      endDatum=(endDatum.ToTime().ToInteger()+versatzGMT).ToString();
      !WriteLine(endDatum.ToInteger().ToTime());

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

      !// Wir haben keine Raumbeschreibung
      string cap = "0";

      !// Verhindern, dass doppelte Einträge erzeugt werden.
      string toadd = RId # ";" # startDatum # ";" # endDatum # ";" # cap # ";" # SHFlag # ";";
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
            logObj.State("Raum: " # RaumName # " ("+toadd.StrValueByIndex(";",0)+") - " #
                         toadd.StrValueByIndex(";",1).ToInteger().ToTime().Format("%X") # " / " #
                         toadd.StrValueByIndex(";",2).ToInteger().ToTime().Format("%X") #
                         " Parameter: " # cap # " " #
                         ("Schalten;Heizen").StrValueByIndex(";",toadd.StrValueByIndex(";",4).ToInteger()));
          }
        }else{
          !// Wir haben den Sonderbefehl NH/NS
          if (log){
            logObj.State("Raum: " # RaumName # " ("+toadd.StrValueByIndex(";",0)+") - " #
                         toadd.StrValueByIndex(";",1).ToInteger().ToTime().Format("%X") # " / " #
                         toadd.StrValueByIndex(";",2).ToInteger().ToTime().Format("%X") #
                         " Nicht Heizen/Schalten (#NH#/#NS#)");
          }
        }
      }
    }
  }
  
  !// Nächster Raum
  raumIndex = raumIndex+1;
}

!//------------------------------------------------------------------------------------------------

!// Geänderte Schaltliste schreiben

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

if(log){logObj.State("Ende ChurchDesk-Skriptlauf");}
WriteLine("Ende ChurchDesk-Skriptlauf");
