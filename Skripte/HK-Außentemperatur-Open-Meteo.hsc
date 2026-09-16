!// Bestimmen der Außentemperatur für den Heizkalender
!//================================================================================================
!// Stand:    16.09.2026
!// Autor:    Martin Richter    (heizkalender@m-ri.de) http://blog.m-ri.de/
!// Projekt:  Helmut Diedrichs  (helmut@diedrichs.de) https://diedrichs.de
!//------------------------------------------------------------------------------------------------
!// Copyright (C) 2026 Martin Richter (xMRi-Software)
!// Dieser Teil des Heizkalenders ist freie Software und wird unter der GNU General Public License
!// Version 3 (GPLv3) oder neuer veröffentlicht.
!// Es besteht keinerlei Garantie oder Haftung. Nutzung auf eigene Verantwortung.
!//================================================================================================
!//
!// Doku siehe hier https://open-meteo.com/en/docs/dwd-api?forecast_days=1
!//
!// Dieser Aufruf liefert eine Temperatur Prognose für die entsprechenden Geokoordinaten für
!// die nächsten 24h und die Daten der letzten 48h.
!// Die Geokoordinaten selbst werden aus den Daten der CCU3 ausgelesen.
!// In dieser Variante ist es möglich eine Durchschnittstemperatur zu nutzen, die dadurch
!// den Temperaturverlauf nach oben/unten abdämpft. Die Anzahl der Stunden kann zwischen
!// 1h und 48h eingestellt werden für die Vergangenheit und 24h für die Prognose in die Zukunft
!// angegeben werden.
!//
!// Das Skript sollte jede volle Stunde laufen

!// TT: 2026-09-16 Robustheit verbessert: pos<0-Prüfung nach Find() eingebaut, verhindert
!//                falsches Parsen bei fehlgeschlagener/unvollständiger wget-Antwort.
!//                lat/lon: unnötige ToFloat()-Konversion und redundante ToString()-Aufrufe entfernt.

!//Eingabe eines Namens Präfix
!//Dies ist nur erforderlich wenn die Namensvorgabe abgeändert werden soll.
!//Wird hier ein Präfix eingeben so muss dieser in allen Skripten auch angegeben werden.
string vrp="";

!//Debug Ausgaben Ein und Aus schalten. 0 = Aus, 1 = Ein
boolean DEBUG=0;

!// Max Stunden in die Vergangenheit für die die Durchschnittstemperatur ermittelt werden soll
!// Maximal dürfen hier 48h eingegeben werden. Und ebenso die Voraussage für die nächsten Stunden,
!// es können maximal 24h voraus berücksichtigt werden. Über diese komplette Anzahl von Stunden
!// wird der Durchschnitt berechnet.
!// Laut Chat/GPT ist 18/6 ein guter Wert für Fussbodenheizungen. Bei normalen Heizkörpern bietet sich
!// 12/3 an. Das thermische Gedächtnis von Häusern liegt bei ca. 10h.
!// Da unsere Termine selten länger als 3h sind wird die Vorheizzeit kaum von der Zukunft beeinflusst
integer stundenZurueck = 18;    !// maximal 48h
integer stundenVoraus = 6;      !// Maximal 24h

!// Logging in "Log" mit 1 zwingend einschalten oder mit -1 zwingend Ausschalten
!// Mit 0 wird die Einstellunge aus der HKx-Logging übernommen
integer log=0;

!//#######---Ende Variabler Bereich---#############################################################
!//Im Folgenden Hier keine Veränderungen vornehmen!

!// Logging vorbereiten
var logObj=dom.GetObject(vrp+"HK-Log");
var loggingObj=dom.GetObject(vrp+"HK-Logging");

!// Prüfe ob logging erwartet wird
if (log<0) {
  !// Zwingend kein logging
  log = false;
} elseif (log==0) {
!// Einstellung der Logging Variable prüfen
  if (loggingObj && loggingObj.State()!=0){
    log = true;
  }
} else {
  !// Logging einschalten
  log = true;
}

!// Logging zwingend ausschalten, wenn keine Variable vorhanden ist
if (!logObj){
  log = false;
}


!//Variablen
string error="kein";
string command;
string stemp;
string lat=system.Latitude();
string lon=system.Longitude();

if (DEBUG){
  WriteLine(lat+"\n");
  WriteLine(lon+"\n");
}

!// Temperaturwerte lesen 48h davor, 24h (heute) in die Zukunft.
command = "wget --timeout=3 -O - 'https://api.open-meteo.com/v1/forecast?latitude="+lat+"&longitude="+lon+"&hourly=temperature_2m&models=icon_seamless&current=temperature_2m&timezone=Europe%2FBerlin&past_days=2&forecast_days=2'";

!// Da es immer wieder mal zu Fehlern kommt, wiederholen wir die Anfrage im Abstand von 1 Sekunde mehrfach
integer iRetry = 10;
while (iRetry>0) {
  system.Exec(command, &stemp, &error);
  if (DEBUG){
    WriteLine(command+"\n");
    WriteLine(stemp+"\n");
  }

  !// passenden eintrag finden
  !// 18 = Länge von "temperature_2m":[ -> Sprung hinter die öffnende Klammer
  integer pos=stemp.Find("\"temperature_2m\":[");
  if (pos<0) {
    !// Muster nicht gefunden -> Antwort unbrauchbar (Timeout/Fehler/Teilantwort)
    if(log){logObj.State("Open-Meteo: Kein temperature_2m in der Antwort, Retry " # (11-iRetry) # "/10");}
    if(DEBUG){WriteLine("Open-Meteo: Kein temperature_2m in der Antwort, Retry " # (11-iRetry) # "/10\n");}
    iRetry = iRetry-1;
    string dummy;
    system.Exec("sleep 1", &dummy, &dummy);
    continue;
  }
  stemp=stemp.Substr(pos+18,1000);
  pos=stemp.Find("]");
  if (pos<0) {
    !// Schließende Klammer fehlt -> Antwort abgeschnitten
    if(log){logObj.State("Open-Meteo: Antwort unvollständig (kein ]), Retry " # (11-iRetry) # "/10");}
    if(DEBUG){WriteLine("Open-Meteo: Antwort unvollständig (kein ]), Retry " # (11-iRetry) # "/10\n");}
    iRetry = iRetry-1;
    string dummy;
    system.Exec("sleep 1", &dummy, &dummy);
    continue;
  }
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

  !// Begrenzungen festlegen
  stundenZurueck = stundenZurueck.Min(48).Max(1);
  stundenVoraus = stundenVoraus.Min(24).Max(0);
  integer ueberspringen = h+48-stundenZurueck;

  if (DEBUG){
    WriteLine("Uhrzeit:        " # h);
    WriteLine("Stunden zurück: " # stundenZurueck);
    WriteLine("Stunden voraus: " # stundenVoraus);
    WriteLine("Überspringen:   " # ueberspringen);
    WriteLine("---------");

    integer i=0;
    foreach (temp, stemp.Split(","))
    {
      WriteLine((i % 24) # ": "# temp);
      i = i+1;
    }
    WriteLine("---------");
  }

  !// Werte berechnen
  integer i=0;
  real aktuellerWert;
  foreach (temp, stemp.Split(","))
  {
    !// Werte bis zur aktuellen Stunde überspringen wir.
    if ((i>h) && i>ueberspringen){
      if (DEBUG){
        WriteLine((i % 24) # ": "# temp);
      }
      summe = summe+temp.ToFloat();
      n = n+1;
      if (n==stundenZurueck){
        aktuellerWert = temp.ToFloat();
        if (DEBUG){
          WriteLine("<-- Letzter Wert");
        }
      }
      if (n>=(stundenZurueck+stundenVoraus)) {
         break;
      }
    }
    i = i+1;
  }

  !// Letzter Wert = Aktuell gemeldete Temperatur

  if (DEBUG){
    WriteLine("---------");
    WriteLine(summe);
    WriteLine(n);
  }
  temp = (summe/n).ToString(1);
  WriteLine("Akt. Aussentemp.= " # aktuellerWert.ToString(1) # " / Durchsch. Aussentemp.= " # temp);

  if (log) {
    if (n!=0) {
      logObj.State("Akt. Aussentemp.= " # aktuellerWert.ToString(1) # " / Durchsch. Aussentemp.= " # temp);
    } else {
      logObj.State("Keine Werte für die Außentemperatur gefunden!");
    }
  }

  !// Ergebnis schreiben
  if (n!=0){
    dom.GetObject(vrp+"HK2-Aussentemperatur").State(temp);
    break;
  } else {
    !// Unerwünschtes Ergebnis/Fehler, also ein Retry mehr
    iRetry = iRetry-1;
    !// Verzögern
    string dummy;
    system.Exec("sleep 1", &dummy, &dummy);
  }
}
