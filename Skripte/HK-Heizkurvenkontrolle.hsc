!// Tool zur Kontrolle der Heizkurve
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
!// Skript sollte alle 5min laufen ca. 30 Sekunden nach dem Schaltskript 
!//

!// MRI: 2026-02-05 Leere Raumzuordnung berücksichtigen
!// MRi: 2026-01-13 HK1-R-Liste erhält nun auch den Namen der Resource getrennt mit Gleichheitszeichen

!//Eingabe eines Namens Präfix
!//Dies ist nur erforderlich wenn die Namensvorgabe beim erstellen den Systemvariablen geändert wurde.
!//Wird hier ein Präfix eingeben so muss dieser in allen Skripten auch angegeben werden.
string vrp="";

!//Debug Ausgaben Ein und Aus schalten. 0 = Aus, 1 = Ein
boolean DEBUG=0;

!//Multiraum Variante, dies unterstützt eine Raumliste in der mehrere Räume mit einem + gemeinsm geschaltet werden können.
boolean multiRaumVariante=true;

!// Logging in "Log" mit 1 zwingend einschalten oder mit -1 zwingend Ausschalten
!// Mit 0 wird die Einstellung aus der HKx-Logging übernommen.
!// log -1 macht hier keinen Sinn und beendet das Skript.
integer log=0;

!//#######---Ende Variabler Bereich---#############################################################
!//Im Folgenden Hier keine Veränderungen vornehmen!

!//-------------------------------------------------------------
integer NOW=system.Date().ToTime().ToInteger();
string  OffsetAT=dom.GetObject(vrp+"HK2-Kurve").State();
string SListe=dom.GetObject(vrp+"HK1-Schaltliste").State();
string VarNamen=dom.GetObject(vrp+"HK2-HKG-Liste").State();
string RIDI=dom.GetObject(vrp+"HK1-R-Liste").State();
integer ATG=dom.GetObject(vrp+"HK2-A.Temp.Grenze").State().ToFloat();
boolean Flag_Hand_Temp=dom.GetObject(vrp+"HK2-Hand-Temp").State();
boolean Flag_Hand_Grundtemp=dom.GetObject(vrp+"HK2-Hand-Grundtemp").State();
real AT=dom.GetObject(vrp+"HK2-Aussentemperatur").State().ToFloat();
real GTStandard=dom.GetObject(vrp+"HK2-Grundtemperatur").State().ToFloat();
real GT=GTStandard;
string AktAktor;
object objAktor;
string AGF;

!// Logging bestimmen
var logObj=dom.GetObject(vrp+"HK-LogHeizkurvenkontrolle");
var loggingObj=dom.GetObject(vrp+"HK-LoggingHeizkurvenkontrolle");

!// Prüfe ob logging erwartet wird
if (log<0) {
  !// Zwingend kein logging
  log = false;
  if(DEBUG)  {WriteLine("Logging ist nicht aktiviert. Abbruch!!!");}
  quit;
} elseif (log==0) {
!// Einstellung der Logging Variable prüfen
  if (loggingObj && loggingObj.State()!=0){
    log = true;
  }
} else {
  !// Logging einschalten
  log = true;
}

!// Logging auschalten, wenn keine Variable vorhanden
if (!logObj){
  !// Ohne logging macht das keinen Snn
  if(DEBUG)  {WriteLine("Logging ist nicht aktiviert. Abbruch!!!");}
  quit;
}

!// Dazu benötigen wir auch eine Variable, in der wie zu den Räumen, den jeweiligen
!// Status und die maximale Temperatur speichern.

var objVar=dom.GetObject(vrp+"HK-RäumeHeizkurvenkontrolle");
if (!objVar){
  if(DEBUG)  {WriteLine("HK-RäumeHeizkurvenkontrolle ist nicht vorhanden. Abbruch!!!");}
  quit;
}
string RaumListe = objVar.State();
string neueRaumListe="";

!// Baue eine simple Namensliste aus den HK2-HKG-Liste. Wir entfernen Prefix und im multiraum Fall auch die anderen Räume
!// Aus HKG-Raum-GrSaal, wird GrSaal. Aus HKG-Foyer wird Foyer
string RIDINamen=VarNamen.Replace("HKG-Raum-","").Replace("HKG-","");

WriteLine("Beginn Heizkurvenkontrolle");
if(DEBUG)  {WriteLine("Vollständige Raumliste: "+VarNamen);}

!// Beginn aussere Schleife------------------------------------------
!// Element von der Schaltliste nehmen und die dazu gehörige Raumvariablen suchen.
!// Wird die Raumvariable nicht gefunden wird der Eintrag übersprungen.

!// Index um über die Schaltliste mit jeweils 5 Einträgen zu schleifen
integer iPos = 0;
integer iEntry = 0;
while (iPos<SListe.Length()) {
  if (SListe.Substr(iPos,1)==";"){
    iEntry=iEntry+1;
    if ((iEntry%5)==0){
      SListe = SListe.Substr(0,iPos) # "\t" # SListe.Substr(iPos+1,SListe.Length()-iPos-1);
    }
  }
  iPos = iPos+1;
}

!// Schleife über die Schaltliste
string SLEintrag;
if(DEBUG){WriteLine("SListe=" # SListe);}
foreach(SLEintrag,SListe){
  if(DEBUG){WriteLine("SLEintrag=" # SLEintrag);}
  string AktSR=SLEintrag.StrValueByIndex(";",0);
  if(DEBUG){WriteLine("AktSR=" # AktSR);}
  !// Listenelement auslesen
  !// Parameter für aktuellen Schaltvorgang. Die Parameter werden später noch einmal gelesen
  !// und Final bestimmt. Auch das HSFlag wird aus der Raumbeschreibung gelesen.
  boolean HSFlag  = SLEintrag.StrValueByIndex(";",4).ToInteger()!=0;
  integer EIN     = SLEintrag.StrValueByIndex(";",1).ToInteger();
  integer AUS     = SLEintrag.StrValueByIndex(";",2).ToInteger();
  integer SDFlag  = SLEintrag.StrValueByIndex(";",3).ToInteger();
  real    RTemp   = SLEintrag.StrValueByIndex(";",3).ToFloat();

  if (SDFlag<0){
    !// Sollte es eine Dauerschaltung sein, ignorieren wir diesen Eintrag
    continue;
  }

  if(HSFlag){
  !// Sonderbefehl kontrollieren
    string cap;
    if (SDFlag<0){
      cap = ("AUS;EIN;NORMAL").StrValueByIndex(";",(1+SDFlag)*(-1));
    }else{
      if (SDFlag==0){
        cap = "Normaltemperatur";
      }else{
        cap = RTemp.ToString(1);
      }
    }
    if(DEBUG)  {WriteLine("---Schaltlisteneintrag für Ressource (" # AktSR # ") Heizen: " # EIN.ToTime().Format("%X").Substr(0,5) # " / " #
                                            AUS.ToTime().Format("%X").Substr(0,5) # "  Parameter " # cap);}
  }

  !// Nun suchen wir über die Ressource Id den Raum Index und den Namen. Leider hat dieser auch einen 
  !// Raumnamen oprional, das gestaltet die Suche etwas schwieriger
  !// Wenn Variable nicht gefunden innerer Schleife für diesen Durchgang beeenden
  boolean bGefunden = false;
  integer raumIndex = 0;
  string AktSRName="";
  string RIDIEintrag;
  foreach(RIDIEintrag,RIDI.Split(";")){
    !// Optionalen Namen suchen (getrennt durch =)
    AktSRName = RIDIEintrag.StrValueByIndex("=",1);
    if (RIDIEintrag.StrValueByIndex("=",0)==AktSR){
      bGefunden = true;
      break;
    }
    raumIndex = raumIndex+1;
  }

  if (!bGefunden){
    !//Wird keine Raumvariable gefunden wir brechen das Script komplett ab
    logObj.State(AktSR # " Raumvariable nicht gefunden, Abbruch Skript !!");
    if(DEBUG)  {WriteLine(AktSR # " Raumvariable nicht gefunden, Abbruch Skript !!");}
    continue;
  }  

  !// Namen für das loggen ermitteln und Raumliste laden
  string RVNListe=VarNamen.StrValueByIndex(";",raumIndex);
  if (AktSRName==""){
    AktSRName=RIDINamen.StrValueByIndex(";",raumIndex);
    !// Im Multiraum Fall nehmen wir nur den ersten / primären Raum
    if(multiRaumVariante){
      AktSRName=AktSRName.StrValueByIndex("+",0);
    }
    if(AktSRName==""){
      AktSRName = AktSR;
    }
  }
  AktSRName = AktSRName # " (" # AktSR # ")";
  if(DEBUG)  {WriteLine("AktSRName=" # AktSRName);}
  
  !// Wir haben nun einen Raum, oder in der multiRaumVariante eine Raumliste durch + getrennt.
  if(multiRaumVariante){
    if(DEBUG) {WriteLine(AktSRName # " Raumliste: "+RVNListe);}
    RVNListe=RVNListe.Split("+");
  }

  !// Beginn innere Schleife-----------------------------------------
  !// In der MultiRaumVariante haben wir eine Liste durch + getrennt, sonst nur einen Namen
  string RVN;
  if(DEBUG){WriteLine("RVNListe=" # RVNListe);}
  foreach(RVN,RVNListe){
    if(DEBUG){WriteLine("RVN=" # RVN);}
    string RVI=dom.GetObject(RVN).State();
    if(DEBUG){WriteLine("RVI=" # RVI);}

    !// Erzeuge einen Namen ohne prefixe
    string RVNName = RVN.Replace(vrp#"HKG-Raum-","");
    if(DEBUG)  {WriteLine("RVNName=" # RVNName);}

    !// Unser Skript macht nur Sinn für Modus Heizen
    HSFlag = RVI.StrValueByIndex(";",1)=="H";
    if (!HSFlag){
      if(DEBUG)  {WriteLine(AktSRName # "-" # RVNName # " Kein Heizvorgang");}
      continue;
    }

    !// Räume im Dauerschaltstatus werden ignoriert
    !// 0=Aus, 1=Ein, 2=Dauer AUS, 3=Dauer EIN
    integer aktuellerSchaltZustand = RVI.StrValueByIndex(";",0).ToInteger();
    if ((aktuellerSchaltZustand==3) ||(aktuellerSchaltZustand==2)){
      if(DEBUG)  {WriteLine(AktSRName # "-" # RVNName # " ist dauerhaft AN/AUS");}
      continue;
    }

    !// Es ist möglich, dass dieser Raum bereits abgearbeitet wurde. Aber wir
    if (neueRaumListe.Find("#" # RVN # ";")>=0){
      !// Diesen Raum haben wir bereits in einem anderen Schaltlisteneintrag abgeprüft.
      if(DEBUG)  {WriteLine(AktSRName # "-" # RVNName # " Raum wurde bereits bearbeitet!"); }
      continue;
    }

    !// Wir benötigen die Ein und Aussschaltzeit frisch, ebenso wie die Wohlfühltemperatur
    EIN = SLEintrag.StrValueByIndex(";",1).ToInteger();
    AUS = SLEintrag.StrValueByIndex(";",2).ToInteger();
    RTemp = SLEintrag.StrValueByIndex(";",3).ToFloat();

    !// Typ des Aktors bestimmen
    AGF=RVI.StrValueByIndex(";",2);

    !// Verhindern dass Ausschaltpunkt vor Einschaltpunkt liegt
    if((AUS<=EIN) || ((AUS+160)<NOW)){
      if(DEBUG)  {WriteLine(AktSRName # "-" # RVNName # " Einschaltzeit liegt nach Ausschaltzeit oder Auschaltzeitpunkt überschritten");}
      continue;
    }

    !// Vorgegebene Temperaur oder Raumtemperatur bestimmen
    if(SDFlag<=0){
      !// Temperaturen individuell bestimmen
      RTemp=RVI.StrValueByIndex(";",3).StrValueByIndex("/",0).ToFloat();
      GT=RVI.StrValueByIndex(";",3).StrValueByIndex("/",1).ToFloat();
      if(GT==0){
        GT=GTStandard;
      }
    }

    !// Daten aus aktueller Raumliste bestimmen.
    !//   Name
    !//   Messzustand
    !//   Bisherige maximal Temeratur
    string RaumParameter="";
    integer iPos = RaumListe.Find("#" # RVN # ";");
    if (iPos>=0){
      string sTemp = RaumListe.Substr(iPos+1,RaumListe.Length()-iPos-1);
      integer iPos2 = sTemp.Find("#");
      if (iPos2<0){
        iPos2 = sTemp.Length();
      }
      RaumParameter = sTemp.Substr(0,iPos2);
    }
    if (DEBUG) { WriteLine("RaumParameter=" # RaumParameter); }
    !// status bitmap. Diese verhindert, dass ein/aus/Zieltmperatur öfters als einmal 
    !// geloggt wird.
    !//   1 bit 0 = Heizung an
    !//   2 bit 1 = Zieltemperatur erreicht
    !//   4 bit 2 = Heizung ausgeschaltet
  
    integer status = RaumParameter.StrValueByIndex(";",1).ToInteger();
    real maxTemperatur = RaumParameter.StrValueByIndex(";",2).ToFloat();

    !// istTemperatur bestimmen, wir benutzen nur den ersten Aktor dafür
    real istTemperatur = GT;
    AktAktor = RVI.StrValueByIndex(";",6);
    if(DEBUG) { WriteLine("AktAktor=" # AktAktor); }
    objAktor = dom.GetObject(AktAktor);
    if (AktAktor && objAktor){
      istTemperatur = objAktor.DPByHssDP("ACTUAL_TEMPERATURE").State().ToFloat();
    }else{
      if(DEBUG) {WriteLine(AktAktor+" Objekt existiert nicht!");}
      continue;
    }
    
    !// Neues maximum bestimmen
    maxTemperatur = maxTemperatur.Max(istTemperatur);

    !// Prüfen ob Heizbeginn erkannt wird. Nur wenn bit 0 = aus
    if ((aktuellerSchaltZustand==1) && ((status & 1)==0)){
      !// Sollten wir einen neuen Heizbeginn haben, setzen wir die maxTemperatur wieder auf istTemperatur
      maxTemperatur = istTemperatur; 
      !// Heizbeginn erkannt
      sTemp = ((NOW-EIN)/60).ToString();
      if (!sTemp.StartsWith("-")){
        sTemp = "+" # sTemp;
      }
      logObj.State          (AktSRName # "-" # RVNName # " Heizbeginn: " # sTemp # "min, Ist: " # istTemperatur.ToString(1) # " Ziel: " # RTemp.ToString(1) # ", AT: " # AT.ToString(1));
      if (DEBUG) { WriteLine(AktSRName # "-" # RVNName # " Heizbeginn: " # sTemp # "min, Ist: " # istTemperatur.ToString(1) # " Ziel: " # RTemp.ToString(1) # ", AT: " # AT.ToString(1)); }
      !// bit 0 setzen und bit 2 zurücksetzen
      status = status & 251;
      status = status | 1;
    }

    !// Prüfen ob Heizende erkannt wird. Nur wenn bit 0 = ein und bit 2 = aus
    if ((aktuellerSchaltZustand==0) && ((status & 1)!=0) && ((status & 4)==0)){
      !// Heizbeginn erkannt
      sTemp = ((NOW-EIN)/60).ToString();
      if (!sTemp.StartsWith("-")){
        sTemp = "+" # sTemp;
      }
      logObj.State          (AktSRName # "-" # RVNName # " Heizende: " # sTemp # "min, Ist: " # istTemperatur.ToString(1) # " Ziel: " # RTemp.ToString(1) # ", Max.: " # maxTemperatur.ToString(1));
      if (DEBUG) { WriteLine(AktSRName # "-" # RVNName # " Heizende: " # sTemp # "min, Ist: " # istTemperatur.ToString(1) # " Ziel: " # RTemp.ToString(1) # ", Max.: " # maxTemperatur.ToString(1)); }
      !// Bit 0 löschen und bit 2 setzen
      status = status & 254;
      status = status | 4;
    }

    !// Warte auf Zieltemperatur. Nur wenn bit 1 = aus
    if ((status & 2)==0){
      if (istTemperatur>=RTemp){
        !// Zieltemperatur erreicht
        sTemp = ((NOW-EIN)/60).ToString();
        if (!sTemp.StartsWith("-")){
          sTemp = "+" # sTemp;
        }
        logObj.State          (AktSRName # "-" # RVNName # " Zieltemperatur: " # sTemp # "min, Ist: " # istTemperatur.ToString(1) # " Ziel: " # RTemp.ToString(1) # ", Max.: " # maxTemperatur.ToString(1));
        if (DEBUG) { WriteLine(AktSRName # "-" # RVNName # " Zieltemperatur: " # sTemp # "min, Ist: " # istTemperatur.ToString(1) # " Ziel: " # RTemp.ToString(1) # ", Max.: " # maxTemperatur.ToString(1)); }
        !// Bit 1 setzen
        status = status | 2;
      }
    }

    !// Der Terminanfang wird 160sec in Zukunft und Vergangenheit (320sec) geprüft. Damit wird ein 5min (300sec) Interval abgedeckt.
    !// Das Skript sollte alle 5min laufen.
    if (((EIN-160)<NOW) && ((EIN+160)>NOW)){
      !// Terminanfang erreicht
      sTemp = ((NOW-EIN)/60).ToString();
      if (!sTemp.StartsWith("-")){
        sTemp = "+" # sTemp;
      }
      logObj.State          (AktSRName # "-" # RVNName # " Terminbeginn: " # sTemp # "min, Ist: " # istTemperatur.ToString(1) # " Ziel: " # RTemp.ToString(1) # ", Max.: " # maxTemperatur.ToString(1));
      if (DEBUG) { WriteLine(AktSRName # "-" # RVNName # " Terminbeginn: " # sTemp # "min, Ist: " # istTemperatur.ToString(1) # " Ziel: " # RTemp.ToString(1) # ", Max.: " # maxTemperatur.ToString(1)); }
    }

    !// Der Terminende wird 160sec in Zukunft und Vergangenheit (320sec) geprüft. Damit wird ein 5min (300sec) Interval abgedeckt.
    !// Das Skript sollte alle 5min laufen.
    if(((AUS-160)<NOW) && ((AUS+160)>NOW)){
      !// Terminende erreicht
      sTemp = ((NOW-EIN)/60).ToString();
      if (!sTemp.StartsWith("-")){
        sTemp = "+" # sTemp;
      }
      logObj.State          (AktSRName # "-" # RVNName # " Terminende: " # sTemp # "min, Ist: " # istTemperatur.ToString(1) # " Ziel: " # RTemp.ToString(1) # ", Max.: " # maxTemperatur.ToString(1));
      if (DEBUG) { WriteLine(AktSRName # "-" # RVNName # " Terminende: " # sTemp # "min, Ist: " # istTemperatur.ToString(1) # " Ziel: " # RTemp.ToString(1) # ", Max.: " # maxTemperatur.ToString(1)); }
      !// Setze den Status 0, wenn wir wirklich nicht mehr heizen. Andernfalls befinden wir uns schon wieder in
      !// einer neuen Heizphase. Jeder andere Status ist uns egal.
      if (aktuellerSchaltZustand==0){
        !// Wir heizen nicht mehr. Damit vergessen wir alles, weil der Ausschalttermin gefallen ist.
        status = 0;
      }
    }
    
    !// Wir speichern einen Raum nur, wenn er sich nicht in einem unbeheizten Zustand (status==0)
    !// befindet. Dadurch erreichen wir, dass auch überlappende Termine sofort in der nächsten
    !// SListe bearbeitet werden.
    if (status!=0){
      !// Neuen RaumParameter zusammensezen
      RaumParameter = "#" # RVN # ";" # status # ";" # maxTemperatur.ToString(1);
      neueRaumListe = neueRaumListe # RaumParameter;
    }
  }
  !// Ende innere Schleife-------------------------------------------
}
!// Ende äußere Schleife---------------------------------------------

!// Neue RaumListen Daten speichern
if (dom.GetObject(vrp+"HK-RäumeHeizkurvenkontrolle").State()!=neueRaumListe){
  dom.GetObject(vrp+"HK-RäumeHeizkurvenkontrolle").State(neueRaumListe);
  if (neueRaumListe==""){
    if (DEBUG) { WriteLine("Neue Raumliste: Keine Termine"); }
  }else{
    if (DEBUG) { WriteLine("Neue Raumliste: "+neueRaumListe); }
  }
} else {
  if (DEBUG) { WriteLine("Raumliste unverändert"); }
}

!// -----------------------------------------------------------------

WriteLine("Ende Heizkurvenkontrolle");

