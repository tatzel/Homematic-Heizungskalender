!//################################################################################################################################################################
!//Event orientiertes Heizen/Schalten Version MRi/1.0.2 / 23.10.2025 (MRi 12.11.2025) Lukas Helduser (Youtube: https://www.youtube.com/LukasvandeHaag)
!//Ergänzungen von Martin Richter (MRi)
!//Teil 4 Skript zum auslesen der Temperature vom Wetterdienst DWD über Open-Meteo

!//MRi: 2025-11-12	Grundlegende Version mit der Nutzung der Geokoordinaten aus der CCU3

!// Doku siehe hier https://open-meteo.com/en/docs/dwd-api?forecast_days=1
!// Dieser Aufruf liefert eine Temperatur Prognose für die entsprechenden Geokoordinaten für 
!// die nächsten 24h.

!//Eingabe eines Namens Präfix
!//Dies ist nur erforderlich wenn die Namensvorgabe abgeändert werden soll.
!//Wird hier ein Präfix eingeben so muss dieser in allen Skripten auch angegeben werden.
string vrp="";

!//#######---Am Skript Inhalt keine Veränderung vornehmen##########################################################################################################

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

