!// Bestimmen der Außentemperatur für den Heizkalender
!//================================================================================================
!// Stand:    21.01.2025;
!// Autor:    Martin Richter    (heizkalender@m-ri.de) http://blog.m-ri.de/
!// Projekt:  Helmut Diedrichs  (helmut@diedrichs.de) https://diedrichs.de
!//------------------------------------------------------------------------------------------------
!// Copyright (C) 2026 Martin Richter (xMRi-Software)
!// Dieser Teil des Heizkalenders ist freie Software und wird unter der GNU General Public License
!// Version 3 (GPLv3) oder neuer veröffentlicht.
!// Es besteht keinerlei Garantie oder Haftung. Nutzung auf eigene Verantwortung.
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
!// die nächsten 24h und die Daten der letzten 48h. 
!// Die Geokoordinaten selbst werden aus den Daten der CCU3 ausgelesen.
!// In dieser Variante ist es möglich eine Durchschnittstemperatur zu nutzen, die dadurch
!// den Temperaturverlauf nach oben/unten abdämpft. Die Anzahl der Stunden kann zwischen 
!// 1h und 48h eingestellt werden.

!//Eingabe eines Namens Präfix
!//Dies ist nur erforderlich wenn die Namensvorgabe abgeändert werden soll.
!//Wird hier ein Präfix eingeben so muss dieser in allen Skripten auch angegeben werden.
string vrp="";

!//Debug Ausgaben Ein und Aus schalten. 0 = Aus, 1 = Ein
boolean DEBUG=0;

!// Max Stunden in die Vergangenheit für die die Durchschnittstemperatur ermittelt werden soll
!// Maximal dürfen hier 48h eingegeben werden. Über diese Zeit wird ein gleitender Durchschniit
!// berechnet.
integer stundenZurueck = 36;

!//#######---Ende Variabler Bereich---#############################################################
!//Im Folgenden Hier keine Veränderungen vornehmen!

!//Variablen
string error="kein";
string command;
string stemp;
string lat=system.Latitude().ToFloat();
string lon=system.Longitude().ToFloat();

if (DEBUG){
  WriteLine(lat+"\n");
  WriteLine(lon+"\n");
}

!// Temperaturwerte lesen 48h davor, 24h (heute) in die Zukunft.
command = "wget --timeout=3 -O - 'https://api.open-meteo.com/v1/forecast?latitude="+lat.ToString()+"&longitude="+lon.ToString()+"&hourly=temperature_2m&past_days=2&forecast_days=1'";
system.Exec(command, &stemp, &error);
if (DEBUG){
  !WriteLine(command+"\n");
  !WriteLine(stemp+"\n");
}

!// passenden eintrag finden
integer pos=stemp.Find("\"temperature_2m\":[");
stemp=stemp.Substr(pos+18,1000);
pos=stemp.Find("]");
stemp=stemp.Substr(0,pos);
if (DEBUG){
  WriteLine(stemp+"\n");
}

!// Alle Einträge summieren und mittelwert bilden
string temp;
real summe=0.0;
integer n=0;

!// Bestimmen ab wann wir von den alten Daten Teperaturen übernehmen.
!// Wir nehmen exakt 48h rückwärts zur aktuellen Uhrzeit
integer h = system.Date("%H").ToInteger();
if (h==0){
  !// Wir beginnen am neuen Tag um 00:00 Uhr
  h = 24;
}

stundenZurueck = stundenZurueck.Min(48).Max(1);
integer ueberspringen = h+48-stundenZurueck;
if (DEBUG){
  WriteLine("Stunden zurück: " # stundenZurueck);
  WriteLine("Überspringen: " # ueberspringen);
  WriteLine("---------");

  integer i=0;
  foreach (temp, stemp.Split(","))
  {
    WriteLine((i % 24) # ": "# temp);
    i = i+1;
  }
  WriteLine("---------");
}


integer i=0;
foreach (temp, stemp.Split(","))
{
  !// Werte bis zur aktuellen Stunde überspringen wir.
  if ((i>h) && i>ueberspringen){
    if (DEBUG){
      WriteLine((i % 24) # ": "# temp);
    }
    summe = summe+temp.ToFloat();
    n = n+1;
    if (n>=stundenZurueck) {
       break;
    }
  }
  i = i+1;
}
if (DEBUG){
  WriteLine("---------");
  WriteLine(summe);
  WriteLine(n);
  WriteLine((temp/n)+"\n");
}
temp = summe/n;

!// Ergebnis schreiben
if (n!=0){
  dom.GetObject(vrp+"HK2-Aussentemperatur").State(temp.ToString(1));
}
