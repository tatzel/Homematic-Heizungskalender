!// Skript 1 um die Termine aus ChurchDesk auszulesen (API)
!//================================================================================================
!// Stand:    05.02.2026
!// Autoren:  Martin Richter    (heizkalender@m-ri.de) http://blog.m-ri.de/
!// Projekt:  Helmut Diedrichs  (helmut@diedrichs.de) https://diedrichs.de
!//------------------------------------------------------------------------------------------------
!// Copyright (C) 2026 Martin Richter (xMRi-Software)
!// Dieser Teil des Heizkalenders ist freie Software und wird unter der GNU General Public License 
!// Version 3 (GPLv3) oder neuer veröffentlicht.
!// Es besteht keinerlei Garantie oder Haftung. Nutzung auf eigene Verantwortung.
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
!// Skript sollte alle 30min laufen
!// ***********************************************************************************************
!// ACHTUNG DIESES SKRIPT KANN NUR ÖFFENTLICHE TERMINE LESEN. Termine in ChurchDesk können 
!// folgende Typen haben: Öffentlich, Gemeinde, Gruppen, Privat. Nur Termine des ersten Typs 
!// "öffentlich" werden aktuell von der API zurückgegegeben.
!// ***********************************************************************************************

!// MRI: 2026-02-05 Leere Raumzuordnung berücksichtigen
!// MRi: 2026-01-13 HK1-R-Liste erhält nun auch den Namen der Resource getrennt mit Gleichheitszeichen
!// MRi: 2026-01-01 Skript gegen fehlende Raumvariablen gesichert
!// MRi: 2025-12-09 Anpassung an ChurchDesk API

!//Eingabe eines Namens Präfix
!//Dies ist nur erforderlich wenn die Namensvorgabe beim erstellen den Systemvariablen geändert wurde.
!//Wird hier ein Präfix eingeben so muss dieser in allen Skripten auch angegeben werden.
string vrp="";

!//Debug Ausgaben Ein und Aus schalten. 0 = Aus, 1 = Ein
boolean DEBUG=0;

!//Multiraum Variante, dies unterstützt eine Raumliste in der mehrere Räume mit einem + gemeinsm geschaltet werden können.
boolean multiRaumVariante=true;

!// Zeitfenster in dem nach Termine geschaut wird
!// minus zeitNachlauf in Minuten (min = eingestellte Nachlaufzeit),
!// plus zeitVorlauf (min = maximale Vorlaufzeit)
integer zeitVorlauf=8*60;		!// 8 Stunden (default=12h)
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
    filter = filter # ",";
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

!// Zugriff auf ChurchDesk API
string cmd = "wget --timeout=3 -O - 'https://api2.churchdesk.com/api/v3.0.0/events?partnerToken=" # apiToken # "&organizationId=" # organizationId #
                                        "&rid=" # filter # "&startDate=" # startDatum # "&endDate=" # endDatum # "'";
if(DEBUG){
  WriteLine("Cmd:" # cmd);
}
string stdout;
string stderr;
system.Exec(cmd, &stdout, &stderr);

if(DEBUG){
  !WriteLine("stdout:" # stdout);
  !WriteLine("stderr:" # stderr);
}

!//------------------------------------------------------------------------------------------------

!// Neue Schaltliste
string SLT="";

!// Ergebn is prüfen
if (stdout=="[]"){
  !// Keine Termine
  if(DEBUG){
    WriteLine("Keine Termine vorhanden!");
  }
}elseif(!stdout.StartsWith("[{\"id\":")){
  if (log){ logObj.State("Fehler beim Lesen der Event-Daten von ChurchDesk!"); }
  if(DEBUG){
    WriteLine("Fehler beim Lesen der Event-Daten von ChurchDesk!");
  }
}else{
  !// Termine durchlesen
  string termine = stdout.Replace("\"updatedAt\":\"","\t");
  !// Termine durchlesen
  string termin;
  foreach(termin,termine){
    !// Schleife über all einzelnen Termine
    if(DEBUG){
      !WriteLine("Termin Daten: " # termin);
    }

    !// Prüfe ob der Eintrag gültig ist
    integer iPos = termin.Find("\"description\":\"");
    if (iPos<0){
      !// Eintrag ist kein Termineintrag
      continue;
    }

    !// Aus der diese speziellen Features laden
    !// #EIN#, #AUS#, #GT#, #NS#, #NH#, #NORMAL#, #RESET#, #<zahl><text>#
    string cap="0";
    iPos=termin.Find(",\"summary\":\"");
    if (iPos>=0){
      !// Ende der Beschreibung finden
      string strTemp = termin.Substr(iPos+12);
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

    !// Start und Enddatum holen.
    iPos = termin.Find("\"startDate\":\"");
    strTemp = termin.Substr(iPos+13,termin.Length()-iPos-13);
    startDatum=strTemp.Substr(0,19).Replace("T"," ");
    if (DEBUG){
      WriteLine(startDatum);
    }
    startDatum=(startDatum.ToTime().ToInteger()+versatzGMT).ToString();

    iPos = strTemp.Find("\"endDate\":\"");
    endDatum=strTemp.Substr(iPos+11,19).Replace("T"," ");
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

    !// Suche alle Resourcen
    iPos = termin.Find("\"resources\":[{\"id\":");
    if (iPos<0){
      continue;
    }
    string resourcen = termin.Substr(iPos+19,termin.Length()-19);
    iPos = resourcen.Find("}],\"categories\":");
    if (iPos<0){
      continue;
    }
    resourcen = resourcen.Substr(0,iPos).Replace("},{\"id\":","\t");
    string resource;
    !// Nun den Termin für alle Resourcen erzeugen
    foreach(resource,resourcen){
      string resId = resource.ToInteger().ToString();
      
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

      !// Zuerst Ramzuordnung ermitteln.
      !// Ohne Raumzuordnung überspringen wir hier die Terminabfrage
      !// Wir holen uns das Schalten/Heizen Flag nur aus dem ersten Raum, in der multiRaumVariante.
      !// In der Multiraumvariante haben wie mehere Raumeinträge durch + getrennt.
      !// MRi: Nach meinem Dafürhalten ist diese Information in der Schaltliste redundant.
      string RaumVarListe=HKGListe.StrValueByIndex(";",raumIndex);
      string RaumVar = RaumVarListe;
      if (!RaumVarListe){
        if (DEBUG) { WriteLine("Keine Raumzuordnung!"); }
        continue;
      }
      if (multiRaumVariante){
        RaumVar=RaumVar.StrValueByIndex("+",0);
      }
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

      string SchaltenHeizen=objVar.State().StrValueByIndex(";",1);
      string SHFlag;
      if(SchaltenHeizen=="H"){
        SHFlag="1";
      }else{
        SHFlag="0";
      };

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
}

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