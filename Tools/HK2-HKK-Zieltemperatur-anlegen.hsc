!// Optionale Systemvariable HK2-HKK-Zieltemperatur anlegen
!// Schwellwerte fuer die farbliche Bewertung der Zieltemperatur-Ereignisse
!// in der Heizkurvenkontrolle (Addon).
!//
!// Format: Kritisch-vor;Perfekt-von;Perfekt-bis;Kritisch-nach
!//   Gruen (perfekt):    Perfekt-von bis Perfekt-bis
!//   Gelb  (akzeptabel): Kritisch-vor bis Perfekt-von / Perfekt-bis bis Kritisch-nach
!//   Rot   (kritisch):   < Kritisch-vor / > Kritisch-nach
!//
!// Standard (Martins Vorgabe): -45;-20;5;15
!// Dieses Skript setzt:       -135;-60;-30;-15
!//
!//   Farbe  | Bewertung  | Delta min       | Bedeutung
!//   -------+------------+-----------------+------------------------------------------
!//   Gruen  | Perfekt    | -60 bis -30     | Zieltemp. 60 bis 30 min vor Termin erreicht
!//   Gelb   | Akzeptabel | -135..-60 / -30..-15 | Bis 75 min zu frueh oder bis 15 min vor Termin
!//   Rot    | Kritisch   | < -135 / > -15  | Alles andere
!//
!// Auf der CCU ausfuehren (z.B. ueber Programme/Skript testen).
!// Ueberschreibt eine bestehende Variable (loescht und legt neu an).

string svName = "HK2-HKK-Zieltemperatur";
string svValue = "-135;-60;-30;-15";
string svDesc = "Heizkurvenkontrolle Zieltemperatur: Kritisch-vor;Perfekt-von;Perfekt-bis;Kritisch-nach";

object sv = dom.GetObject(svName);
if (sv) {
  object svObjects = dom.GetObject(ID_SYSTEM_VARIABLES);
  svObjects.Remove(sv.ID());
  dom.DeleteObject(sv.ID());
  WriteLine("Variable '" # svName # "' geloescht.");
}

object svObjects = dom.GetObject(ID_SYSTEM_VARIABLES);
sv = dom.CreateObject(OT_VARDP);
sv.Name(svName);
svObjects.Add(sv.ID());
sv.ValueType(ivtString);
sv.ValueSubType(istChar8859);
sv.DPInfo(svDesc);
sv.State(svValue);
sv.Internal(false);
sv.Visible(true);
WriteLine("Variable '" # svName # "' angelegt mit Wert: " # svValue);
