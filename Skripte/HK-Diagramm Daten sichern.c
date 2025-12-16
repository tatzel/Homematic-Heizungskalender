!// Sichern der Cloudmatic Diagramm Daten
!//================================================================================================
!// Stand:    16.12.2025; 
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

!// Der Pfad sollte keine Leerzeichen enthalten
string pfad = "/media/usb1/";
!// Prefix für den Namen, er wird dann mit dem Datum und der Extension .LOG erweitert.
!// Diagrammdaten werden wöchentlich gespeichtert
!// Also z.B. HK-Diagramm_<name>_2025-11-01.log!
string prefix = "HK-CM-Diag_";
!// Anzahl der Dateien die erhalten bleiben sollen. Die ältesten Dateien werden automatisch gelöscht.
integer AnzahlDateien=10;

!// Debug Mouds, wir haben mehrere Levels 0 (Keine Debug Ausgaben) Debugausgaben Leve 1/2
integer DEBUG=0;

!//#######---Ende Variabler Bereich---####################################################################
!//Im Folgenden Hier keine Veränderungen vornehmen!

string cmd = "";
string stdout;
string stderr;
string dateiname;

!// Lade die Diagrammliste
object logVar = dom.GetObject("_CM_diagrams_");
string logValues = logVar.Value();
!logValues="3058,_CM_Diagramm_Außentemperatur,1,30,1,Außentemperatur";
if(DEBUG){WriteLine("_CM_diagrams_" # logValues);}

!// Über alle Diagramme laufen
string logVal;
foreach(logVal, logValues.Split(";")){
  !if(DEBUG){WriteLine("logVal=" # logVal);}
  string stringDP = logVal.StrValueByIndex(",", 0);
  string valueStringName = logVal.StrValueByIndex(",", 1);
  real digits = logVal.StrValueByIndex(",", 2);
  string diffMinutes = logVal.StrValueByIndex(",", 3);
  integer active = logVal.StrValueByIndex(",", 4);
  string diagrammName = logVal.StrValueByIndex(",", 5)
                .Replace(" ","_")
                .Replace("ß","ss")
                .Replace("ä","ae")
                .Replace("Ä","Ae")
                .Replace("ö","oe")
                .Replace("Ö","Oe")
                .Replace("ü","ue")
                .Replace("Ü","Ue");
  if (active != 0){
    !// String mit Daten laden
    if(DEBUG){WriteLine("----------------------------------------");}
    if(DEBUG){WriteLine("valueStringName=" # valueStringName);}
    string valueString = dom.GetObject(valueStringName).State().Replace(",",";");
    !if(DEBUG){WriteLine("valueString=" # valueString);}

    !// Cleanup. Lösche so viele Dateien, biss nur noch die entsprechende Anzahl übrig sind.
    cmd = "ls -w 1 -r \"" # pfad # prefix #"\"*\"_" # diagrammName # ".log\"";
    !if(DEBUG){WriteLine("cmd="#cmd);}
    system.Exec(cmd,&stdout,&stderr);
    !if(DEBUG){WriteLine(stdout);}
    !if(DEBUG){WriteLine(stderr);}
    string datei;
    integer cnt=0;
    foreach(datei,stdout.Trim().Split("\n")) {
      !if(DEBUG){WriteLine("Zu löschen="  # datei);}
      if (cnt>=AnzahlDateien){
        if(DEBUG){WriteLine("Löschen=" # datei);}
        cmd = "rm \"" # datei #"\" &";
        !if(DEBUG){WriteLine(cmd);}
        system.Exec(cmd);
      }
      cnt=cnt+1;
    }

    !// String aufspalten und neu aufbauen
    integer n = valueString.StrValueByIndex(";",0).ToInteger();
    if(DEBUG){WriteLine(n);}
    integer i = 0;
    string alleEintraege="";
    string aktDatum="";
    integer iPos=valueString.Find(";");
    !// Diese Schelife mit StrValueByIndex ist extrem viel langsamer.
    !// Wir nehmen einfach Substr. Eine Schleife die Zeichenweise prüft geht.
    !// Ich benutzer aber die Substr Variant, dieweniger Script Schritte auslöst.
    valueString = valueString.Substr(iPos+1,valueString.Length()-iPos-1);
    iPos = valueString.Find(";");
    while ((iPos>=0) && (valueString.Length()!=0))
    {
      !// Ersten Teilstring nehmen
      alleEintraege = alleEintraege # valueString.Substr(0,iPos+1);
      valueString = valueString.Substr(iPos+1,valueString.Length()-iPos-1);
      !// Zweiter Teilstring
      iPos = valueString.Find(";");
      alleEintraege = alleEintraege # valueString.Substr(0,iPos) # "\n";
      valueString = valueString.Substr(iPos+1,valueString.Length()-iPos-1);
      iPos = valueString.Find(";");
    }
    !if(DEBUG){WriteLine("alleEintraege=\n" # alleEintraege);}

    !// Zuletzt geschriebene Daten suchen
    while (true){
      !// Modus Suche starten
      boolean modusSuchen=true;
      string aktDatum="";
      string letzterGeschriebenerDatensatz;
      string zuSchreiben="";
      string eintrag;
      foreach(eintrag,alleEintraege.Split("\n")){
        !if(DEBUG){WriteLine("Eintrag=" # eintrag);}
        !// Wir wollen keine Semikolons, wir wollen Tabs
        !// Wir testen mal das Datum und prüfen ob wir noch auf der aktuellen Datum liegen
        eintrag = eintrag.Replace(";","\t");
        string stmpDate = eintrag.StrValueByIndex("\t",0).StrValueByIndex(" ",0);

        !// Wohenanfang suchen
        stmpDate = (stmpDate.ToTime().ToInteger()-((stmpDate.ToTime().Format("%u").ToInteger()-1)*86400)).ToTime().Format("%F");
        !if(DEBUG){WriteLine("aktDatum=" # aktDatum # " stmpDate="#stmpDate);}
        if ((aktDatum=="") || (aktDatum!=stmpDate)){
          !// Start? Dann ist aktDatum leer. Ansonsten bestehende Daten speichern
          if(DEBUG){WriteLine("Datum=" # stmpDate);}
          if (aktDatum){
            !// Alte Daten speichern, wenn vorhanden
            if (zuSchreiben.Length()>0){
              !// Zeilenschaltung am Ende entfernen, denn den haben wir schon
              if(DEBUG){WriteLine("Dateiname="#dateiname);}
              if(DEBUG){WriteLine("Daten schreiben 1: " # dateiname # " Bytes: " # zuSchreiben.Length());}
              cmd = "echo -n \"" # zuSchreiben # "\" >> '" # dateiname # "' &";
              system.Exec(cmd);
              zuSchreiben = "";
            }else{
              !// Sollten wir gerade in den Schreibmodus gegangen sein, aber nun einen Dateiwechsel
              !// haben, dann hatten wir gerade den letzten Datensatz gefunden. Und wir suchen auch in dern
              !// neuen Datei weiter
              modusSuchen = true;
            }
          }
          !// Datumswechsel. Neue Datei.
          aktDatum = stmpDate;
          !// neuen Dateiname setzen
          dateiname = pfad # prefix # aktDatum # "_"# diagrammName # ".log";
          if(DEBUG){WriteLine("Dateiname="#dateiname);}
          if(modusSuchen){
            !// Wir laden den zuletzt gelesenen Datensatz
            cmd = "tail -n 1 '" # dateiname # "'";
            !if(DEBUG){WriteLine("cmd="#cmd);}
            stdout="";
            system.Exec(cmd,&stdout,&stderr);
            !if(DEBUG){WriteLine("stdout="# stdout);}
            !if(DEBUG){WriteLine("stderr="# stderr);}

            !// Evtl. diesen Datensatz suchen, wenn da was war, ist kein Datensatz (keine Datei vorhanden)
            !// gehen wir sofort in den Schreibmodus
            letzterGeschriebenerDatensatz = stdout.Trim();
            !if(DEBUG){WriteLine("letzterGeschriebenerDatensatz=" # letzterGeschriebenerDatensatz);}
            modusSuchen = (letzterGeschriebenerDatensatz!="");
            if(DEBUG){WriteLine("modusSuchen=" # modusSuchen # " letzterGeschriebenerDatensatz=" # letzterGeschriebenerDatensatz);}
          }
        }

        !// Prüfe ob wir die Zeile erreicht haben, die wir bereits geschrieben haben
        if (modusSuchen){
          if (eintrag.Trim()==letzterGeschriebenerDatensatz){
            !// Wir können sofort anfangen zu schreiben, ist aber möglich,
            !// das dies der letzte Einrag war.
            modusSuchen = false;
            if(DEBUG){WriteLine("Modus Suchen: Datensatz gefunden, Wechsel nach Modus schreiben und Daten sammeln");}
          }
        }else{
          !// Wir sammeln die Daten und haben alles gelesen um es zu schreiben
          !// Jetzt können wir die while(true) Schleife abbrechen.
          if(DEBUG){WriteLine("Modus Schreiben: Ende der Daten gefunden");}
          zuSchreiben = zuSchreiben # eintrag # "\n";
        }
      }

      !// Wir haben die Liste einmal durch.
      if (modusSuchen){
        !// Wenn wir hierher kommen, dann haben wir keinen passenden datensatz gefunden
        !// Wir starten von vorne und schreiben jetzt alles
        if(DEBUG){WriteLine("Modus: Suchen. Stop! Keine Daten gefunden, alle Daten sammeln");}
        modusSuchen = false;
        aktDatum = "";
      }
      else
      {
        !// Jetzt haben wir alle Daten zum schreiben fertig
        !if(DEBUG){WriteLine(zuSchreiben);}
        if(DEBUG){WriteLine("Modus: Schreiben. Stop! Gefundene Daten schreiben");}
        break;
      }
    }

    !// Rest schreiben
    if (zuSchreiben.Length()>0){
      !// Zeilenschaltung entfernen, denn den haben wir schon
      if(DEBUG){WriteLine("Daten schreiben 2: " # dateiname # " Bytes: " # zuSchreiben.Length());}
      string cmd = "echo -n \"" # zuSchreiben # "\" >> '" # dateiname # "' &";
      !if(DEBUG){WriteLine("cmd=" # cmd);}
      system.Exec(cmd);
      zuSchreiben = "";
    }else{
      if(DEBUG){WriteLine("Daten schreiben 2: Keine Daten zu schreiben " # dateiname);}
    }
  }
}

!// Mal auch alles sichern
system.Save();

WriteLine("Alles fertig...");
