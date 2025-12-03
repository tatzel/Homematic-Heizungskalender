!// Skript 1 um die Termine aus ChurchDesk auszulesen
!//================================================================================================
!// Stand:    25.11.2025;
!// Autor:    Lukas Helduser
!//           Martin Richter    (heizkalender@m-ri.de)
!// Projekt:  Helmut Diedrichs  (helmut@diedrichs.de)
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

!// MRi: 2025-12-02 Anpassung an ChurchTools Variante, Verbesserung der String Operationen ($->\t)
!//                 Allgemeine Vereinheitlichung des Codes
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

!//Logging in "Log" mit 1 zwingend einschalten oder mit 0 Ausschalten
boolean log=0;

!// Zeitfenster in dem nach Termine geschaut wird
!// minus zeitNachlauf în Minuten (min = eingestellte Nachlaufzeit),
!// plus zeitVorlauf (min = maximale Vorlaufzeit)
integer zeitVorlauf=8*60;   !// 8 Stunden (default=12h)
integer zeitNachlauf=30;    !// 30min Stunden (default = 120min)

!//#######---Ende Variabler Bereich---#############################################################################################################
!//Stript Variablen. Von Benutzer nicht zu verändern !!!!
string data;
string datafiltered;
string error;
string aktevent;
string aktRaum;
integer i=0;
string SLN;
string SLT;
string SHFlag;
string temp;
string value;
string start;
string stop;
string cap="0";
integer NOW=system.Date().ToTime().ToInteger();
!//Raumliste auslesen und Zeit Horizont setzen
string IDL=dom.GetObject(vrp+"HK1-R-Liste").Value().ToUpper();
string SRaumVarListe=dom.GetObject(vrp+"HK2-HKG-Liste").Value();
string RaumVarListe;
string RaumVar;
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

i = 0;
if(DEBUG){WriteLine("Raumliste="#IDL);}
foreach(aktRaum,IDL.Split(";")){
  !// Raumdaten bestimmen einmal bestimmen
  RaumVarListe=SRaumVarListe.StrValueByIndex(";",i);
  RaumVar=RaumVarListe;
  if (multiRaumVariante){
    RaumVar=RaumVar.StrValueByIndex("+",0);
  }
  if(dom.GetObject(dom.GetObject(RaumVar)).Value().StrValueByIndex(";",1)=="S"){
    SHFlag="0;";
  }else{
    SHFlag="1;";
  };

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
  }elseif(DEBUG){
    WriteLine(data);
  }

  integer iPos=data.Find("BEGIN:VEVENT");
  data=";"+data.Substr(iPos,data.Length()-iPos);
  !// Trennzeichen sicherheitshalber tauschen
  data=data.Replace("\t"," ");
  data=data.Replace(";",",");
  !// Zeilenschaltung auflösen und Termine trennen
  data=data.Replace("\r\n",";");
  data=data.Replace("END:VEVENT","\t");
  datafiltered="";

  !//Filter nach Zeit
  integer iTime;
  foreach(aktevent,data.Split("\t")){
    if(aktevent.Contains("DTSTART")){
      temp=aktevent.Substr(aktevent.Find("DTSTART")+8,15);
      temp=temp.Substr(0,4)+"-"+temp.Substr(4,2)+"-"+temp.Substr(6,2)+" "+temp.Substr(9,2)+":"+temp.Substr(11,2)+":"+temp.Substr(13,2);
      if(DEBUG){WriteLine("Termin:"#temp)};
      iTime=(temp.ToTime().ToInteger())+((system.Date("%z").ToInteger()/100)*3600);
      if((iTime>(NOW-(zeitVorlauf*60)))&&(iTime<(NOW+86400+(zeitNachlauf*60)))){
        datafiltered=datafiltered+aktevent+"\t";
      }
    }
  }
  datafiltered=datafiltered.ToUpper();

  if(DEBUG){
    WriteLine("------------------------");
    WriteLine("SRaumVarListe:\n"#SRaumVarListe);
    WriteLine("error:\n"#error);
    WriteLine("data:\n"#data);
    WriteLine("datafiltered:\n"#datafiltered);
    WriteLine("------------------------");
  }

  !//Events auflisten und eintragen
  string eventdetail;
  if(datafiltered.Contains("BEGIN:VEVENT")){
    foreach(aktevent,datafiltered.Split("\t")){
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

      cap = "0";

      !// MRi: Wir fügen diesen Termin nur hinzu, wenn er nicht schon in der Liste vorhanden ist
      toadd = aktRaum+";"+start+";"+stop+";"+cap+";"+SHFlag+RaumVarListe;
      if (SLN.Find(toadd)<0){
        if (SLN!=""){
          SLN=SLN+"\t";
        }
        SLN=SLN+toadd;
      }
    }
  }else{
    if(DEBUG){WriteLine("Keine Termine!");}
  }

  i=i+1;
  if(DEBUG){WriteLine("-------------------------------------------------------------------------");}
}

!//------------------------------------------------------------------------------------------------
!// Ab hier haben wir Standard Code. Die Variablen SLN, SLA und SLT müssen belegt werden.
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


!//Debugausgaben
if(DEBUG){
  WriteLine("------------------------------------------------");
  WriteLine("SLN: " +SLN);
  WriteLine("SLT: " +SLT);
  WriteLine("------------------------------------------------");
}

!//------------------------------------------------------------------------------------------------

if(log){logObj.State("Ende ChurchDesk-Skriptlauf");}