!// Wake On LAN für bestimmte PCs
!//================================================================================================
!// Stand:    25.11.2025; 
!// Autor:    Martin Richter    (heizkalender@m-ri.de)
!// Projekt:  Helmut Diedrichs
!//================================================================================================
!// Dieser Code wurde im Rahmen der Heizkalender-Implementierung der Baptisten Gemeinde Hanau 
!// entwickelt.
!// Das Heizkalender-Team freut sich, dass Sie den kostenlosen Heizkalender anwenden und somit 
!// einen Beitrag zum Umweltschutz leisten. Und es wäre schön, wenn Sie die Nutzung per E-Mail an 
!// Info@Heizkalender.de melden. Dadurch ergäbe ich eine Übersicht und zudem die Möglichkeit auf 
!// wichtige Änderungen hinzuweisen. Gerne können Sie auch über Ihre Erfahrung mit dem Heizkalender 
!// berichten.
!//================================================================================================
!//
!// Basiert auf: 
!//   https://homematic-forum.de/forum/viewtopic.php?f=19&t=16051&p=704919&hilit=wake+on+lan#p704919
!//

!// MAC vom BeamerPC    00:E0:4C:71:0B:E2
!// MAC meines Rechner  10:7C:61:07:A3:AB
string Devices = "00:E0:4C:71:0B:E2;10:7C:61:07:A3:AB";

!//#######---Ende Variabler Bereich---#############################################################
!//Im Folgenden Hier keine Veränderungen vornehmen!


string Device;

foreach(Device,Devices.Split(";")){
  if(Device!=""){
    system.Exec ("/usr/sbin/ether-wake " # Device # " &");
  }
}

WriteLine("Done");

