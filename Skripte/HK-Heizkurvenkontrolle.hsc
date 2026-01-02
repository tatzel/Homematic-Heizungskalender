!// Tool zur Kontrolle der Heizkurve
!//================================================================================================
!// Stand:    02.01.2026
!// Autoren:  Martin Richter    (heizkalender@m-ri.de) http://blog.m-ri.de/
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
!// Skript sollte alle 5min laufen ca. 30 Sekunden nach dem Schaltskript 
!//


!//Eingabe eines Namens Präfix
!//Dies ist nur erforderlich wenn die Namensvorgabe beim erstellen den Systemvariablen geändert wurde.
!//Wird hier ein Präfix eingeben so muss dieser in allen Skripten auch angegeben werden.
string vrp="";

!//Debug Ausgaben Ein und Aus schalten. 0 = Aus, 1 = Ein
boolean DEBUG=1;

!//Multiraum Variante, dies unterstützt eine Raumliste in der mehrere Räume mit einem + gemeinsm geschaltet werden können.
boolean multiRaumVariante=true;

!//Logging in "Log" mit 1 zwingend einschalten oder mit 0 Ausschalten
boolean log=0;

!//#######---Ende Variabler Bereich---#############################################################
!//Im Folgenden Hier keine Veränderungen vornehmen!

!// Aktoren müssen einen Namen länger als diesen Wert haben, sonst werden Sie wie einen Parameter
!// in der Raumvariable behandelt.
integer minAktorNamenLaenge=10;

!//-------------------------------------------------------------
integer NOW=system.Date().ToTime().ToInteger();
string  OffsetAT=dom.GetObject(vrp+"HK2-Kurve").State();
string SListe=dom.GetObject(vrp+"HK1-Schaltliste").State();
string VarNamen=dom.GetObject(vrp+"HK2-HKG-Liste").State();
string RIDI=dom.GetObject(vrp+"HK1-R-Liste").State().ToUpper();
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

!// Prüfe logging erwartet wird
if ((!log) && loggingObj){
  if (loggingObj.State()!=0){
    log = true;
  }
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

  !// Zum Listenelement passende Raumvariable suchen
  !// Wenn Variable gefunden innerer Schleife für diesen Durchgang beeenden

  string RIDIsearch = ";" # RIDI # ";";
  !// Nun suchen wir den Raum Index mit einem Trick. Wir zählen einfach die
  !// vorhandenen Semikolons im SubSTr
  iPos = RIDIsearch.Find(";" # AktSR # ";");
  if (iPos<0){
    !//Wird keine Raumvariable gefunden wir brechen das Script komplett ab
    if(DEBUG)  {WriteLine(AktSR # " Raumvariable nicht gefunden, Abbruch Skript !!");}
    continue;
  }
  integer raumIndex = (RIDIsearch.Substr(0,iPos).Length())-(RIDIsearch.Substr(0,iPos).Replace(";","").Length());

  !// Namen für das loggen ermitteln und Raumliste laden
  string RVNListe=VarNamen.StrValueByIndex(";",raumIndex);
  string AktSRName=RIDINamen.StrValueByIndex(";",raumIndex);
  !// Im Multiraum Fall nehmen wir nur den ersten / primären Raum
  if(multiRaumVariante){
    AktSRName=AktSRName.StrValueByIndex("+",0);
  }
  if(AktSRName==""){
    AktSRName = AktSR;
  }
  AktSRName = AktSRName # " (" # AktSR # ")";

  !// Wir haben nun einen Raum, oder in der multiRaumVariante eine Raumliste dirch + getrennt.
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
    !//     0 = Warte auf Heizen
    !//     1 = Heizen / Warte auf Temperatur erreicht oder abschalten
    !//     2 = Zieltemperatur erriecht / Warte auf Temperatur erreicht oder abschalten
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
    integer status = RaumParameter.StrValueByIndex(";",1).ToInteger();
    real maxTemperatur = RaumParameter.StrValueByIndex(";",2).ToFloat();

    !// istTemperatur bestimmen, wir benutzen nur den ersten Aktor dafür
    real istTemperatur = GT;
    AktAktor = RVI.StrValueByIndex(";",6);
    objAktor = dom.GetObject(AktAktor);
    if (objAktor){
      istTemperatur = objAktor.DPByHssDP("ACTUAL_TEMPERATURE").State().ToFloat();
    }else{
      if(DEBUG) {WriteLine(AktAktor+" Objekt existiert nicht!");}
    }
    
    !// Neues maximum bestimmen
    maxTemperatur = maxTemperatur.Max(istTemperatur);

    !// Prüfen ob Heizbeginn erkannt wird.
    if ((aktuellerSchaltZustand==1) && (status==0)){
      !// Heizbeginn erkannt
      sTemp = ((NOW-EIN)/60).ToString();
      if (!sTemp.StartsWith("-")){
        sTemp = "+" # sTemp;
      }
      logObj.State          (AktSRName # "-" # RVNName # " Heizbeginn: " # sTemp # "min, Ist: " # istTemperatur.ToString(1) # " Ziel: " # RTemp.ToString(1) # ", AT: " # AT.ToString(1));
      if (DEBUG) { WriteLine(AktSRName # "-" # RVNName # " Heizbeginn: " # sTemp # "min, Ist: " # istTemperatur.ToString(1) # " Ziel: " # RTemp.ToString(1) # ", AT: " # AT.ToString(1)); }
      status = 1;
    }

    !// Der Einschaltzeitpunkt wird 160sec in Zukunft und Vergangenheit (320sec) geprüft. Damit wird ein 5min (300sec) Interval abgedeckt.
    !// Das Skript sollte alle 5min laufen.
    if (((EIN-160)<NOW) && ((EIN+160)>NOW)){
      !// Einschaltpunkt erreicht
      sTemp = ((NOW-EIN)/60).ToString();
      if (!sTemp.StartsWith("-")){
        sTemp = "+" # sTemp;
      }
      logObj.State          (AktSRName # "-" # RVNName # " Terminbeginn: " # sTemp # "min, Ist: " # istTemperatur.ToString(1) # " Ziel: " # RTemp.ToString(1) # ", Max.: " # maxTemperatur.ToString(1));
      if (DEBUG) { WriteLine(AktSRName # "-" # RVNName # " Terminbeginn: " # sTemp # "min, Ist: " # istTemperatur.ToString(1) # " Ziel: " # RTemp.ToString(1) # ", Max.: " # maxTemperatur.ToString(1)); }
    }

    !// Warte auf Zieltemperatur.
    if (status==1){
      if (istTemperatur>=RTemp){
        !// Zieltemperatur erreicht
        sTemp = ((NOW-EIN)/60).ToString();
        if (!sTemp.StartsWith("-")){
          sTemp = "+" # sTemp;
        }
        logObj.State          (AktSRName # "-" # RVNName # " Zieltemperatur: " # sTemp # "min, Ist: " # istTemperatur.ToString(1) # " Ziel: " # RTemp.ToString(1) # ", Max.: " # maxTemperatur.ToString(1));
        if (DEBUG) { WriteLine(AktSRName # "-" # RVNName # " Zieltemperatur: " # sTemp # "min, Ist: " # istTemperatur.ToString(1) # " Ziel: " # RTemp.ToString(1) # ", Max.: " # maxTemperatur.ToString(1)); }
        status = 2;
      }
    }

    !// Der Ausschaltzeitpunkt wird 160sec in Zukunft und Vergangenheit (320sec) geprüft. Damit wird ein 5min (300sec) Interval abgedeckt.
    !// Das Skript sollte alle 5min laufen.
    if(((AUS-160)<NOW) && ((AUS+160)>NOW)){
      !// Ausschaltpunkt erreicht
      sTemp = ((NOW-EIN)/60).ToString();
      if (!sTemp.StartsWith("-")){
        sTemp = "+" # sTemp;
      }
      logObj.State          (AktSRName # "-" # RVNName # " Terminende: " # sTemp # "min, Ist: " # istTemperatur.ToString(1) # " Ziel: " # RTemp.ToString(1) # ", Max.: " # maxTemperatur.ToString(1));
      if (DEBUG) { WriteLine(AktSRName # "-" # RVNName # " Terminende: " # sTemp # "min, Ist: " # istTemperatur.ToString(1) # " Ziel: " # RTemp.ToString(1) # ", Max.: " # maxTemperatur.ToString(1)); }
      !// Setze den Status 0, wenn wir wirklich nicht mehr heizen. Andernfalls befinden wir uns schon wieder in
      !// einer neuen Heizphase
      if ((aktuellerSchaltZustand==0) && (status!=0)){
        !// Wir heizen nicht mehr
        status = 0;
      }
    }
    
    !// Wir speichern einen Raum nur, wenn er sich nicht in einem unbeheizten Zustand (status==0)
    !// befindet. Dadurch erreichen wir, dass auch überlappende Termine sofort in der nächsten
    !// SListe bearbeitet werden.
    if (status!=0){
      !// Neuen RaumParameter zusammensetzen
      RaumParameter = "#" # RVN # ";" # status # ";" # maxTemperatur.ToString(1);
      neueRaumListe = neueRaumListe # RaumParameter;
    }
  }
  !// Ende innere Schleife-------------------------------------------
}
!// Ende äußere Schleife---------------------------------------------

!// Neue RaumListen Daten speichern
if(DEBUG)  {WriteLine("Neue RaumListe=" # neueRaumListe);}
dom.GetObject(vrp+"HK-RäumeHeizkurvenkontrolle").State(neueRaumListe);

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

