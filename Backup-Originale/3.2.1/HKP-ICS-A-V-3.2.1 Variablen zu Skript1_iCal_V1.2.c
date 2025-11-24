
!//Dieses Skript erstellt die nötigen Systemvariablen des Heizkalender für Skript 1 per iCal
! Stand: 30.09.2025; Urheber: Programm: Lukas Helduser, Projekt: Helmut Diedrichs
! Anleitung um das Skript auszuführen: Menü WebUI der CCU
! Kopieren sie den Inhalt dieses Datei komplett und unverändert in das Fenster:
! => Programm und Verknüpfungen => Skripte Testen
! Betätigen sie den Button "Ausführen",
! im Ausgabe Fenster sehen sie ob und welche Variablen angelegt wurden (als Info).
! Beim erscheinen des Text "alles erledigt" wurde das Skript komplett ausgeführt.
! Sie können das Fenster dann wieder schließen.
! im Menü Status und Bedienung => Systemvariable die systemvariablen anschauen
! im Menü Einstellungen => Systemvariable => den Systemvariablen Werte ggf. zuweisen

!//Dieses Skript erstellt die nötigen Systemvariablen des Heizkalender für Skript 1 Google Kalender
!//Hinweis: Ein erneutes Ausführen dieses Programms ändert bestehende Variablen und ihren Inhalt nicht

!//Eingabe eines Namens Präfix
!//Dies ist nur erforderlich wenn die Namensvorgabe abgeändert werden soll.
!//Wird hier ein Präfix eingeben so muss dieser in allen Skripten auch angegeben werden.
string vrp="";

string nm1="HK1-Schaltliste;HK1-ICS-Url;HK1-R-Liste";
string be1="Hier bitte nichts verändern;Eingabe URL des iCal Kalender;Namensliste der Gruppen";
string tp1="string;string;string";


!//---------------------------------------------------------------------------------------------------------------------------------------------------------
integer i=0;
while(i<3){

  object  svObj  = dom.GetObject(vrp+nm1.StrValueByIndex(";",i));
  if (!svObj){
  object svObjects = dom.GetObject(ID_SYSTEM_VARIABLES);
  svObj = dom.CreateObject(OT_VARDP);
  svObjects.Add(svObj.ID());
  !//
  svObj.Name(vrp+nm1.StrValueByIndex(";",i));
  svObj.DPInfo(be1.StrValueByIndex(";",i));
  svObj.ValueUnit("");
  svObj.DPArchive(true);
  svObj.Internal(false);
  svObj.Visible(true);

  if(tp1.StrValueByIndex(";",i)=="string"){
    svObj.ValueType(ivtString);
    svObj.ValueSubType(istChar8859);
    if(i<>0){svObj.State("Eingabe fehlt");}else{svObj.State("");};
    }

  dom.RTUpdate(false);
  WriteLine("Variable "+vrp+nm1.StrValueByIndex(";",i)+" angelegt")
    }else{
      WriteLine("Variable "+vrp+nm1.StrValueByIndex(";",i)+" existiert schon!")
    }
  i=i+1;
  }
  WriteLine("Alles erledigt...!");
