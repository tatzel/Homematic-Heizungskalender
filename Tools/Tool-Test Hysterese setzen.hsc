!// Mini-Test: Hysterese-Wert in Feld 3 einer Raumvariable setzen
!//================================================================================================
!// Stand:    21.09.2026
!// Autor:    Martin Richter    (heizkalender@m-ri.de) http://blog.m-ri.de/
!// Projekt:  Helmut Diedrichs  (helmut@diedrichs.de) https://diedrichs.de
!//================================================================================================
!// Setzt die raumspezifische Hysterese (dritter Slash-Teil in Feld 3) auf den gewuenschten Wert.
!// Feld 3: Wohlfuehltemp[/Grundtemp[/Hysterese]]
!// Wohlfuehltemp und Grundtemp bleiben unveraendert, alle anderen Felder ebenfalls.

!// Name der zu aendernden Raumvariable
string RaumName = "HKG-Raum-EG|Saal";

!// Neuer Hysterese-Wert in °C (leer "" loescht die Hysterese => Regelung aus)
string NeueHysterese = "0.5";

!// Neuer Schaltzustand (Feld 0): "" = unveraendert lassen
!//   0 = AUS, 1 = EIN, 2 = Dauer-AUS, 3 = Dauer-EIN
!// Zum Testen der Uebertemperatur-Abschaltung auf "1" setzen (Raum gilt als eingeschaltet).
string NeuerStatus = "0";

!//#######---Ende Variabler Bereich---#############################################################

object oRaum = dom.GetObject(RaumName);
if(!oRaum){
  WriteLine("FEHLER: Systemvariable \"" # RaumName # "\" existiert nicht.");
  quit;
}

string RVI = oRaum.State();
WriteLine("Vorher:  " # RVI);

!// Feld 3 neu zusammensetzen: Wohlfuehltemp[/Grundtemp]/Hysterese
string Feld3Alt = RVI.StrValueByIndex(";",3);
string sTemp = Feld3Alt.StrValueByIndex("/",0);
string sGT   = Feld3Alt.StrValueByIndex("/",1);

string Feld3Neu = sTemp;
if(NeueHysterese!=""){
  !// Mittelteil (Grundtemp) muss erhalten bleiben, ggf. als leerer Teil
  Feld3Neu = sTemp # "/" # sGT # "/" # NeueHysterese;
}elseif(sGT!=""){
  !// Hysterese loeschen, aber Grundtemp behalten
  Feld3Neu = sTemp # "/" # sGT;
}

!// Raumvariable feldweise neu aufbauen, Feld 0 (Status) und Feld 3 (Temp/Hysterese) ersetzen
string RVINeu = "";
integer i = 0;
string sFeld;
foreach(sFeld, RVI.Split(";")){
  if(i==0 && NeuerStatus!=""){ sFeld = NeuerStatus; }
  if(i==3){ sFeld = Feld3Neu; }
  if(i==0){ RVINeu = sFeld; } else { RVINeu = RVINeu # ";" # sFeld; }
  i = i+1;
}

oRaum.State(RVINeu);
WriteLine("Nachher: " # oRaum.State());
