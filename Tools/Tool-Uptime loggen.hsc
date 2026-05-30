!// Im Systemprotokoll die Uptime der CCU anzeigen
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

!//Eingabe eines Namens Präfix
!//Dies ist nur erforderlich wenn die Namensvorgabe beim erstellen den Systemvariablen geändert wurde.
!//Wird hier ein Präfix eingeben so muss dieser in allen Skripten auch angegeben werden.
string vrp="";

!//Logging in "Log" mit 1 zwingend einschalten oder mit 0 Ausschalten
boolean log=0;

!//#######---Ende Variabler Bereich---#############################################################
!//Im Folgenden Hier keine Veränderungen vornehmen!

string stdout;
string stderr;
system.Exec("uptime",&stdout,&stderr);

var logObj=dom.GetObject(vrp+"HK-Log");
var loggingObj=dom.GetObject(vrp+"HK-Logging");

!// Prüfe logging erwartet wird
if ((!log) && loggingObj){
  if (loggingObj.State()!=0){
	  log = true;
  }
}
	
!// Logging auschalten, wenn keine Variable vorhanden
if (!logObj){
	log = false;
}

if(log){
  logObj.State(stdout.Trim());
}

WriteLine(stdout.Trim());
