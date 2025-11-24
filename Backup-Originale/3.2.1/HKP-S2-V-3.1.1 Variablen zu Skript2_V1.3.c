
!//Dieses Skript erstellt die nötigen Systemvariablen des Heizkalender für Skript 2 zum Schalten des Heizkalender
! Stand: 09.10.2025; Urheber: Programm: Lukas Helduser, Projekt: Helmut Diedrichs
! Anleitung um das Skript auszuführen: Menü WebUI der CCU
! Kopieren sie den Inhalt dieses Datei komplett und unverändert in das Fenster:
! => Programm und Verknüpfungen => Skripte Testen
! Betätigen sie den Button "Ausführen",
! im Ausgabe Fenster sehen sie ob und welche Variablen angelegt wurden (als Info).
! Beim erscheinen des Text "alles erledigt" wurde das Skript komplett ausgeführt.
! Sie können das Fenster dann wieder schließen.
! im Menü Status und Bedienung => Systemvariable die systemvariablen anschauen
! im Menü Einstellungen => Systemvariable => den Systemvariablen Werte ggf. zuweisen

!//Dieses Skript erstellt die nötigen Systemvariablen des Heizkalender für Skript 2 zum Schalten des Heizkalender
!//Hinweis: Ein erneutes Ausführen dieses Programms ändert bestehende Variablen und ihren Inhalt nicht

!//Eingabe eines Namens Präfix
!//Dies ist nur erforderlich wenn die Namensvorgabe abgeändert werden soll.
!//Wird hier ein Präfix eingeben so muss dieser in allen Skripten auch angegeben werden.
string vrp="";

string nm1="HK2-HKG-Liste;HK2-Grundtemperatur;HK2-A.Temp.Grenze;HK2-Aussentemperatur;HK2-Hand-Temp;HK2-Hand-Grundtemp;HK2-VorzeitAus;HK2-Kurvenversatz;HK2-Kurve;HK2-Location";
string wt1=";16.5;19;0;0;1;50;0;";
string un1=";°C;°C;°C;;;min;min;min";
string wt2="360;300;240;180;150;120;90;60";
string be1="Liste der HK-Gruppenvariablen;Grundtemperatur;Aussentemperatur-Grenzwert;Aussentemperatur;Funktionswahl Handvorrang;Funktionswahl Hand-Grundtemp;Grundoffsetzeit Ausschalten;Grundoffsetzeit Einschalten;Vorlaufzeiten;Location für Wetter-API";
string tp1="string;int;int;int;int;int;int;int;string;string";
string be2="Hzg.-Vorlaufzeit: Eingeben in Minuten zu Außentemperaturen kleiner als -10, -5, 8, 10, 12, 15, 17,5";


!//---------------------------------------------------------------------------------------------------------------------------------------------------------
integer i=0;
while(i<10){

  object  svObj  = dom.GetObject(vrp+nm1.StrValueByIndex(";",i));
  if (!svObj){
  object svObjects = dom.GetObject(ID_SYSTEM_VARIABLES);
  svObj = dom.CreateObject(OT_VARDP);
  svObjects.Add(svObj.ID());
  !//
  svObj.Name(vrp+nm1.StrValueByIndex(";",i));
  if(i<>8){svObj.DPInfo(be1.StrValueByIndex(";",i));}else{svObj.DPInfo(be2);}
  svObj.ValueUnit("");
  svObj.DPArchive(true);
  svObj.Internal(false);
  svObj.Visible(true);

  if(tp1.StrValueByIndex(";",i)=="string"){
    svObj.ValueType(ivtString);
    svObj.ValueSubType(istChar8859);
    svObj.State("");
    if(i==8){svObj.State(wt2);}
    if(i==0){svObj.State("Eingabe fehlt");}
    if(i==10){svObj.State("Eingabe fehlt");}
    if(i==9){svObj.State("Eingabe fehlt");}
    }
  if(tp1.StrValueByIndex(";",i)=="int"){
    svObj.ValueType(ivtFloat);
    svObj.ValueSubType(istGeneric);
    svObj.State(wt1.StrValueByIndex(";",i).ToInteger());
    svObj.ValueMin(0);
    svObj.ValueMax(100);
    }
  dom.RTUpdate(false);
  WriteLine("Variable "+vrp+nm1.StrValueByIndex(";",i)+" angelegt")
  }else{
    WriteLine("Variable "+vrp+nm1.StrValueByIndex(";",i)+" existiert schon!")
  }
i=i+1;
}
WriteLine("Alles erledigt...!");
