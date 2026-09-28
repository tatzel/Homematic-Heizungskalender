!// Test: Beweist den False-Positive-Bug im Duplikat-Check des ChurchTools-Skripts
!//================================================================================================
!// Stand:    28.09.2026
!// Autor:    Martin Richter    (heizkalender@m-ri.de) http://blog.m-ri.de/
!// Projekt:  Helmut Diedrichs  (helmut@diedrichs.de) https://diedrichs.de
!//================================================================================================
!// Hintergrund:
!//   HK-Skript 1_ChurchTools.hsc prüft Duplikate mit SLT.Find(toadd).
!//   toadd beginnt mit der Ressource-ID, z.B. "1;2026-09-27 09:30:00;...".
!//   Ist eine zweistellige ID wie "11" bereits in SLT enthalten, findet
!//   SLT.Find("1;...") einen Treffer innerhalb von "11;..." → False-Positive.
!//   Folge: Ressource 1 (EG|Kinderraum) wird als vermeintliches Duplikat
!//   übersprungen und weder geheizt noch geloggt.
!//
!// Reproduziert mit realen Werten vom 27.09.2026:
!//   Ressource 11 (EG|Eltern-Kind-Raum) und Ressource 1 (EG|Kinderraum)
!//   haben identische Start- und Endzeiten.

!// Schaltliste wie sie nach Verarbeitung von Ressource 11 aussieht
string SLT = "11;2026-09-27 09:30:00;2026-09-27 12:00:00;0;HS;";

!// toadd für Ressource 1 (Kinderraum) - identische Zeiten
string toadd = "1;2026-09-27 09:30:00;2026-09-27 12:00:00;0;HS;";

WriteLine("SLT:   " # SLT);
WriteLine("toadd: " # toadd);
WriteLine("");

if(SLT.Find(toadd)<0){
  WriteLine("ERGEBNIS: OK - kein False-Positive, Ressource 1 wuerde eingetragen");
} else {
  WriteLine("ERGEBNIS: BUG BESTAETIGT - False-Positive! SLT.Find(toadd)=" # SLT.Find(toadd).ToString());
  WriteLine("  Ressource 1 wird faelschlicherweise als Duplikat von Ressource 11 erkannt.");
  WriteLine("  -> Kinderraum wird nicht in Schaltliste aufgenommen, kein Heizen, kein Log.");
}

WriteLine("");
WriteLine("--- Gegenprobe: Fix mit Semikolon-Praefix ---");
string SLTfix = ";" # SLT;
string toaddfix = ";" # toadd;
if(SLTfix.Find(toaddfix)<0){
  WriteLine("ERGEBNIS FIX: OK - mit Praefix kein False-Positive");
} else {
  WriteLine("ERGEBNIS FIX: Fix funktioniert nicht!");
}
