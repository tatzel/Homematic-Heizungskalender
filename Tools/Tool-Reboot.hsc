!// Reboot der HomeMatic CCU3 mit sichern des Systemprotokolls
!//================================================================================================
!// Stand:    04.03.2026
!// Autor:    Martin Richter    (heizkalender@m-ri.de) http://blog.m-ri.de/
!// Projekt:  Helmut Diedrichs  (helmut@diedrichs.de) https://diedrichs.de
!//------------------------------------------------------------------------------------------------
!// Copyright (C) 2026 Martin Richter (xMRi-Software)
!// Dieser Teil des Heizkalenders ist freie Software und wird unter der GNU General Public License 
!// Version 3 (GPLv3) oder neuer veröffentlicht.
!// Es besteht keinerlei Garantie oder Haftung. Nutzung auf eigene Verantwortung.
!//================================================================================================
!//
!// Quelle für Reboot Code auf der CCU3:
!//   https://www.christian-luetgens.de/homematic/programmierung/tools/neustart/Neustart.htm
!//

!//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!//Sicherheitsfunktion die entfernt werden muss für die Ausführung!
quit;
!//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

!// Evtl. mehrere Prefixe. Leerer Prefix muss mit einer Leertaste gesetzt sein.
string vrps=" ";

!// Name des Programme, die vor dem Neustart ausgeführt werden sollen.
!// Bei uns muß das Systemprotokoll gesichert werden.
string listeProgramme = "HK-Systemprotokoll sichern;HK-CloudMatic Diagramm Daten sichern";

!//Logging in "Log" mit 1 zwingend einschalten oder mit 0 Ausschalten
boolean log=true;

!//#######---Ende Variabler Bereich---#############################################################
!//Im Folgenden Hier keine Veränderungen vornehmen!


!// Einträge in die Logs zuerst schreiben. Ist sinnlos, wenn das System Protokoll
!// nicht auf USB geschrieben wird.
string logs="HK;HK1;HK2";
string logPrefix;
string vrp;
foreach(vrp,vrps.Split(";")){
  vrp=vrp.Trim();
  foreach(logPrefix,logs.Split(";")){
    var logObj=dom.GetObject(vrp# logPrefix # "-Log");
    var loggingObj=dom.GetObject(vrp # logPrefix # "-Logging");
    if (logObj && loggingObj){
      if (loggingObj.State()!=0){
      	WriteLine("log");
        logObj.State("Reboot");
      }
    }
  }
}

!// System auf festplatte sichern
system.Save();

!// Bevor wir den Reboot durchführen sichern wir das Systemprotokoll, wenn das aktiv ist
!// Quelle: https://homematic-forum.de/forum/viewtopic.php?t=4211
!//         https://homematic-forum.de/forum/viewtopic.php?t=37907
string programm;
foreach(programm,listeProgramme.Split(";")){
  var objpr = dom.GetObject(programm);
  if (objpr && objpr.Active()){
    objpr.ProgramExecute();
  }
}

!// Jetzt der Reboot verzögert um 60sec.
string stdout;
string stderr;

!// System auf festplatte sichern
system.Save();

!// Wir warten bis der Befehl ausgeführt wird
system.Exec ("/sbin/reboot -d 60&");