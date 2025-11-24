!//################################################################################################################################################################
!//Event orientiertes Heizen/Schalten Version 1.0.2 / 23.10.2025 Lukas Helduser (Youtube: https://www.youtube.com/LukasvandeHaag)
!//Teil 4 Skript zum auslesen der Temperature vom Wetterdienst DWD
!//Erarbeitung der Grundlage der DWD API Jesko Stolp / Olaf Hoffmann Schleswig

!// hier ist er Link zu den Ortskennungen (Stations-ID):
!//Https://opendata.dwd.de/climate_environment/CDC/observations_germany/climate/daily/kl/recent/KL_Tageswerte_Beschreibung_Stationen.txt


!//Eingabe eines Namens Präfix
!//Dies ist nur erforderlich wenn die Namensvorgabe abgeändert werden soll.
!//Wird hier ein Präfix eingeben so muss dieser in allen Skripten auch angegeben werden.
string vrp="";

!//#######---Am Skript Inhalt keine Veränderung vornehmen##########################################################################################################


!//Variablen
string error="kein";
string command;
string stemp;
string ort=dom.GetObject(vrp+"HK2-Location").State();

command = "wget --timeout=3 -O - 'https://dwd.api.proxy.bund.dev/v30/stationOverviewExtended?stationIds="+ort+"'";
system.Exec(command, &stemp, &error);


string p=stemp.Substr(stemp.Find("\"days\":[{\"stationId"),100);
string min=p.Substr(p.Find("temperatureMin")+16,4);
string max=p.Substr(p.Find("temperatureMax")+16,4);

min=min.Replace(",","").Replace("\"","").Replace("t","");
max=max.Replace(",","").Replace("\"","").Replace("t","");


var tempmin = 0.0 + min.ToInteger();
var fmin=tempmin/10.0;
var tempmax = 0.0 + max.ToInteger();
var fmax=tempmax/10.0;

!WriteLine(fmin+"\n");
!WriteLine(fmax+"\n");

dom.GetObject(vrp+"HK2-Aussentemperatur").State(fmax);
