!// Skript zum Anlegen der Systemvariablen für Skript 1 (ChurchDesk)
!//================================================================================================
<<<<<<< Updated upstream
!// Stand:    25.11.2025;
=======
!// Stand:    16.12.2025; 
>>>>>>> Stashed changes
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
!//   HKP-CT-V--3.2.1 Variablen zu Skript1_ChurchTools_V1.4.c
!// Der ursprüngliche Code wurde geschrieben von:
!//   Lukas Helduser (Youtube: https://www.youtube.com/LukasvandeHaag)
!// Ich (MRi) habe diesen Code dann erweitert, korrigiert und verbessert um sie an die Nutzung in
!// meiner Gemeinde anzupassen.
!//

!// Dieses Skript erstellt die nötigen Systemvariablen des Heizkalender für Skript 1 ChurchDesk
!// Hinweis: Ein erneutes Ausführen dieses Programms ändert bestehende Variablen und ihren Inhalt nicht
<<<<<<< Updated upstream
!// Tipp: Sie sollten unbedingt die Variablen organizationId und apiToken vorbelegen, weil dann
=======
!// Tipp: Sie sollten unbedingt die Variablen organizationId und apiToken vorbelegen, weil dann 
>>>>>>> Stashed changes
!// automatisch alle benötigten Variablen automatisch erzeugt.

!//Eingabe eines Namens Präfix
!//Dies ist nur erforderlich wenn die Namensvorgabe abgeändert werden soll.
!//Wird hier ein Präfix eingeben so muss dieser in allen Skripten auch angegeben werden.
string vrp="CD_";

!// Es ist möglich hier sofort ein API-Tokn für ChurchDesk anzugeben. Da mit werden gleich weitere
!// Variablen für alle Ressourcen angelegt und die "HK1-R-Liste" befüllt. Ist ein API-Token bereits
!// in den Variablen angelegt wird dieses verwendet
string apiToken="c394ee6875210309c3cbe5c54f34054998ef41bf402ea299f7361059f72ea308";

<<<<<<< Updated upstream
!// Vorgegebene Orgnaisations ID
=======
!// Vorgegebene Orgnaisations ID 
>>>>>>> Stashed changes
string organizationId = "7102";

!// Vorgabe für Raumnamen Prefix
string raumNamenPreFix =  vrp#"HKG-Raum-";

!// Vorgabe für Raumlisten Variablen
string raumVorgabePreFix = "0;H;IP;20;0;30;Heizgruppe: ";
string raumVorgabePostFix = ":1";

!//################################################################################################
!//######------Skript Variablen und Skript Arbeitsteil. Vom Benutzer nicht zu verändern------######

!// Namen der Variablen, die angelegt werden sollen
string nm=  "HK1-Schaltliste;" #
            "HK1-R-Liste;" #
            "HK1-CD-OrganisationsId;" #
            "HK1-CD-Token;" #
            "HK2-HKG-Liste;";
<<<<<<< Updated upstream

=======
            
>>>>>>> Stashed changes
!// Beschreibungstexte
string be=  "Schaltliste. Hier bitte nichts verändern!;" #
            "Zuordnung der Räume aus ChurchDesk;" #
            "Organisations ID in ChurchDesk;" #
            "API-Token für ChurchDesk;" #
<<<<<<< Updated upstream
            "Liste der HK-Raum-Variablen;";

!// Typen
string tp=  "string;" #
            "string;" #
            "string;" #
            "string;" #
            "string";
=======
            "Liste der HK-Raum-Variablen;";              
            
!// Typen
string tp=  "string;" #                                                          
            "string;" #                                                          
            "string;" #                                                          
            "string;" #                                                          
            "string";                                                            
>>>>>>> Stashed changes

!// Vorgabe Werte
string vl=  "";

!// Einheit (z.B. °C)
string vu=  "";

!// Zusätzliche Info Texte für
!//   - Boolean   - Werte für eine Variable durch + getrennt
!//   - Numerisch - min/max durch + getrennt
string wr=  "";

!// Protokollierungs Flags
<<<<<<< Updated upstream
string pr=  "1;" #
            "1;" #
            "1;" #
            "1;" #
            "1";
=======
string pr=  "1;" #                                                               
            "1;" #                                                               
            "1;" #                                                               
            "1;" #                                                               
            "1";                                                                 
>>>>>>> Stashed changes

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
    WriteLine("Variable " # name # " angelegt");
  }else{
    WriteLine("Variable " # name # " existiert schon!");
  }
  i=i+1;
}

!//------------------------------------------------------------------------------------------------

!// Prüfe ob ein API-Token vorhanden ist
svObj = dom.GetObject(vrp#"HK1-CD-Token");
if(svObj.State()!=""){
  !// Aktuelles Token verwenden
  apiToken = svObj.State();
}elseif(apiToken!=""){
  !// Setze das aktuelle Token
<<<<<<< Updated upstream
  svObj.State(apiToken);
=======
  svObj.State(apiToken);  
>>>>>>> Stashed changes
}

!// Prüfe ob eine OrganisationsId vorhanden ist
svObj = dom.GetObject(vrp#"HK1-CD-OrganisationsId");
if(svObj.State()!=""){
  !// Aktuelles Token verwenden
  organizationId = svObj.State();
}elseif(organizationId!=""){
  !// Setze das aktuelle Token
<<<<<<< Updated upstream
  svObj.State(organizationId);
=======
  svObj.State(organizationId);  
>>>>>>> Stashed changes
}

!// Wenn ein API-Token vorhandne ist lesen wir die Ressource Liste
if (apiToken && organizationId){
  !// Wir versuchen die Ressource Liste zu lesen
  if (dom.GetObject(vrp#"HK1-R-Liste").State()!=""){
    WriteLine("Die Variablen für die ChurchDesk Ressourcen wurden bereits gesetzt!");
  }else{
    !// URL aufbauen
<<<<<<< Updated upstream
    string cmd = "wget --timeout=3 -O - 'https://api2.churchdesk.com/api/v3.0.0/events/resources?partnerToken=" # apiToken # "&organizationId=" # organizationId #"'";
    string stdout;
    string stderr;
    system.Exec(cmd, &stdout, &stderr);

=======
    string cmd = "wget --timeout=3 -O - 'https://api2.churchdesk.com/api/v3.0.0/events/resources?partnerToken=" # apiToken # "&organizationId=" # organizationId #"'";  
    string stdout;
    string stderr;
    system.Exec(cmd, &stdout, &stderr);
    
>>>>>>> Stashed changes
    if (stdout.StartsWith("[")){
      !// Zeichensatz fixen
      stdout = stdout.ToLatin();
      !// Wir bauen nun die raumliste auf
      string raumListe = "";
      string raumNamen = "";
      !// Nun die Ressourcen separieren "{\"id\":"
      string res;
      !// Achtung der Spli ersetzt nur das erste Zeichen durch \t
      foreach(res,stdout.Split("{\"id\":")){
        if (!res.StartsWith("\"id\":")){
          !// Fehler, falsches format
          continue;
        }
<<<<<<< Updated upstream

        !// Bei Color 0 ist das der Gemeinde-Eintrag, den überspringen wir.
        integer iPos = res.Find("\"color\":");
        if (iPos && res.Substr(iPos+8,10).ToInteger()==0){
          !// Den Gemeinde-Eintrag kann man beim buchen nicht benutzen.
          !// Einen anderen Indikator als die Farbe habe ich nicht gefunden.
          continue;
        }

        !// Id holen
        integer resId = res.Substr(5,10).ToInteger();

=======
        
        !// Bei Color 0 ist das der Gemeinde-Eintrag, den überspringen wir.
        integer iPos = res.Find("\"color\":");
        if (iPos && res.Substr(iPos+8,10).ToInteger()==0){
          !// Den Gemeinde-Eintrag kann man beim buchen nicht benutzen. 
          !// Einen anderen Indikator als die Farbe habe ich nicht gefunden.
          continue;
        }
        
        !// Id holen
        integer resId = res.Substr(5,10).ToInteger();
        
>>>>>>> Stashed changes
        !// Namen suchen
        integer iPos = res.Find(",\"name\":\"");
        if (iPos<0){
          !// Fehler
          continue;
        }
<<<<<<< Updated upstream

=======
        
>>>>>>> Stashed changes
        !// Namen finden
        res = res.Substr(iPos+9,res.Length()-9);
        integer iPosEnd = res.Find("\",\"");;
        if (iPos<0){
          !// Fehler
          continue;
        }
<<<<<<< Updated upstream

        !// Namen bereinigen
        name = res.Substr(0,iPosEnd);
        name = name.Replace(" ","").Replace("\\","").Replace("\"","").Replace("+","").Replace(";","").Replace(".","");

        if (raumListe){
          raumListe = raumListe # ";";
        }
        raumListe = raumListe # resId;
        if (raumNamen){
          raumNamen = raumNamen # ";";
        }
        raumNamen = raumNamen # name;
      }

=======
        
        !// Namen bereinigen
        name = res.Substr(0,iPosEnd);
        name = name.Replace(" ","").Replace("\\","").Replace("\"","").Replace("+","").Replace(";","").Replace(".","");
        
        if (raumListe){
          raumListe = raumListe # ";";            
        }          
        raumListe = raumListe # resId;
        if (raumNamen){
          raumNamen = raumNamen # ";";            
        }          
        raumNamen = raumNamen # name;            
      }      
      
>>>>>>> Stashed changes
      !// Wenn wir eine Raumliste haben dann setzen wir, wenn diese nicht leer ist
      svObj = dom.GetObject(vrp#"HK1-R-Liste");
      if(svObj.State()!=""){
        !// Ist schon gesetzt
        WriteLine("Variable HK1-R-Liste ist bereits gefüllt: " # svObj.State());
        WriteLine("Ermittelte Daten: " # raumListe);
      }else{
        svObj.State(raumListe);
<<<<<<< Updated upstream
        WriteLine("Variable HK1-R-Liste wird gesetzt auf: " # raumListe);
      }

=======
        WriteLine("Variable HK1-R-Liste wird gesetzt auf: " # raumListe);        
      }
        
>>>>>>> Stashed changes
      !// Raum Variablen anlegen und HK2-HKG-Liste füllen
      string hkgListe = "";
      foreach(name,raumNamen.Split(";")){
        !// Variablen Name erzeugen
        name = raumNamenPreFix # name;
        if (hkgListe){
          hkgListe = hkgListe # ";";
        }
        hkgListe = hkgListe+name;
<<<<<<< Updated upstream

=======
        
>>>>>>> Stashed changes
        svObj = dom.GetObject(name);
        if (svObj){
          WriteLine("Raumvariable " # name # " existiert bereits! Wert: " # svObj.State());
        }else{
          !// Variable anlegen
          object svObjects = dom.GetObject(ID_SYSTEM_VARIABLES);
          svObj = dom.CreateObject(OT_VARDP);
          svObjects.Add(svObj.ID());
<<<<<<< Updated upstream

=======
    
>>>>>>> Stashed changes
          svObj.Name(name);
          svObj.ValueType(ivtString);
          svObj.ValueSubType(istChar8859);
          svObj.DPInfo("Raumbeschreibung für HK-Skript 2");
          svObj.DPArchive(true);
          svObj.Internal(false);
          svObj.Visible(true);
<<<<<<< Updated upstream
          svObj.DPArchive(true);

          !// Vorgabe machen
          svObj.State(raumVorgabePreFix # name.Replace(raumNamenPreFix,"") # raumVorgabePostFix);
          WriteLine("Raumvariable " # name # " angelegt! Wert: " # svObj.State());
        }
      }

=======
          svObj.DPArchive(true);        
                    
          !// Vorgabe machen
          svObj.State(raumVorgabePreFix # name.Replace(raumNamenPreFix,"") # raumVorgabePostFix);
          WriteLine("Raumvariable " # name # " angelegt! Wert: " # svObj.State());       
        }
      }
      
>>>>>>> Stashed changes
      !// HK2-HKG-Liste ändern
      svObj = dom.GetObject(vrp#"HK2-HKG-Liste");
      if(svObj.State()!=""){
        !// Ist schon gesetzt
        WriteLine("Variable " # vrp # "HK2-HKG-Liste ist bereits gefüllt: " # svObj.State());
        WriteLine("Ermittelte Daten: " # hkgListe);
      }else{
        svObj.State(hkgListe);
<<<<<<< Updated upstream
        WriteLine("Variable HK2-HKG-Liste wird gesetzt auf: " # hkgListe);
=======
        WriteLine("Variable HK2-HKG-Liste wird gesetzt auf: " # hkgListe);        
>>>>>>> Stashed changes
      }
    }else{
      !// Kein Zugriff möglich
      WriteLine("Fehler:\n" # stdout # "\n" # stderr);
    }
  }
}

!//------------------------------------------------------------------------------------------------

!// Update der ReGa Skript Engine. Siehe:
!//     https://homematic-forum.de/forum/viewtopic.php?t=45380
dom.RTUpdate(0);

WriteLine("Alles erledigt...!");