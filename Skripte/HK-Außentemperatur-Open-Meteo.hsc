!// Bestimmen der Außentemperatur für den Heizkalender
!//================================================================================================
!// Stand:    22.11.2025; 
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
!// Doku siehe hier https://open-meteo.com/en/docs/dwd-api?forecast_days=1
!//
!// Dieser Aufruf liefert eine Temperatur Prognose für die entsprechenden Geokoordinaten für 
!// die nächsten 24h. Die Geokoordinaten selbst werden aus den Daten der CCU3 ausgelesen.
!//

!//Eingabe eines Namens Präfix
!//Dies ist nur erforderlich wenn die Namensvorgabe abgeändert werden soll.
!//Wird hier ein Präfix eingeben so muss dieser in allen Skripten auch angegeben werden.
string vrp="";

!//#######---Ende Variabler Bereich---#############################################################
!//Im Folgenden Hier keine Veränderungen vornehmen!

!//Variablen
string error="kein";
string command;
string stemp;
string lat=system.Latitude().ToFloat();
string lon=system.Longitude().ToFloat();

!WriteLine(lat+"\n");
!WriteLine(lon+"\n");

command = "wget --timeout=3 -O - 'https://api.open-meteo.com/v1/forecast?latitude="+lat.ToString()+"&longitude="+lon.ToString()+"&hourly=temperature_2m&forecast_days=1'";
!WriteLine(command+"\n");
system.Exec(command, &stemp, &error);
!WriteLine(stemp+"\n");

!// passenden eintrag finden
integer pos=stemp.Find("\"temperature_2m\":[");
stemp=stemp.Substr(pos+18,1000);
pos=stemp.Find("]");
stemp=stemp.Substr(0,pos);
!WriteLine(stemp+"\n");

!// Eintrag anhand der Uhrzeit finden
string temp=stemp.StrValueByIndex(",",system.Date("%H").ToInteger()).ToFloat();
WriteLine(temp+"\n");

!// Ergebnis schreiben
if (temp!=""){
  dom.GetObject(vrp+"HK2-Aussentemperatur").State(temp);
}

