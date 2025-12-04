!// Reboot der HomeMatic CCU3 mit sichern des Systemprotokolls
!//================================================================================================
!// Stand:    25.11.2025; 
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
!//
!// Quelle für Reboot Code auf der CCU3:
!//   https://www.christian-luetgens.de/homematic/programmierung/tools/neustart/Neustart.htm
!//

string vrp="";

!// Name des Programmes, das das Systemprotokoll sichert
string SystemProtokollSichern = "HK-Systemprotokoll sichern";

!//Logging in "Log" mit 1 zwingend einschalten oder mit 0 Ausschalten
boolean log=0;

!//#######---Ende Variabler Bereich---#############################################################
!//Im Folgenden Hier keine Veränderungen vornehmen!

var logObj=dom.GetObject(vrp+"HK1-Log");
var loggingObj=dom.GetObject(vrp+"HK1-Logging");

!// Einträge in die Logs zuerst schreiben. Ist sinnlos, wenn das System Protokoll
!// nicht auf USB geschrieben wird.
string logs="HK;HK1;HK2";
string log;
if(logging){
  foreach(log,logs.Split(";")){
    var logObj=dom.GetObject(vrp# log # "-Log");
    var loggingObj=dom.GetObject(vrp # log # "-Logging");
    if (logObj && loggingObj){
      if (loggingObj.State()!=0){
        logObj.State("Reboot");
      }
    }
  }
}

!// Bevor wir den Reboot durchführen sichern wir das Systemprotokoll, wenn das aktiv ist
!// Quelle: https://homematic-forum.de/forum/viewtopic.php?t=4211
!//         https://homematic-forum.de/forum/viewtopic.php?t=37907
var pr = dom.GetObject(SystemProtokollSichern);
if (pr && pr.Active()){
  pr.ProgramExecute();
}

!// Warte für 5 Sekunden, damit das Skript gespeichert wird.
integer Start = system.Date("%s").ToInteger();
while((system.Date("%s").ToInteger()-Start)<5){
  !// Warte
}

!// Jetzt der Reboot
string stdout;
string stderr;
system.Save();
!// Wir warten bis der Befehl ausgeführt wird
system.Exec ("/sbin/reboot &");