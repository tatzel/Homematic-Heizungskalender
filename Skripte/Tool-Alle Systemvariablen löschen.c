!// Skript zum Erstellen der Systemvariablen des Heizkalender für das Logging
!//================================================================================================
<<<<<<< Updated upstream
!// Stand:    08.12.2025;
=======
!// Stand:    16.12.2025; 
>>>>>>> Stashed changes
!// Autor:    Martin Richter    (heizkalender@m-ri.de) http://blog.m-ri.de/
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

!//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!// ACHTUNG DIESES SKRIPT SOLLTE NUR GANZ BEWUSST EINGESETZT WERDEN!!!!
!// ES KÖNTEN  WICHTIGE DATEN GELÖSCHT WERDEN!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

!// Leerer Prefix mit einem Blank " ", löscht alle Variablen mit "HK" oder "Tool-".
!// Andere Prefixe ChurchDesk cd_, Prefix ChurchTools ct_
string vrps=" ;cd_;ct_";

!//################################################################################################
!//######------Skript Variablen und Skript Arbeitsteil. Vom Benutzer nicht zu verändern------######

!// Skript zum löschen von SystemVariablen mit bestimmten Prefixen.
string svName;
object svObject;

string vrp="";
foreach(vrp,vrps.Split(";")){
  vrp=vrp.Trim();
  WriteLine("Prefix:" #vrp);
  foreach(svName, dom.GetObject(ID_SYSTEM_VARIABLES).EnumUsedNames()){
    !WriteLine(svName);
    if (svName.ToUpper().StartsWith(vrp.ToUpper()#"HK") || svName.ToUpper().StartsWith(vrp.ToUpper()#"Tool-")){
      svObject = dom.GetObject(ID_SYSTEM_VARIABLES).Get(svName);
      dom.DeleteObject(svObject);
      WriteLine("Systemvariable \""#svName#"\" gelöscht");
    }
  }
}
<<<<<<< Updated upstream
=======

WriteLine("Alles fertig...");
>>>>>>> Stashed changes
