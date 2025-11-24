!// Das Skript erstellt die nötigen Systemvariablen des Heizkalender für das Logging zu erzeugen
!// Stand: 18.11.2025; 
!// Autor: Martin Richter, Projekt: Helmut Diedrichs
!//
!// Anleitung um das Skript auszuführen: 
!// - Menü WebUI der CCU
!// - Kopieren sie den Inhalt dieses Datei komplett und unverändert in das Fenster:
!//   => Programm und Verknüpfungen => Skripte Testen
!// - Betätigen sie den Button "Ausführen", im Ausgabe Fenster sehen sie ob und welche Variablen 
!//   angelegt wurden (als Info).
!// - Beim erscheinen des Text "alles erledigt" wurde das Skript komplett ausgeführt.
!// Sie können das Fenster dann wieder schließen.
!// im Menü Status und Bedienung => Systemvariable die Systemvariablen kontrollieren
!// Werte können nur über HQ-WebUI, oder den CloudMatic Zugang geesetzt werden

!// Die Variablen HK1-Logging;HK1-Log;HK2-Logging;HK2-Log werden für das interne Logging in Skript1 und 
!// Skript2 verwenden in der MRI.
!// HK-SystemProtokollSichern wird verwendet um das System Protokoll regelmässig auf einen 
!// USB Stick zu speichern.

!//Dieses Skript erstellt die nötigen Systemvariablen des Heizkalender für Skript 1 ChurchTools
!//Hinweis: Ein erneutes Ausführen dieses Programms ändert bestehende Variablen und ihren Inhalt nicht

!//Eingabe eines Namens Präfix
!//Dies ist nur erforderlich wenn die Namensvorgabe abgeändert werden soll.
!//Wird hier ein Präfix eingeben so muss dieser in allen Skripten auch angegeben werden.
string vrp="";

// Namen der Variablen, die angelegt werden sollen
string nm="HK1-Logging;HK1-Log;HK2-Logging;HK2-Log;HK-SystemProtokollSichern";
!// Typen
string tp="boolean;string;boolean;string;string";
!// Beschreibungstexte
string be="Flag um das Logging für Skript 1 ein- und auszuschalten;Variable um ein einfaches Log im System Protokoll zu erzeugen;Flag um das Logging für Skript 2 ein- und auszuschalten;Variable um ein einfaches Log im System Protokoll zu erzeugen;Letzter gesicherter Datensatz des Systemprotokolls";
!// Zusätzliche Info Texte für Boolean (Werte für eine Variable durch + getrennt)
string vl= "Logging ausgeschaltet+Logging eingeschaltet;;Logging ausgeschaltet+Logging eingeschaltet;;";
!// Werte Bereich (String/Numerisch (min/max durch + getrennt)
string wr= ";;;;";
!// Protokollierungs Flags
string pr= "1;1;1;1;0";


!//---------------------------------------------------------------------------------------------------------------------------------------------------------
integer i=0;

WriteLine("Start ...!");
while(true){
  if (nm.StrValueByIndex(";",i)==""){
    break;
  }
  string name=vrp+nm.StrValueByIndex(";",i);
  object  svObj  = dom.GetObject(name);
  if (!svObj){
    !// Variable existiert nicht
    object svObjects = dom.GetObject(ID_SYSTEM_VARIABLES);
    svObj = dom.CreateObject(OT_VARDP);
    svObjects.Add(svObj.ID());
  
    svObj.Name(name);
    svObj.DPInfo(be.StrValueByIndex(";",i));
    svObj.ValueUnit("");
    svObj.DPArchive(pr.StrValueByIndex(";",i).StrValueByIndex("+",0).ToInteger()!=0);
    svObj.Internal(false);
    svObj.Visible(true);

    !// Nach typ optionale Werte setzen
    string typ = tp.StrValueByIndex(";",i);
    if(typ=="string"){
      svObj.ValueType(ivtString);
      svObj.ValueSubType(istChar8859);
      svObj.State(wr.StrValueByIndex(";",i).StrValueByIndex("+",0));
    }elseif(typ=="integer"){
      svObj.ValueType(ivtFloat);
      svObj.ValueSubType(istGeneric);
      svObj.State(0);
      svObj.ValueMin(wr.StrValueByIndex(";",i).StrValueByIndex("+",0).ToFloat());
      svObj.ValueMax(wr.StrValueByIndex(";",i).StrValueByIndex("+",1).ToFloat());
    }elseif(typ=="boolean"){
      svObj.ValueType(ivtBinary);
      svObj.ValueSubType(istBool);
      svObj.ValueName0(vl.StrValueByIndex(";",i).StrValueByIndex("+",0));
      svObj.ValueName1(vl.StrValueByIndex(";",i).StrValueByIndex("+",1));    
      svObj.State(true);
    }else{
      quit;
    }
    dom.RTUpdate(false);
    WriteLine("Variable " # name # " angelegt")
  }else{
    WriteLine("Variable " # name # " existiert schon!")
  }
  i=i+1;
}
WriteLine("Alles erledigt...!");
