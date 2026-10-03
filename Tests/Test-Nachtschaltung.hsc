!// Test-Nachtschaltung.hsc
!// Stand: 03.10.2026
!// Zweck: Nachtschaltungs-Block aus HK-Skript 2 isoliert testen, ohne Zeitfenster-Bedingung.
!// Ausfuehren ueber "Skript testen" auf der CCU3 -- KEIN Produktivskript, nicht installieren.
!//
!// Was dieser Test tut:
!//   - Liest dieselben Systemvariablen wie HK-Skript 2
!//   - Fuehrt den Nachtschaltungs-Block direkt aus (kein Zeitfenster-Check)
!//   - Logging zwingend EIN (log=1), DEBUG zwingend EIN
!//   - Schaltet KEINE Aktoren (SIMULATION=1 verhindert State()-Schreibzugriffe)
!//
!// Erwartet im Log (HK2-Log): pro Gruppe eine "Nachtschaltung pruefen"-Meldung,
!// bei Schaltzustand != 1 zusaetzlich "Nachtschaltung setzen fuer ..."-Meldungen.

!//---Variabler Bereich---
string vrp="";
boolean DEBUG=1;
integer log=1;

!// SIMULATION=1: Aktoren werden NICHT geschaltet, nur geloggt
boolean SIMULATION=1;
!//---Ende variabler Bereich---

!// Schaltparameter
string AGFParamIP="SET_POINT_TEMPERATURE";
string AGFParamRT="SET_TEMPERATURE";
string AGFParamTC="SETPOINT";
string AGFParamIT="SET_TEMPERATURE";
string AGFParamSW="STATE";

!// Systemvariablen laden
string VarNamen=dom.GetObject(vrp#"HK2-HKG-Liste").State();
real GTStandard=dom.GetObject(vrp#"HK2-Grundtemperatur").State().ToFloat();
real GT=GTStandard;
real RTemp=GTStandard;
string AGF;
string AGFParam;
string Param;
string AktorenListe;
object objAktor;
object objDP;
string HSFlag;
integer aktuellerSchaltZustand;

!// Logging vorbereiten
var logObj=dom.GetObject(vrp#"HK2-Log");
if (!logObj){
  log = false;
  WriteLine("WARNUNG: HK2-Log nicht gefunden, nur WriteLine-Ausgabe!");
}

WriteLine("=== Test-Nachtschaltung Start ===");
WriteLine("VarNamen=" # VarNamen);
WriteLine("GTStandard=" # GTStandard.ToString(1));

!// Nachtschaltungs-Block (identisch zu HK-Skript 2, ohne Zeitfenster-Bedingung)
if(log){logObj.State("Beginn Nachtabschaltung=============================");}
WriteLine("Beginn Nachtabschaltung=============================");

string RVNGruppe;
foreach(RVNGruppe,VarNamen.Split(";")) {
  if (!RVNGruppe){ continue; }

  !// Primaeren Raumnamen ermitteln
  string RVNPrimaer = RVNGruppe.StrValueByIndex("+",0);
  string RVNPrimaerName = RVNPrimaer.Replace(vrp#"HKG-Raum-","");

  !// Gruppenname aufbauen
  string GruppeLogName = RVNPrimaerName;
  string RVNZusatz;
  foreach(RVNZusatz,RVNGruppe.Split("+")) {
    if (!RVNZusatz){ continue; }
    string RVNZusatzName = RVNZusatz.Replace(vrp#"HKG-Raum-","");
    if (RVNZusatzName!=RVNPrimaerName){
      if (GruppeLogName==RVNPrimaerName){
        GruppeLogName = RVNPrimaerName # " (inkl. " # RVNZusatzName;
      }else{
        GruppeLogName = GruppeLogName # ", " # RVNZusatzName;
      }
    }
  }
  if (GruppeLogName!=RVNPrimaerName){ GruppeLogName = GruppeLogName # ")"; }
  if(log){logObj.State(GruppeLogName # ": Nachtschaltung pruefen");}
  WriteLine(GruppeLogName # ": Nachtschaltung pruefen");

  !// Laufe ueber alle Raeume der Gruppe
  string RVN;
  foreach(RVN,RVNGruppe.Split("+")) {
    if (!RVN){ continue; }
    string RVNName = RVN.Replace(vrp#"HKG-Raum-","");
    string RaumLogName = RVNPrimaerName;
    if (RVNName!=RVNPrimaerName){
      RaumLogName = RVNPrimaerName # " (inkl. " # RVNName # ")";
    }

    !// Raum-Parameter bestimmen
    string RVI = dom.GetObject(RVN).State();
    if (!RVI){
      WriteLine(RaumLogName # ": Raumvariable " # RVN # " nicht gefunden!");
      continue;
    }
    HSFlag = RVI.StrValueByIndex(";",1);
    AGF = RVI.StrValueByIndex(";",2);

    AGFParam = "AGFParam"#AGF.ToUpper();
    Param = AGFParamIP;
    if (system.IsVar(AGFParam)){
      Param = system.GetVar(AGFParam);
    }
    if (HSFlag!="H"){
      Param="STATE";
    }

    aktuellerSchaltZustand = RVI.StrValueByIndex(";",0).ToInteger();
    WriteLine(RaumLogName # ": Schaltzustand=" # aktuellerSchaltZustand # " HSFlag=" # HSFlag # " AGF=" # AGF);

    if (aktuellerSchaltZustand!=1){
      integer sollZustand = 1;
      if ((aktuellerSchaltZustand==0) || (aktuellerSchaltZustand==3)){
        GT=RVI.StrValueByIndex(";",3).StrValueByIndex("/",1).ToFloat();
        if(GT==0){
          GT=GTStandard;
        }
        RTemp = GT;
        sollZustand = 0;
      }

      !// AktorenListe aufbauen
      integer iPos = 0;
      integer iEntry = 1;
      integer iMaxEntry=6;
      if (HSFlag=="HS"){
        iMaxEntry = iMaxEntry+1;
      }
      AktorenListe = "";
      while (iPos<RVI.Length()) {
        if (iEntry>iMaxEntry){
          AktorenListe = AktorenListe # RVI.Substr(iPos,1);
        } elseif (RVI.Substr(iPos,1)==";"){
          iEntry=iEntry+1;
        }
        iPos = iPos+1;
      }
      WriteLine(RaumLogName # ": AktorenListe=[" # AktorenListe # "] RTemp=" # RTemp.ToString(1) # " Param=" # Param);

      string AktAktor;
      foreach(AktAktor,AktorenListe.Split(";")){
        objAktor = dom.GetObject(AktAktor);
        if (objAktor){
          objDP = objAktor.DPByHssDP(Param);
          if (objDP){
            if(HSFlag=="H"){
              real istTemperatur = objDP.State();
              if(istTemperatur!=RTemp){
                if(SIMULATION){
                  if(log){logObj.State(RaumLogName # ": " # AktAktor # " [SIM] Nachtschaltung setzen fuer \"" #
                          ("Aus;Ein;Dauer-Aus;Dauer-Ein").StrValueByIndex(";",aktuellerSchaltZustand) #
                          "\" - Ist: " # istTemperatur.ToString(1) # " Soll: " # RTemp.ToString(1) # " Parameter: " # Param);}
                  WriteLine(RaumLogName # ": " # AktAktor # " [SIM] wuerde setzen: " # RTemp.ToString(1) # " Grad");
                }else{
                  objDP.State(RTemp);
                  if(log){logObj.State(RaumLogName # ": " # AktAktor # " Nachtschaltung setzen fuer \"" #
                          ("Aus;Ein;Dauer-Aus;Dauer-Ein").StrValueByIndex(";",aktuellerSchaltZustand) #
                          "\" - Ist: " # istTemperatur.ToString(1) # " Soll: " # RTemp.ToString(1) # " Parameter: " # Param);}
                  WriteLine(RaumLogName # ": " # AktAktor # " gesetzt auf " # RTemp.ToString(1));
                }
              }else{
                WriteLine(RaumLogName # ": " # AktAktor # " bereits korrekt (" # istTemperatur.ToString(1) # " Grad)");
              }
            }else{
              boolean istZustand = objDP.State()!=0;
              if (istZustand!=(sollZustand!=0)){
                if(SIMULATION){
                  if(log){logObj.State(RaumLogName # ": " # AktAktor # " [SIM] Nachtschaltung setzen fuer \"" #
                          ("Aus;Ein;Dauer-Aus;Dauer-Ein").StrValueByIndex(";",aktuellerSchaltZustand) #
                          "\" - Ist: " # ("aus;ein").StrValueByIndex(";",istZustand.ToInteger()) # " Soll: " # ("aus;ein").StrValueByIndex(";",sollZustand) # ". Parameter: " # Param);}
                  WriteLine(RaumLogName # ": " # AktAktor # " [SIM] wuerde schalten: Ist=" # ("aus;ein").StrValueByIndex(";",istZustand.ToInteger()) # " Soll=" # ("aus;ein").StrValueByIndex(";",sollZustand));
                }else{
                  objDP.State(sollZustand);
                  if(log){logObj.State(RaumLogName # ": " # AktAktor # " Nachtschaltung setzen fuer \"" #
                          ("Aus;Ein;Dauer-Aus;Dauer-Ein").StrValueByIndex(";",aktuellerSchaltZustand) #
                          "\" - Ist: " # ("aus;ein").StrValueByIndex(";",istZustand.ToInteger()) # " Soll: " # ("aus;ein").StrValueByIndex(";",sollZustand) # ". Parameter: " # Param);}
                  WriteLine(RaumLogName # ": " # AktAktor # " geschaltet auf " # ("aus;ein").StrValueByIndex(";",sollZustand));
                }
              }else{
                if(log){logObj.State(RaumLogName # ": " # AktAktor # " Nachtschaltung bereits korrekt: Zustand=" # ("aus;ein").StrValueByIndex(";",istZustand.ToInteger()));}
                WriteLine(RaumLogName # ": " # AktAktor # " Nachtschaltung bereits korrekt: Zustand=" # ("aus;ein").StrValueByIndex(";",istZustand.ToInteger()));
              }
            }
          }else{
            if(log){logObj.State(RaumLogName # ": Datenpunkt " # Param # " nicht vorhanden!");}
            WriteLine(RaumLogName # ": Datenpunkt " # Param # " nicht vorhanden!");
          }
        }else{
          if(log){logObj.State(RaumLogName # ": " # AktAktor # " Objekt existiert nicht!");}
          WriteLine(RaumLogName # ": " # AktAktor # " Objekt existiert nicht!");
        }
      }
    }else{
      WriteLine(RaumLogName # ": Schaltzustand=1 (aktiv), wird uebersprungen");
    }
  }
}

if(log){logObj.State("Ende Nachtabschaltung=============================");}
WriteLine("Ende Nachtabschaltung=============================");
WriteLine("=== Test-Nachtschaltung Ende ===");
