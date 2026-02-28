!// Skript 2 für das Schalten der Heizgruppen
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
!//  HKP-S2-3.2.1  Skript2 Schalten_Heizkalender V2.13.7.c
!// Der ursprüngliche Code wurde geschrieben von:
!//   Lukas Helduser (Youtube: https://www.youtube.com/LukasvandeHaag)
!// Ich (MRi) habe diesen Code dann erweitert, korrigiert und verbessert um sie an die Nutzung in
!// meiner Gemeinde anzupassen.
!//
!// Skript sollte alle 5min laufen
!//

!// MRi: 2026-02-19 Heizen mit Schalten eingebaut, Schaltliste umgebaut
!// MRI: 2026-02-05 Leere Raumzuordnung berücksichtigen
!// MRi: 2026-01-27 Begrenzung der Vorheizzeit nach unten auf mindestens 10%
!// MRi: 2026-01-13 HK1-R-Liste erhält nun auch den Namen der Resource getrennt mit Gleichheitszeichen
!// MRi: 2025-01-12 Bessere Behandlung von mehreren Aktoren in den Raumvars. minAktorNamenLaenge entfernt.
!// MRi: 2025-01-05 Begrenzung der Berücksichtigung der Raumtemperatur, Schaltzeiten korrekt berücksichtigen
!// MRi: 2026-01-01 Skript gegen fehlende Aktoren gesichert
!// MRi: 2025-12-29 Berücksichtigung der aktuellen Raumtemperatur bei der Vorheizzeit
!// MRi: 2025-12-22 Schaltskript arbeitete nicht für Schalten. Es wurde immer ein Heizvorgang angenommen
!// MRi: 2025-12-19 Log-Darstellung für Schalttemperaturen verbessert
!// MRi: 2025-12-18 Einfachere Parameter ermittlung je Typ der HomeMatic Geräte
!// MRi: 2025-12-17 Individuelle Absenktemperatur in Parameter 3 des RVI eingebaut, getrennt mit /
!// MRi: 2025-12-10 Grundsätzliche Überarbeitungen für Sonderbefehle #AUS# #EIN# #NORMAL#
!//                 Bessere Debugausgaben.
!// MRi: 2025-12-09 Einschaltverschiebung erhält optionalen Faktor (getrennt mit *)
!// MRi: 2025-12-08 Kosmetische Änderungen, Reduktion von State Aufrufen.
!// MRi: 2025-12-03 Schaltvorgänge reduzieren, wenn die entsprechende Temp. bereits gesetzt ist.
!// MRi: 2025-11-29 Beheizte Räume für die kein Schaltlisteneintrag vorhanden ist werden sofort abgeschaltet
!// MRi: 2025-11-26 HK1-R-ListeNamen fest eingebaut für verbesseters Logging
!// MRi: 2025-11-21 Kein Einschalten, wenn Schaltezeit <5min oder Ausschalktzeitpunkt vor Einschlatzeitpunkt liegt
!// MRi: 2025-11-20 Logging verbessert.
!// MRi: 2025-11-18 Logging verbessert, Behandlung der Grenztemperatur fürs Heizen Übersteuerung geändert
!// MRi: 2025-11-14 MultiRaumVariante, damit lassen sich mehrere Räume einer Ressource zuordnen.
!// MRi: 2025-11-13 Log-Ausgaben verbessert und präzisiert.
!// MRi: 2025-11-13 1. Auschaltzyklen Übersprungsicher gemacht! Das Programm muss aber alle 5min laufen
!//                 2. Ebenfalls schalten wir alle Heizkörper auf Grundtemperatur zurück und setzen die
!//                    Raumvariablen zurück wenn HK2-Hand-Grundtemp gesetzt ist
!//                 3. Ist die Schaltliste leer prüfen wir ob noch ein Raum geschaltet ist.
!// MRi: 2025-11-11 Logging über System Variablen HK2-Log (Text) und HK2-Logging (boolean) eingebaut.
!//                 Damit das Logging korrekt arbeitet müssen beide Variablen vorhanden sein. Beide sollten
!//                 protokolliert werden. Alle Einträge finden sich dann im System Protokoll.
!// MRi: 2025-11-10 Interpolation der Vorlaufzeit aus der Außentemperatur über die HK2-Kurve

!//Eingabe eines Namens Präfix
!//Dies ist nur erforderlich wenn die Namensvorgabe beim erstellen den Systemvariablen geändert wurde.
!//Wird hier ein Präfix eingeben so muss dieser in allen Skripten auch angegeben werden.
string vrp="";

!//Debug Ausgaben Ein und Aus schalten. 0 = Aus, 1 = Ein
boolean DEBUG=0;

!// Logging in "Log" mit 1 zwingend einschalten oder mit -1 zwingend Ausschalten
!// Mit 0 wird die Einstellunge aus der HKx-Logging übernommen
integer log=0;

!//#######---Ende Variabler Bereich---#############################################################
!//Im Folgenden Hier keine Veränderungen vornehmen!

!//-------------------------------------------------------------
!// Schaltparameter Varablen

!// IP- Thermostate-Aktoren-Gerätetyp (Kanal 1)
!//   BWTH_V1, BWTH_V2, TRVB_V1, TRV-C_V1, TRV, TRV-V1, TRV-V2, TRV-V3, TRV-V4, C_V2, WTH-2_V1, WTH-2_V2, WTH2_V3, WTH-BV1, WTH_V1, WT-V1
string AGFParamIP="SET_POINT_TEMPERATURE";

!// RT- Kennung Kanal Klassik-Thermostate-Aktoren-Gerätetyp (Kanal 4)
!//   HM-CC-RT-DN HM-CC-RT-DN
string AGFParamRT="SET_TEMPERATURE";

!// TC- Kennung Kanal Klassik-Thermostate-Aktoren-Gerätetyp (Kanal 4)
!//   HM-CC-TC
string AGFParamTC="SETPOINT";

!// IT- Kennung Kanal Klassik-Thermostate-Aktoren-Gerätetyp (Kanal 4)
!//   HM-TC-IT-WM-W-EU
string AGFParamIT="SET_TEMPERATURE";

!// SW- Kennung Kanal Klassik-Schalter-Aktoren-Gerätetyp (Kanal 1 bzw. 2)
!//   HM-LC-Sw1-FM HM-LC-Sw1PBU-FM, HM-LC-Sw2-FM, HM-ES-PMSw1-DR HM-LC-Sw1-PCB
!// Kennung Kanal IP-Schalter-Aktoren-Gerätetyp (Kanal 1)
!//   SW  Noch unerprobt
string AGFParamSW="STATE";

!//-------------------------------------------------------------
integer NOW=system.Date().ToTime().ToInteger();
string  OffsetAT=dom.GetObject(vrp+"HK2-Kurve").State();
integer GrundOffsetRaumAus=dom.GetObject(vrp+"HK2-VorzeitAus").State();
integer GrundOffsetRaumEin=dom.GetObject(vrp+"HK2-Kurvenversatz").State();
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
string AktorenListe;
object objAktor;
object objDP;
string AGF;
string AGFParam;
string Param;

!// Logging vorbereiten
var logObj=dom.GetObject(vrp+"HK2-Log");
var loggingObj=dom.GetObject(vrp+"HK2-Logging");

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

!// Baue eine simple Namensliste aus den HK2-HKG-Liste. Wir entfernen Prefix und im multiraum Fall auch die anderen Räume
!// Aus HKG-Raum-GrSaal, wird GrSaal. Aus HKG-Foyer wird Foyer
string RIDINamen=VarNamen.Replace("HKG-Raum-","").Replace("HKG-","");

if(log){logObj.State("Beginn Schaltskriptlauf===========================");}
WriteLine("Beginn Schaltskriptlauf");
if(SListe!=""){
  !// Log nur, wenn es auch was zu tun gibt
  !if(log) {logObj.State("Vollständige Raumliste: "+VarNamen);}
  if(DEBUG)  {WriteLine("Vollständige Raumliste: "+VarNamen);}
}

!// Beginn aussere Schleife------------------------------------------
!// Element von der Schaltliste nehmen und die dazu gehörige Raumvariablen suchen.
!// Wird die Raumvariable nicht gefunden wird der Eintrag übersprungen. Wir sammeln auch
!// alle Räume die wir Schalten. Um evtl. Räume zu finden, die noch die im Heizzustand sind
!// aber eigentlich keinen Schaltlisten Eintrag mehr haben. (Passiert wenn ein AUS Zeitpunkt
!// verpasst wird).

!// In dieser Variable sammeln wir Räume, die wie einschalten
string  RVNListeAn;

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
  string  HSFlag  = SLEintrag.StrValueByIndex(";",4);
  integer EIN     = SLEintrag.StrValueByIndex(";",1).ToTime().ToInteger();
  integer AUS     = SLEintrag.StrValueByIndex(";",2).ToTime().ToInteger();
  integer SDFlag  = SLEintrag.StrValueByIndex(";",3).ToInteger();
  real    RTemp   = SLEintrag.StrValueByIndex(";",3).ToFloat();

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

  !// Dieser Code hat keine Schaltwirkung, er ist rein informativ.
  if(HSFlag!="S"){
    !// Sonderbefehl kontrollieren und Raumtemperatur Sonderbefehl bestimmen
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

    string strTemp = "Heizen: ";
    if (HSFlag=="HS"){
      strTemp = "Heizen+Schalten: ";
    }
    if(log) {logObj.State("---Schaltlisteneintrag für Ressource " # AktSRName # " (" # AktSR # ") " # strTemp # EIN.ToTime().Format("%X").Substr(0,5) # " / " #
                                            AUS.ToTime().Format("%X").Substr(0,5) # "  Parameter " # cap);}
    if(DEBUG)  {WriteLine("---Schaltlisteneintrag für Ressource " # AktSRName # " (" # AktSR # ") " # strTemp # EIN.ToTime().Format("%X").Substr(0,5) # " / " #
                                            AUS.ToTime().Format("%X").Substr(0,5) # "  Parameter " # cap);}
  }else{
    !// Schalten kennt kein Offset
    if(log) {logObj.State("---Schaltlisteneintrag für Ressource (" # AktSR # ") Schalten: " # EIN.ToTime().Format("%X").Substr(0,5) # " - " #
                                            AUS.ToTime().Format("%X").Substr(0,5));}
    if(DEBUG)  {WriteLine("---Schaltlisteneintrag für Ressource (" # AktSR # ") Schalten: " # EIN.ToTime().Format("%X").Substr(0,5) # " - " #
                                            AUS.ToTime().Format("%X").Substr(0,5));}
  }

  if (!bGefunden){
    !//Wird keine Raumvariable gefunden wir brechen das Script komplett ab
    if(log) {logObj.State(AktSR # " Raumvariable nicht gefunden, Abbruch Skript !!");}
    if(DEBUG)  {WriteLine(AktSR # " Raumvariable nicht gefunden, Abbruch Skript !!");}
    continue;
  }

  !// Raumliste aus dem der HK2-HKG-Liste
  string RVNListe=VarNamen.StrValueByIndex(";",raumIndex);

  !// Wenn wir keinen Raumnamen haben, dann bauen wir uns einen. Optional ist der in der HK1-R-Liste
  !// getrennt mit =.
  if (AktSRName=="") {
    !// Namen für das loggen ermitteln und Raumliste laden
    string AktSRName=RIDINamen.StrValueByIndex(";",raumIndex);
    !// Im Multiraum Fall nehmen wir nur den ersten / primären Raum
    AktSRName=AktSRName.StrValueByIndex("+",0);
  }
  AktSRName = AktSRName # " (" # AktSR # ")";

  !// Wir haben multiRaumVariante eine Raumliste durch + getrennt.
  if(log){logObj.State(AktSRName # " Raumliste: "+RVNListe);}
  if(DEBUG) {WriteLine(AktSRName # " Raumliste: "+RVNListe);}
  RVNListe=RVNListe.Split("+");

  !// Beginn innere Schleife-----------------------------------------
  !// In der MultiRaumVariante haben wir eine Liste durch + getrennt
  string RVN;
  if(DEBUG){WriteLine("RVNListe=" # RVNListe);}
  foreach(RVN,RVNListe){
    if(DEBUG){WriteLine("RVN=" # RVN);}
    string RVI=dom.GetObject(RVN).State();
    if(DEBUG){WriteLine("RVI=" # RVI);}
    if(log){logObj.State(AktSRName # " Raumvariable: "+RVN+"="+RVI);}

    !// AktorenListe aufbauen. Das ist alles ab der siebte Eintrag der Raumliste. Das dient dazu
    !// Die Liste für spätere Schaltvorgänge bereit zu halten. Der alte Code hat damit gerechnet
    !// Das ein Aktorname eine Mindestlänge hat.
    iPos = 0;
    iEntry = 1;
    AktorenListe = "";
    while (iPos<RVI.Length()) {
      if (iEntry>6){
        AktorenListe = AktorenListe # RVI.Substr(iPos,1);
      } elseif (RVI.Substr(iPos,1)==";"){
        iEntry=iEntry+1;
      }
      iPos = iPos+1;
    }

    !// Erzeuge einen Namen ohne prefixe
    string RVNName = RVN.Replace(vrp#"HKG-Raum-","");

    !// Dauerschaltstatus anzeigen, wenn es den gibt.
    !// 0=Aus, 1=Ein, 2=Dauer AUS, 3=Dauer EIN
    integer aktuellerSchaltZustand = RVI.StrValueByIndex(";",0).ToInteger();
    if(aktuellerSchaltZustand==0){
      if(log){logObj.State(AktSRName # "-" # RVNName # " befindet sich im Schaltzustand AUS!");}
      if(DEBUG) {WriteLine(AktSRName # "-" # RVNName # " befindet sich im Schaltzustand AUS!");}
    }elseif (aktuellerSchaltZustand==1){
      if(log){logObj.State(AktSRName # "-" # RVNName # " befindet sich im Schaltzustand EIN!");}
      if(DEBUG) {WriteLine(AktSRName # "-" # RVNName # " befindet sich im Schaltzustand EIN!");}
    }elseif(aktuellerSchaltZustand==2){
      if(log){logObj.State(AktSRName # "-" # RVNName # " befindet sich im Schaltzustand Dauer-AUS!");}
      if(DEBUG) {WriteLine(AktSRName # "-" # RVNName # " befindet sich im Schaltzustand Dauer-AUS!");}
    }elseif (aktuellerSchaltZustand==3){
      if(log){logObj.State(AktSRName # "-" # RVNName # " befindet sich im Schaltzustand Dauer-EIN!");}
      if(DEBUG) {WriteLine(AktSRName # "-" # RVNName # " befindet sich im Schaltzustand Dauer-EIN!");}
    }

    !//Aktoren und Raumtemp setzen, und bestimmen ob wi Heizen oder Schalten
    HSFlag = RVI.StrValueByIndex(";",1);

    !// Wir benötigen die Ein und Aussschaltzeit frisch, weil diese hier manipuliert wird.
    !// Sie wird frisch aus der Schaltliste bezogen!
    EIN = SLEintrag.StrValueByIndex(";",1).ToTime().ToInteger();
    AUS = SLEintrag.StrValueByIndex(";",2).ToTime().ToInteger();
    RTemp = SLEintrag.StrValueByIndex(";",3).ToFloat();

    !// Typ des Aktors bestimmen
    AGF=RVI.StrValueByIndex(";",2);

    !// Schaltverzögerung berechnen
    integer offsetRaumAn=0;
    integer offsetRaumAus=0;
    if (SDFlag>=0){
      !// Die  Einschaltverschiebung, kann mit einem Faktor versehen sein.
      offsetRaumAn = 0-(RVI.StrValueByIndex(";",4).StrValueByIndex("*",0).ToInteger()*60);
      offsetRaumAus = 0-(RVI.StrValueByIndex(";",5).ToInteger()*60);
      !// Wir berücksichtigen die Offset Zeiten nur, bei normalen Heizvorgängen.
      if(HSFlag!="S"){
        offsetRaumAn = offsetRaumAn-(GrundOffsetRaumEin*60);
        offsetRaumAus = offsetRaumAus-(GrundOffsetRaumAus*60);
      }
    }
    if(SDFlag<=0){
      !// Temperaturen individuell bestimmen
      RTemp=RVI.StrValueByIndex(";",3).StrValueByIndex("/",0).ToFloat();
      GT=RVI.StrValueByIndex(";",3).StrValueByIndex("/",1).ToFloat();
      if(GT==0){
        GT=GTStandard;
      }
      if((log))
      {
        if (SDFlag==-3)     {logObj.State(AktSRName # "-" # RVNName # " Sonderfunktion \"Normalisierung\": " # GT.ToString(1));}
        elseif(SDFlag==-1)  {logObj.State(AktSRName # "-" # RVNName # " Sonderfunktion \"AUS\": " # GT.ToString(1));}
        elseif(SDFlag==-2)  {logObj.State(AktSRName # "-" # RVNName # " Sonderfunktion \"EIN\": " # RTemp.ToString(1));}
      }
    }

    !//Nach aktueller Aussentemperatur den Offsetwert berechnen und am Einschaltzeitpunkt abziehen und Aktor Paramter setzen
    real ux=0.0;
    real uy=0.0;
    real lx=0.0;
    real ly=0.0;
    if (HSFlag!="S"){
      !// Schaltparameter bestimmen
      AGFParam = "AGFParam"#AGF.ToUpper();
      Param = AGFParamIP;
      if (system.IsVar(AGFParam)){
        Param = system.GetVar(AGFParam);
      }

      !// Wir nutzen die Temperaturverschiebung nur, wenn wir einen normalen Schaltvorgang haben
      !// Spezial Befehle #EIN# #AUS# #NORMAL# werden zur normalen Zeit ausgeführt.
      if (SDFlag>=0){
        if(AT<-5.0){
          lx=-10.0;
          ly = OffsetAT.StrValueByIndex(";",0).ToFloat();
          ux=-5;
          uy = OffsetAT.StrValueByIndex(";",1).ToFloat();
        }elseif(AT<0.0){
          lx=-5.0;
          ly = OffsetAT.StrValueByIndex(";",1).ToFloat();
          ux=0.0;
          uy = OffsetAT.StrValueByIndex(";",2).ToFloat();
        }elseif(AT<8.0){
          lx=0.0;
          ly = OffsetAT.StrValueByIndex(";",2).ToFloat();
          ux=8.0;
          uy = OffsetAT.StrValueByIndex(";",3).ToFloat();
        }elseif(AT<10.0){
          lx=8.0;
          ly = OffsetAT.StrValueByIndex(";",3).ToFloat();
          ux=10.0;
          uy = OffsetAT.StrValueByIndex(";",4).ToFloat();
        }elseif(AT<12.0){
          lx=10.0;
          ly = OffsetAT.StrValueByIndex(";",4).ToFloat();
          ux=12.0;
          uy = OffsetAT.StrValueByIndex(";",5).ToFloat();
        }elseif(AT<15.0){
          lx=12.0;
          ly = OffsetAT.StrValueByIndex(";",5).ToFloat();
          ux=15.0;
          uy = OffsetAT.StrValueByIndex(";",6).ToFloat();
        }elseif(AT<17.5){
          lx=15.0;
          ly = OffsetAT.StrValueByIndex(";",6).ToFloat();
          ux=17.5;
          uy = OffsetAT.StrValueByIndex(";",7).ToFloat();
        }else{
          lx=0.0;
          ly=0.0;
        }

        !// linear Interpolieren
        real offsetTempAn=0.0;
        if((lx!=0)||(ly!=0)){
          offsetTempAn = (((uy-ly)/(ux-lx))*(AT-lx))+ly;
        }else{
          offsetTempAn = 0;
        }
        if (offsetTempAn<0){
          offsetTempAn = 0;
        }

        !// Bestimme den Verschiebungsfaktor zur Temperaturverschiebung. maximal 300%
        !// minmal 25%. Andere Werte setzen den Faktor auf
        real faktor1 = RVI.StrValueByIndex(";",4).StrValueByIndex("*",1).ToFloat();
        if (faktor1==0){
          faktor1 = 1.0;
        }
        faktor1 = faktor1.Max(0.20).Min(5.0);
        if(DEBUG)  {WriteLine("faktor1=" # faktor1.ToString(2));}

        !// Bestimme nun einen weiteren Faktor aus der ISTTemperatur und der Solltemperatur RTemp
        !// und der Grundtemperatur für den Raum (in diesem Fall wird nur die Temperatur, des ersten
        !// Aktors verwendet). Dadurch wird ein bereits warmer Raum nur so lange vorgeheizt, wie das
        !// die temperaturabhängige Vorheizzeit eben auch angibt, denn diese bezieht sich ja immer
        !// auf die Grundtemperatur.
        real ISTTemperatur = GT;
        AktAktor = RVI.StrValueByIndex(";",6);
        if(DEBUG) { WriteLine("AktAktor=" # AktAktor); }
        objAktor = dom.GetObject(AktAktor);
        if (AktAktor && objAktor){
          objDP = objAktor.DPByHssDP("ACTUAL_TEMPERATURE");
          if (!objDP){
            objDP = objAktor.DPByHssDP("TEMPERATURE");
          }
          if (objDP){
            ISTTemperatur = objDP.State().ToFloat();
            }else{
              if(log){logObj.State("Datenpunkt ACTUAL_TEMPERATURE nicht vorhanden!");}
              if(DEBUG) {WriteLine("Datenpunkt ACTUAL_TEMPERATURE nicht vorhanden!");}
            }
        }elseif(!AktAktor) {
          if(log){logObj.State(AktAktor+" Kein Aktor zugeordnet!");}
          if(DEBUG) {WriteLine(AktAktor+" Kein Aktor zugeordnet!");}
        }else{
          if(log){logObj.State(AktAktor+" Objekt existiert nicht!");}
          if(DEBUG) {WriteLine(AktAktor+" Objekt existiert nicht!");}
        }

        !// faktor2 wird nach unten auf 0.2 begrenzt. Besonders wenn wir bereits in der Heizphase sind.
        !// Sonst verschiebt sich die EIN Zeit immer weiter auf die AUS-Zeit zu. Was dazu führen könnte,
        !// dass die Heizung ausgeschaltet wird. faktor2 ist also ein Wert >=0.2
        real faktor2 = 1.0-((ISTTemperatur.Min(RTemp)-GT)/(RTemp-GT));
        faktor2 = faktor2.Max(0.2);
        offsetRaumAn = (offsetRaumAn.ToFloat()*faktor2).ToInteger();
        offsetTempAn = (0.0-(faktor1*faktor2*offsetTempAn).ToInteger()*60).ToInteger();
        if(DEBUG)  {WriteLine("faktor2=" # faktor2.ToString(2) # " - " # AT.ToString(1) # "/" # GT.ToString(1) # "/" # ISTTemperatur.ToString(1) # "°C offsetRaumAn=" # (offsetRaumAn/60) # " offsetTempAn=" # (offsetTempAn/60));}

        !// Reale Schaltzeiten berechnen
        string logText = AktSRName # "-" # RVNName # " Heizen - " # AT.ToString(1) # "/"  # GT.ToString(1) # "/" # ISTTemperatur.ToString(1) # "°C - "  #EIN.ToTime().Format("%X").Substr(0,5) # " ";
        if (offsetRaumAn>0){ logText=logText#"+"; }elseif(offsetRaumAn==0){ logText=logText#"-"; }
        logText = logText # (offsetRaumAn/60) #"min ";
        if (offsetTempAn==0){ logText=logText#" - "; }
        logText = logText # (offsetTempAn/60) # "min = ";
        EIN = EIN + offsetRaumAn + offsetTempAn;
        logText = logText # EIN.ToTime().Format("%X").Substr(0,5) # " / " # AUS.ToTime().Format("%X").Substr(0,5) # " ";
        if (offsetRaumAus>0){ logText=logText#"+"; }elseif(offsetRaumAus==0){ logText=logText#"-"; }
        AUS = AUS + offsetRaumAus;
        logText = logText # (offsetRaumAus/60) # "min = " # AUS.ToTime().Format("%X").Substr(0,5);
        if(log) {logObj.State(logText);}
        if(DEBUG)  {WriteLine(logText);}
      }
    }else{
      !// Reale Schaltzeiten berechnen
      string logText = AktSRName # "-" # RVNName # " Schalten - " #EIN.ToTime().Format("%X").Substr(0,5) # " ";
      if (offsetRaumAn>0){ logText=logText#"+"; }elseif(offsetRaumAn==0){ logText=logText#"-"; }
      logText = logText # (offsetRaumAn/60) #"min ";
      EIN = EIN + offsetRaumAn;
      logText = logText # EIN.ToTime().Format("%X").Substr(0,5) # " / " # AUS.ToTime().Format("%X").Substr(0,5) # " ";
      if (offsetRaumAus>0){ logText=logText#"+"; }elseif(offsetRaumAus==0){ logText=logText#"-"; }
      AUS = AUS + offsetRaumAus;
      logText = logText # (offsetRaumAus/60) # "min = " # AUS.ToTime().Format("%X").Substr(0,5);
      if(log) {logObj.State(logText);}
      if(DEBUG)  {WriteLine(logText);}

      !// Parameter setzen
      Param="STATE";
    }

    !// Verhindern dass Ausschaltpunkt vor Einschaltpunkt liegt
    if(AUS<=EIN){
      !// Durch das überspringen des Raumes hier, wird ein Raum evtl. ausgeschaltet, weil er nicht mehr
      !// als zu heizen gilt.
      if(log) {logObj.State(AktSRName # "-" # RVNName # " Einschaltzeit liegt nach Ausschaltzeit");}
      if(DEBUG)  {WriteLine(AktSRName # "-" # RVNName # " Einschaltzeit liegt nach Ausschaltzeit");}
      continue;
    }

    !// Das Schalten der Sonderbefehle erfolgt (wie das Ausschalten) nur einmal in dem Moment in dem der Schalt Zyklus den
    !// Einschaltpunkt erreicht. Der Einschaltzeitpunkt wird 160sec in Zukunft und Vergangenheit (320sec) geprüft. Damit wird
    !// ein 5min (300sec) Interval abgedeckt.
    !// Das Skript sollte alle 5m,in laufen. Der Ausschaltpunkt der Sonderbefehle hat keine Wirkung.

    !// Sonderbefehl: Ausschalten generell (Status 1/3 ==> 2)
    !// Geht nur, wenn wir heizen oder im Dauer-Ein sind.
    if(SDFlag==-1){
      if(aktuellerSchaltZustand!=2){
        if (((EIN-160)<NOW) && ((EIN+160)>NOW)){
          foreach(AktAktor,AktorenListe.Split(";")){
            objAktor = dom.GetObject(AktAktor);
            if (AktAktor && objAktor){
              objDP = objAktor.DPByHssDP(Param);
              if (objDP){
                if(HSFlag=="H"){
                  objDP.State(GT);
                  if(log){logObj.State(AktAktor+" wird dauerhaft ausgeschaltet auf Temp.: "+GT.ToString(1));}
                  if(DEBUG) {WriteLine(AktAktor+" wird dauerhaft ausgeschaltet auf Temp.: "+GT.ToString(1));}                
                }else{
                  objDP.State(0);
                  if(log){logObj.State(AktAktor+" wird dauerhaft ausgeschaltet. Parameter:" # Param);}
                  if(DEBUG) {WriteLine(AktAktor+" wird dauerhaft ausgeschaltet. Parameter:" # Param);}
                }
              }else{
                if(log){logObj.State("Datenpunkt " # Param # " nicht vorhanden!");}
                if(DEBUG) {WriteLine("Datenpunkt " # Param # " nicht vorhanden!");}
              }
            }else{
              if(log){logObj.State(AktAktor+" Objekt existiert nicht!");}
              if(DEBUG) {WriteLine(AktAktor+" Objekt existiert nicht!");}
            }
          }
          dom.GetObject(RVN).State("2;"+RVI.Substr(2,RVI.Length()-2));
          !// if(log){logObj.State(AktSRName # "-" # RVNName # " Schaltstatus setzen "+AktSR+" "+dom.GetObject(RVN).State().StrValueByIndex(";",0));}
          continue;
        }
      }else{
!       if(log){logObj.State(AktSRName # "-" # RVNName # " ist dauerhaft ausgeschaltet");}
!       if(DEBUG) {WriteLine(AktSRName # "-" # RVNName # " ist dauerhaft ausgeschaltet");}
      }
    }

    !// Sonderbefehl: Einschalten generell (Status 1/2 ==> 3)
    !// Geht nur wenn wir nicht heizen, oder im Dauer-Aus sind
    if(SDFlag==-2){
      if(aktuellerSchaltZustand!=3){
        if (((EIN-160)<NOW) && ((EIN+160)>NOW)){
          foreach(AktAktor,AktorenListe.Split(";")){
            objAktor = dom.GetObject(AktAktor);
            if (AktAktor && objAktor){
              objDP = objAktor.DPByHssDP(Param);
              if (objDP){
                if(HSFlag=="H"){
                  objDP.State(RTemp);
                  if(log){logObj.State(AktAktor +" wird dauerhaft eingeschaltet auf Temp.: "+RTemp.ToString(1));}
                  if(DEBUG) {WriteLine(AktAktor +" wird dauerhaft eingeschaltet auf Temp.: "+RTemp.ToString(1));}
                }else{
                  objDP.State(1);
                  if(log){logObj.State(AktAktor +" wird dauerhaft eingeschaltet");}
                  if(DEBUG) {WriteLine(AktAktor +" wird dauerhaft eingeschaltet");}
                }
              }else{
                if(log){logObj.State("Datenpunkt " # Param # " nicht vorhanden!");}
                if(DEBUG) {WriteLine("Datenpunkt " # Param # " nicht vorhanden!");}
              }
            }else{
              if(log){logObj.State(AktAktor+" Objekt existiert nicht!");}
              if(DEBUG) {WriteLine(AktAktor+" Objekt existiert nicht!");}
            }
          }
          dom.GetObject(RVN).State("3;"+RVI.Substr(2,RVI.Length()-2));
          !// if(log){logObj.State(AktSRName # "-" # RVNName # " Schaltstatus setzen "+AktSR+" "+dom.GetObject(RVN).State().StrValueByIndex(";",0));}
          continue;
        }
      }else{
!        if(log){logObj.State(AktSRName # "-" # RVNName # " ist dauerhaft eingeschaltet");}
!        if(DEBUG) {WriteLine(AktSRName # "-" # RVNName # " ist dauerhaft eingeschaltet");}
      }
    }

    !// Sonderbefehl: Rückstellung (Status 2/3 ==> 0)
    !// Rückstellen erlauben wir nur bei Dauer-Ein, oder Dauer-Aus
    if(SDFlag==-3){
      if((aktuellerSchaltZustand==2) || (aktuellerSchaltZustand==3)){
        if (((EIN-160)<NOW) && ((EIN+160)>NOW)){
          foreach(AktAktor,AktorenListe.Split(";")){
            objAktor = dom.GetObject(AktAktor);
            if (AktAktor && objAktor){
              objDP = objAktor.DPByHssDP(Param);
              if (objDP){
                if(HSFlag=="H"){
                  objDP.State(GT);
                  if(log){logObj.State(AktAktor +" Rückstellung aus dauerhafter Schaltung Ein/Aus auf Grundtemperatur: "+GT.ToString(1));}
                  if(DEBUG) {WriteLine(AktAktor +" Rückstellung aus dauerhafter Schaltung Ein/Aus auf Grundtemperatur: "+GT.ToString(1));}
                }else{
                  objDP.State(0);
                  if(log){logObj.State(AktAktor +" Rückstellung aus dauerhafter Schaltung auf AUS. Parameter:" # Param);}
                  if(DEBUG) {WriteLine(AktAktor +" Rückstellung aus dauerhafter Schaltung auf AUS. Parameter:" # Param);}
                }
              }else{
                if(log){logObj.State("Datenpunkt " # Param # " nicht vorhanden!");}
                if(DEBUG) {WriteLine("Datenpunkt " # Param # " nicht vorhanden!");}
              }
            }else{
              if(log){logObj.State(AktAktor+" Objekt existiert nicht!");}
              if(DEBUG) {WriteLine(AktAktor+" Objekt existiert nicht!");}
            }
          }
          dom.GetObject(RVN).State("0;"+RVI.Substr(2,RVI.Length()-2));
          !// if(log){logObj.State(AktSRName # "-" # RVNName # " Schaltstatus setzen "+AktSR+" "+dom.GetObject(RVN).State().StrValueByIndex(";",0));}
          continue;
        }
      }else{
!        if(log){logObj.State(AktSRName # "-" # RVNName # " ist bereits in einem normalen Schaltzustand");}
!        if(DEBUG) {WriteLine(AktSRName # "-" # RVNName # " ist bereits in einem normalen Schaltzustand");}
      }
    }

    !//Ausschalten (Status ==> 0)
    !//Liegt der Ausschaltzeitpunkt des aktuellen Schaltlistenelement in der Vergangenheit dann Raumvariable durchgehen und Aktoren auf Grundtemp bringen WENN Heizung
    !//noch nicht ausgeschaltet ist.
    if(SDFlag>=0){
      if(aktuellerSchaltZustand==1){
        !// Der war oder ist angeschaltet, wir vermerken ihn in der Liste der angeschalteten Räume, wenn er noch nicht drin ist
        !// Dadurch können wir Schaltzustände finden, deren Termin evtl. gelöscht wurde
        if (RVNListeAn.Find(RVN+";")<0){
           RVNListeAn=RVNListeAn+RVN+";";
        }
        !// Der Ausschaltzeitpunkt wird 160sec in Zukunft und Vergangenheit (320sec) geprüft. Damit wird ein 5min (300sec) Interval abgedeckt.
        !// Das Skript sollte alle 5m,in laufen.
        if(((AUS-160)<NOW) && ((AUS+160)>NOW)){
          foreach(AktAktor,AktorenListe.Split(";")){
            objAktor = dom.GetObject(AktAktor);
            if (AktAktor && objAktor){
              objDP = objAktor.DPByHssDP(Param);
              if (objDP){
                  if(HSFlag=="H"){
                    if(Flag_Hand_Temp!=false){
                      real istTemperatur = objDP.State();
                      if(istTemperatur==RTemp){
                        objDP.State(GT);
                        if(log) {logObj.State(AktAktor+" Ausschalten (mit Funktion Reglervorrang) - Ist: " # istTemperatur.ToString(1) # " Neu: " # GT.ToString(1) #" Parameter: " # Param);}
                        if(DEBUG)  {WriteLine(AktAktor+" Ausschalten (mit Funktion Reglervorrang) - Ist: " # istTemperatur.ToString(1) # " Neu: " # GT.ToString(1) #" Parameter: " # Param);}
                      }else{
                        if(log){logObj.State(AktAktor+" Reglervorrang bei AUS - Regler händisch verstellt auf Temp.: " # istTemperatur.ToString(1));}
                      }
                    }else{
                      objDP.State(GT);
                      if(log) {logObj.State(AktAktor+" Ausschalten (ohne Funktion Reglervorrang) auf Temp.: "+GT.ToString(1)+" Parameter: "+Param);}
                      if(DEBUG)  {WriteLine(AktAktor+" Ausschalten (ohne Funktion Reglervorrang) auf Temp.: "+GT.ToString(1)+" Parameter: "+Param);}
                    }
                }else{
                  objDP.State(0);
                  if(log) {logObj.State(AktAktor+" Ausschalten. Parameter: "+Param);}
                  if(DEBUG)  {WriteLine(AktAktor+" Ausschalten. Parameter: "+Param);}
                }
              }else{
                if(log){logObj.State("Datenpunkt " # Param # " nicht vorhanden!");}
                if(DEBUG) {WriteLine("Datenpunkt " # Param # " nicht vorhanden!");}
              }
            }else{
              if(log){logObj.State(AktAktor+" Objekt existiert nicht!");}
              if(DEBUG) {WriteLine(AktAktor+" Objekt existiert nicht!");}
            }
          }
          dom.GetObject(RVN).State("0;"+RVI.Substr(2,RVI.Length()-2));
          !// if(log){logObj.State(AktSRName # "-" # RVNName # " Schaltstatus setzen "+AktSR+" "+dom.GetObject(RVN).State().StrValueByIndex(";",0));}
          continue;
        }
      }
    }

    !//Einschalten (Status ==> 1)
    !//Liegt der Einschaltzeitpunkt in der Vergangenheit UND Ausschaltzeitpunkt in der Zukunft Heizung einschalten WENN diese noch nicht eingeschaltet ist.
    if(SDFlag>=0){
      if(aktuellerSchaltZustand==0){
        !// Sollte die Außentemperatur über unserem Schwellenwert liegen, Heizen wir nicht
        if((HSFlag=="S") || (AT<ATG)){
          !// Und wir schalten nur ein wenn die Einschaltzeit mehr als 5min beträgt
          if(((EIN+300)<AUS) && (EIN<=NOW) && (AUS>NOW)){
            !// Da wir die Heizung einschalten, setzen wir den Raum in die Heizliste
            if (RVNListeAn.Find(RVN+";")<0){
              RVNListeAn=RVNListeAn+RVN+";";
            }
            foreach(AktAktor,AktorenListe.Split(";")){
              objAktor = dom.GetObject(AktAktor);
              if (objAktor){
                objDP = objAktor.DPByHssDP(Param);
                if (objDP){
                  if(HSFlag=="H"){
                    if(Flag_Hand_Temp!=false){
                      real istTemperatur = objDP.State();
                      if(istTemperatur==GT){
                        objDP.State(RTemp);
                        if(log) {logObj.State(AktAktor+" Einschalten (mit Funktion Reglervorrang) - Ist: " # istTemperatur.ToString(1) # " Neu: " # RTemp.ToString(1) #" Parameter: " # Param);}
                        if(DEBUG)  {WriteLine(AktAktor+" Einschalten (mit Funktion Reglervorrang) - Ist: " # istTemperatur.ToString(1) # " Neu: " # RTemp.ToString(1) #" Parameter: " # Param);}
                      }else{
                        if(log){logObj.State(AktAktor+" Reglervorrang bei EIN (Regeler händisch verstellt auf Temp.: " # istTemperatur.ToString(1));}
                        if(DEBUG) {WriteLine(AktAktor+" Reglervorrang bei EIN (Regeler händisch verstellt auf Temp.: " # istTemperatur.ToString(1));}
                      }
                    }else{
                      objDP.State(RTemp);
                      if(log) {logObj.State(AktAktor+" Einschalten (ohne Funktion Reglervorrang) auf Temp.: "+RTemp.ToString(1)+" Parameter: "+Param);}
                      if(DEBUG)  {WriteLine(AktAktor+" Einschalten (ohne Funktion Reglervorrang) auf Temp.: "+RTemp.ToString(1)+" Parameter: "+Param);}
                    }
                  }else{
                    objDP.State(1);
                    if(log) {logObj.State(AktAktor+" Einschalten "+Param);}
                    if(DEBUG)  {WriteLine(AktAktor+" Einschalten "+Param);}
                  }
                }else{
                  if(log){logObj.State("Datenpunkt " # Param # " nicht vorhanden!");}
                  if(DEBUG) {WriteLine("Datenpunkt " # Param # " nicht vorhanden!");}
                }
              }else{
                if(log){logObj.State(AktAktor+" Objekt existiert nicht!");}
                if(DEBUG) {WriteLine(AktAktor+" Objekt existiert nicht!");}
              }
            }
            dom.GetObject(RVN).State("1;"+RVI.Substr(2,RVI.Length()-2));
            !// if(log){logObj.State(AktSRName # "-" # RVNName # " Schaltstatus setzen "+AktSR+" "+dom.GetObject(RVN).State().StrValueByIndex(";",0));}
            continue;
          }
        }else{
          if(log){logObj.State(AktSRName # "-" # RVNName # " Heizen abgebrochen Aussentemperatur " # AT # " größer Grenzwert "+ATG.ToString());}
          if(DEBUG) {WriteLine(AktSRName # "-" # RVNName # " Heizen abgebrochen Aussentemperatur " # AT # " größer Grenzwert "+ATG.ToString());}
        }
      }
    }
  }
  !// Ende innere Schleife-------------------------------------------
}
!// Ende äußere Schleife---------------------------------------------

!// Nachtschaltung---------------------------------------------------

!// Code für Prüfung der Nachschaltung immer zwischen 00:57 und 01:03 Uhr!
!// Die Nachtschaltung kontrolliert nur die Zustände 0,2,3. Ist noch ein normaler
!// Schaltzustand vorhanden, gehen wir davon aus, dass es einen Sc haltlisten Eintrag gibt.

if((Flag_Hand_Grundtemp!=false) && (NOW.ToTime().Format("%H%M")>="0057") && (NOW.ToTime().Format("%H%M")<="0103")){
  if(log){logObj.State("Beginn Nachtabschaltung");}
  if(DEBUG) {WriteLine("Beginn Nachtabschaltung");}

  !// Wandle die Raumliste um, sodass auch die multiRaumVariante berücksichtigt wird
  RVNListe = VarNamen;
  RVNListe=RVNListe.Replace("+",";");

  !// Laufe über alle Räume
  foreach(RVN,RVNListe.Split(";")) {
    !// Es ist möglich, dass eine Ressource keine Zuordnung hat
    if (!RVN){
      continue;
    }
    !// Raum Parameter bestimmen
    if(log){logObj.State("Gruppe:"+RVN);}
    if(DEBUG) {WriteLine("Gruppe:"+RVN);}
    RVI = dom.GetObject(RVN).State();
    HSFlag = RVI.StrValueByIndex(";",1);
    AGF = RVI.StrValueByIndex(";",2);

    !// Schaltparameter bestimmen
    AGFParam = "AGFParam"#AGF.ToUpper();
    Param = AGFParamIP;
    if (system.IsVar(AGFParam)){
      Param = system.GetVar(AGFParam);
    }
    if (HSFlag!="H"){
      Param="STATE";
    }

    !// Heizung oder Schaltung in jedem Fall zurücksetzen.
    !// Wir schalten auch aus, wenn der Raum auf heizen steht. Sollten wir in einem Heizzyklus sein
    !// wird die  nächste EINSCHALTEN Prüfung wieder schalten.
    aktuellerSchaltZustand = RVI.StrValueByIndex(";",0).ToInteger();

    !// AuchSchaltzustände wie Dauer EIN und Dauer AUS werden geprüft
    !// 0=Aus, 1=Ein, 2=Dauer AUS, 3=Dauer EIN
    !// Wir stellen die Wunschtemperaturen ein.

    !// Haben wir Schaltzustand 1 (eingeschaltet), gehen wir davon aus, dass wir noch
    !// einn Schaltbefehl ausführen und lassen den Eintrag.
    if (aktuellerSchaltZustand!=1){
      !// Bestimme den passenden Zustand für 0=Aus, 2=Dauer AUS, 3=Dauer EIN
      integer sollZustand = 1;
      if ((aktuellerSchaltZustand==0) || (aktuellerSchaltZustand==3)){
        !// Grundtemperatur individuell bestimmen
        GT=RVI.StrValueByIndex(";",3).StrValueByIndex("/",1).ToFloat();
        if(GT==0){
          GT=GTStandard;
        }
        RTemp = GT;
        sollZustand = 0;
      }

      !// AktorenListe aufbauen.
      iPos = 0;
      iEntry = 1;
      AktorenListe = "";
      while (iPos<RVI.Length()) {
        if (iEntry>6){
          AktorenListe = AktorenListe # RVI.Substr(iPos,1);
        } elseif (RVI.Substr(iPos,1)==";"){
          iEntry=iEntry+1;
        }
        iPos = iPos+1;
      }

      !// Aktoren schalten
      foreach(AktAktor,AktorenListe.Split(";")){
        objAktor = dom.GetObject(AktAktor);
        if (objAktor){
          objDP = objAktor.DPByHssDP(Param);
          if (objDP){
            if(HSFlag=="H"){
              real istTemperatur = objDP.State();
              if(istTemperatur==RTemp){
                objDP.State(RTemp);
                if(log){logObj.State(AktAktor+" Nachtschaltung für \"" #
                                     ("Aus;Ein;Dauer-Aus;Dauer-Ein").StrValueByIndex(";",aktuellerSchaltZustand) #
                                     "\" bereits gesetzt auf Temp.: "+RTemp.ToString(1));}
                if(DEBUG) {WriteLine(AktAktor+" Nachtschaltung für \"" #
                                     ("Aus;Ein;Dauer-Aus;Dauer-Ein").StrValueByIndex(";",aktuellerSchaltZustand) #
                                     "\" bereits gesetzt auf Temp.: "+RTemp.ToString(1));}
              }else{
                objDP.State(RTemp);
                if(log){logObj.State(AktAktor+" Nachtschaltung setzen für \"" #
                        ("Aus;Ein;Dauer-Aus;Dauer-Ein").StrValueByIndex(";",aktuellerSchaltZustand) #
                        "\" - Ist: " # istTemperatur.ToString(1) # " Soll: " # RTemp.ToString(1) # " Parameter: " # Param);}
                if(DEBUG) {WriteLine(AktAktor+" Nachtschaltung setzen für \"" #
                        ("Aus;Ein;Dauer-Aus;Dauer-Ein").StrValueByIndex(";",aktuellerSchaltZustand) #
                        "\" - Ist: " # istTemperatur.ToString(1) # " Soll: " # RTemp.ToString(1) # " Parameter: " # Param);}
              }
            }else{
              boolean istZustand = objDP.State(0)!=0;
              if (istZustand!=(sollZustand!=0)){
                objDP.State(sollZustand);
                if(log){logObj.State(AktAktor+" Nachtschaltung setzen für \"" #
                        ("Aus;Ein;Dauer-Aus;Dauer-Ein").StrValueByIndex(";",aktuellerSchaltZustand) #
                        "\" - Ist: " # istZustand # " Soll: " # sollZustand #". Parameter: " # Param);}
                if(DEBUG) {WriteLine(AktAktor+" Nachtschaltung setzen für \"" #
                        ("Aus;Ein;Dauer-Aus;Dauer-Ein").StrValueByIndex(";",aktuellerSchaltZustand) #
                        "\" - Ist: " # istZustand # " Soll: " # sollZustand #". Parameter: " # Param);}
              }
            }
          }else{
            if(log){logObj.State("Datenpunkt " # Param # " nicht vorhanden!");}
            if(DEBUG) {WriteLine("Datenpunkt " # Param # " nicht vorhanden!");}
          }            
        }else{
          if(log){logObj.State(AktAktor+" Objekt existiert nicht!");}
          if(DEBUG) {WriteLine(AktAktor+" Objekt existiert nicht!");}
        }
      }
    }
  }
  if(log){logObj.State("Ende Nachtabschaltung");}
  if(DEBUG) {WriteLine("Ende Nachtabschaltung");}
}

!// Schaltlistenprüfung----------------------------------------------

!// Code für die Prüfung Schaltliste. Alle Räume, die jetzt beheizt werden müssen auch
!// in der RVNListeAn enthalten sein.
RVNListe = VarNamen;
!// Wandle die Raumliste um, sodass auch die multiRaumVariante berücksichtigt wird
RVNListe=RVNListe.Replace("+",";");

!// Laufe über alle Räume und prüfe ob die sich im "An"-zustand befinden
foreach(RVN,RVNListe.Split(";")) {
  !// Es ist möglich, dass eine Ressource keine Zuordnung hat
  if (!RVN){
    continue;
  }
  !// Raum Parameter bestimmen
  RVI = dom.GetObject(RVN).State();
  HSFlag = RVI.StrValueByIndex(";",1);
  AGF = RVI.StrValueByIndex(";",2);

  !// Schaltparameter bestimmen
  AGFParam = "AGFParam"#AGF.ToUpper();
  Param = AGFParamIP;
  if (system.IsVar(AGFParam)){
    Param = system.GetVar(AGFParam);
  }
  if (HSFlag!="H"){
    Param="STATE";
  }

  !// Grundtemperatur individuell bestimmen
  GT=RVI.StrValueByIndex(";",3).StrValueByIndex("/",1).ToFloat();
  if(GT==0){
    GT=GTStandard;
  }

  !// Heizung oder Schaltung zurücksetzen, wenn diese sich immer noch im Heizen-/Schaltenzustand
  !// befindet und nicht in der Liste der beheizten/geschalteten Räume enthalten ist. Evtl. wurde
  !// ein Ausschaltpunkt versäumt oder der Termin wurde entfernt.
  aktuellerSchaltZustand = RVI.StrValueByIndex(";",0).ToInteger();
  if((RVNListeAn.Find(RVN+";")<0) && (aktuellerSchaltZustand==1)){
    !// Reset der Heizvariablen von 1 auf 0. Da die Heizliste leer ist, sollte der Eintrag
    !// in der Raumliste auch auf 0 stehen.
    dom.GetObject(RVN).State("0;"+RVI.Substr(2,RVI.Length()-2));
    if(log){logObj.State("Kein Schaltlisten Eintrag vorhanden für " # RVN # "! Heizung/Schaltung wird ausgeschaltet!");}
    if(DEBUG) {WriteLine("Kein Schaltlisten Eintrag vorhanden für " # RVN # "! Heizung/Schaltung wird ausgeschaltet!");}

    !// AktorenListe aufbauen.
    iPos = 0;
    iEntry = 1;
    AktorenListe = "";
    while (iPos<RVI.Length()) {
      if (iEntry>6){
        AktorenListe = AktorenListe # RVI.Substr(iPos,1);
      } elseif (RVI.Substr(iPos,1)==";"){
        iEntry=iEntry+1;
      }
      iPos = iPos+1;
    }

    !// Aktoren zurücksetzen
    foreach(AktAktor,AktorenListe.Split(";")){
      objAktor = dom.GetObject(AktAktor);
      if (objAktor){
        objDP = objAktor.DPByHssDP(Param);
        if (objDP){
          objDP.State(GT);
          if(HSFlag!="S"){
            real istTemperatur = objDP.State();
            objDP.State(GT);
            if(log){logObj.State("Kein Schaltlisten Eintrag vorhanden für " # RVN # "- Ist: " # istTemperatur.ToString(1) # " Neu: " # GT.ToString(1) #" Parameter: " # Param);}
            if(DEBUG) {WriteLine("Kein Schaltlisten Eintrag vorhanden für " # RVN # "- Ist: " # istTemperatur.ToString(1) # " Neu: " # GT.ToString(1) #" Parameter: " # Param);}
          }else{
            boolean istZustand = objDP.State()!=0;
            if (istZustand)
            {
              objDP.State(0);
              if(log){logObj.State("Kein Schaltlisten Eintrag vorhanden für " # RVN # "! Schalter - "+AktAktor+" ausschalten. Parameter: "+Param);}
              if(DEBUG) {WriteLine("Kein Schaltlisten Eintrag vorhanden für " # RVN # "! Schalter - "+AktAktor+" ausschalten. Parameter: "+Param);}
            }
          }
        }else{
          if(log){logObj.State("Datenpunkt " # Param # " nicht vorhanden!");}
          if(DEBUG) {WriteLine("Datenpunkt " # Param # " nicht vorhanden!");}
        }
      }else{
        if(log){logObj.State(AktAktor+" Objekt existiert nicht!");}
        if(DEBUG) {WriteLine(AktAktor+" Objekt existiert nicht!");}
      }
    }
  }
}

!// -----------------------------------------------------------------

if(log){logObj.State("Ende Schaltskriptlauf=============================");}
WriteLine("Ende Schaltskriptlauf");

