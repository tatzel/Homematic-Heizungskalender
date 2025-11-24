!// Wake On LAN für bestimmte PCs
!//================================================================================================
!// Stand:    23.11.2025; 
!// Autor:    Martin Richter    (heizkalender@m-ri.de)
!// Projekt:  Helmut Diedrichs
!//================================================================================================
!// Dieser Code wurde im Rahmen der Heizkalender-Implementierung der Baptisten Gemeinde Hanau 
!// entwicklet.
!// Die Nutzung ist kostenlos, aber wir bitten die Nutzung an einer der obigen Email Adressen zu 
!// melden.
!//================================================================================================
!//
!// Basiert auf: 
!//   https://homematic-forum.de/forum/viewtopic.php?f=19&t=16051&p=704919&hilit=wake+on+lan#p704919
!//

!// MAC vom BeamerPC    00:E0:4C:71:0B:E2
!// MAC meines Rechner  10:7C:61:07:A3:AB
string Devices = "00:E0:4C:71:0B:E2;10:7C:61:07:A3:AB";
string Device;

foreach(Device,Devices.Split(";")){
  if(Device!=""){
    system.Exec ("/usr/sbin/ether-wake " # Device # " &");
  }
}

WriteLine("Done");

