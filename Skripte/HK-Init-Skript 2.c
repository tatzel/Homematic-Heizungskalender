!// Script zum Erzeugen der Systemvariablen des Heizkalenders für Skript 2 
!//================================================================================================
!// Stand:    25.11.2025; 
!// Autor:    Lukas Helduser
!//           Martin Richter    (heizkalender@m-ri.de)
!// Projekt:  Helmut Diedrichs  (helmut@diedrichs.de)
!//================================================================================================
!// Dieser Code wurde im Rahmen der Heizkalender-Implementierung der Baptisten Gemeinde Hanau 
!// entwicklet.
!// Die Nutzung ist kostenlos, aber wir bitten die Nutzung an einer der obigen Email Adressen zu 
!// melden.
!//================================================================================================
!//
!// Der Code basiert in großen Teilen auf der Datei: 
!//   HKP-S2-V-3.1.1 Variablen zu Skript2_V1.3.c
!// Der ursprüngliche Code wurde geschrieben von:
!//   Lukas Helduser (Youtube: https://www.youtube.com/LukasvandeHaag) 
!// Ich (MRi) habe diesen Code dann erweitert, korrigiert und verbessert um sie an die Nutzung in
!// meiner Gemeinde anzupassen.
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

!//Dieses Skript erstellt die nötigen Systemvariablen des Heizkalender für Skript 2 zum Schalten des Heizkalender
!//Hinweis: Ein erneutes Ausführen dieses Programms ändert bestehende Variablen und ihren Inhalt nicht

!//Eingabe eines Namens Präfix
!//Dies ist nur erforderlich wenn die Namensvorgabe abgeändert werden soll.
!//Wird hier ein Präfix eingeben so muss dieser in allen Skripten auch angegeben werden.
string vrp="";

!// Namen der Variablen, die angelegt werden sollen
string nm=  "HK2-HKG-Liste;" #                                               
            "HK2-Grundtemperatur;" #                                         
            "HK2-A.Temp.Grenze;" #                                           
            "HK2-Aussentemperatur;" #                                        
            "HK2-Hand-Temp;" #                                               
            "HK2-Hand-Grundtemp;" #                                          
            "HK2-VorzeitAus;" #                                              
            "HK2-Kurvenversatz;" #                                           
            "HK2-Kurve";                                                     

!// Beschreibungstexte
string be=  "Liste der HK-Gruppenvariablen;" #
            "Grundtemperatur;" #
            "Aussentemperatur-Grenzwert;" #
            "Aussentemperatur;" #
            "Vorrang manuell eingestellte Temperatur beim Ausschalten;" #
            "Rückstellung auf Grundtemperatur;" #
            "Grundoffsetzeit Ausschalten;" #
            "Grundoffsetzeit Einschalten;" #
            "Hzg.-Vorlaufzeit: Eingeben in Minuten zu Außentemperaturen kleiner als -10, -5, 0, 8, 10, 12, 15, 17,5";

!// Typen
string tp=  "string;" #                                                
            "integer;" #                                               
            "integer;" #                                               
            "integer;" #                                               
            "boolean;" #                                               
            "boolean;" #                                               
            "integer;" #                                               
            "integer;" #                                               
            "string;";

!// Vorgabe Werte
string vl=  ";" #
            "16.0;" #
            "19.0;" #
            "0.0;" #
            "0;" #
            "1;" #
            "0;" #
            "0;" #
            "360,300,240,180,150,120,90,60";    !// Komma wird ersetzt durch Semikolon

!// Einheit (z.B. °C)
string vu=  ";" #
            "°C;" #
            "°C;" #
            "°C;" #
            ";" #
            ";" #
            "min;" #
            "min;" #
            "min";

!// Zusätzliche Info Texte für 
!//   - Boolean   - Werte für eine Variable durch + getrennt
!//   - Numerisch - min/max durch + getrennt
string wr=  ";" #                     
            "5+30;" #                                                                
            "-50+50;" #                                                                
            "-50+50;" #                                                                
            "Immer auf Grundtemperatur zurücksetzen+Manuelle Temperatur hat Vorrang;" #                                                                
            "Keine Rückstellung auf Grundtemperatur um 01:00 Uhr+Rückstellung auf Grundtemperatur um 01:00 Uhr;" #                                                                
            "0+120;" #                                                                
            "-100+100;" #                                                                
            "";
                        
!// Protokollierungs Flags
string pr=  "1;" #                                                               
            "1;" #                                                               
            "1;" #                                                               
            "1;" #                                                               
            "1;" #                                                               
            "1;" #                                                               
            "1;" #                                                               
            "1;" #
            "1";                                                                 

!//------------------------------------------------------------------------------------------------
!// Ab hier Standard Code zum erzeugen von Variablen
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
    svObj.ValueUnit(vu.StrValueByIndex(";",i));
    svObj.DPArchive(pr.StrValueByIndex(";",i).StrValueByIndex("+",0).ToInteger()!=0);
    svObj.Internal(false);
    svObj.Visible(true);

    !// Nach typ optionale Werte setzen
    string typ = tp.StrValueByIndex(";",i);
    if(typ=="string"){
      svObj.ValueType(ivtString);
      svObj.ValueSubType(istChar8859);
      svObj.State(vl.StrValueByIndex(";",i).Replace(",",";"));
    }elseif(typ=="integer"){
      svObj.ValueType(ivtFloat);
      svObj.ValueSubType(istGeneric);
      svObj.State(vl.StrValueByIndex(";",i).ToFloat());
      svObj.ValueMin(wr.StrValueByIndex(";",i).StrValueByIndex("+",0).ToInteger());
      svObj.ValueMax(wr.StrValueByIndex(";",i).StrValueByIndex("+",1).ToInteger());
    }elseif(typ=="boolean"){
      svObj.ValueType(ivtBinary);
      svObj.ValueSubType(istBool);
      svObj.ValueName0(wr.StrValueByIndex(";",i).StrValueByIndex("+",0));
      svObj.ValueName1(wr.StrValueByIndex(";",i).StrValueByIndex("+",1));    
      svObj.State(vl.StrValueByIndex(";",i).ToInteger());
    }else{
      quit;
    }
    WriteLine("Variable " # name # " angelegt")
  }else{
    WriteLine("Variable " # name # " existiert schon!")
  }
  i=i+1;
}

!// Update der ReGa Skript Engine. Siehe:
!//     https://homematic-forum.de/forum/viewtopic.php?t=45380
dom.RTUpdate(0);

WriteLine("Alles erledigt...!");
