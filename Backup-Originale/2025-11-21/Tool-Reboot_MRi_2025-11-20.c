!// Reboot der HomeMatic CCU3
!// Stand: 20.11.2025; 
!// Autor: Martin Richter, Projekt: Helmut Diedrichs
!// Quelle: https://www.christian-luetgens.de/homematic/programmierung/tools/neustart/Neustart.htm
string vrp="";

!// Einträge in die Logs zuerst schreiben. Ist sinnlos, wenn das System Protokoll
!// nicht auf USB geschrieben wird.
string logs="HK1;HK2";
string log;
foreach(log,logs.Split(";")){
  var logObj=dom.GetObject(vrp# log # "-Log");
  var loggingObj=dom.GetObject(vrp # log # "-Logging");
  if (loggingObj){
    if (loggingObj.State()!=0){
      logObj.State("Reboot");
    }
  }
}

!// Bevor wir den Reboot durchführen sichern wir das Systemprotokoll, wenn das aktiv ist
!// Quelle: https://homematic-forum.de/forum/viewtopic.php?t=4211
!//         https://homematic-forum.de/forum/viewtopic.php?t=37907
var pr = dom.GetObject("HK-SystemProtokoll speichern");
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