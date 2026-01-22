!// Skript zum Anlegen der Systemvariablen für Skript 1 (Google)
!//================================================================================================
!// Stand:    23.01.2026
!// Autoren:  Lukas Helduser    (Youtube: https://www.youtube.com/LukasvandeHaag)
!//           Martin Richter    (heizkalender@m-ri.de) http://blog.m-ri.de/
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
!// Der Code basiert in großen Teilen auf der Datei: 
!//   HKP-GK-V-3.2.1 Variablen zu Skript1_GoogleKalender_V1.2.c
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

!//Dieses Skript erstellt die nötigen Systemvariablen des Heizkalender für Skript 1 Google Kalender
!//Hinweis: Ein erneutes Ausführen dieses Programms ändert bestehende Variablen und ihren Inhalt nicht

!//Eingabe eines Namens Präfix
!//Dies ist nur erforderlich wenn die Namensvorgabe abgeändert werden soll.
!//Wird hier ein Präfix eingeben so muss dieser in allen Skripten auch angegeben werden.
string vrp="";

!// Namen der Variablen, die angelegt werden sollen
string nm=  "HK1-Schaltliste;" #
            "HK1-GK-API-Key;" #
            "HK1-GK-Kalender-ID;" #
            "HK1-R-Liste";
            
!// Beschreibungstexte
string be=  "Schaltliste. Hier bitte nichts verändern!;" #
            "API-Key für den Google-Kalender;" #
            "ID des Google-Kalender;" #
            "Zuordnung der Räume aus der Ressourcenverwaltung";
            
!// Typen
string tp=  "string;" #                                                          
            "string;" #                                                          
            "string;" #                                                          
            "string";                                                            

!// Vorgabe Werte
string vl=  "";                                                                  
                                          
!// Einheit (z.B. °C)
string vu=  "";                                                                  

!// Zusätzliche Info Texte für 
!//   - Boolean   - Werte für eine Variable durch + getrennt
!//   - Numerisch - min/max durch + getrennt
string wr=  "";                                                                  
                        
!// Protokollierungs Flags
string pr=  "1;" #                                                               
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
    svObj.DPArchive(pr.StrValueByIndex(";",i).ToInteger()!=0);
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
