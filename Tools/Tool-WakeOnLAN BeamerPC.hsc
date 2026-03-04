!// Wake On LAN für bestimmte PCs
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

