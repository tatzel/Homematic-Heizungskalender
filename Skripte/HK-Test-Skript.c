!// Skript zum Testen der Einstellungen für den Heizkalender.
!//================================================================================================
!// Stand:    22.11.2025; 
!// Autor:    Martin Richter    (heizkalender@m-ri.de)
!// Projekt:  Helmut Diedrichs  (helmut@diedrichs.de)
!//================================================================================================
!// Dieser Code wurde im Rahmen der Heizkalender-Implementierung der Baptisten Gemeinde Hanau 
!// entwicklet.
!// Die Nutzung ist kostenlos, aber wir bitten die Nutzung an einer der obigen Email Adressen zu 
!// melden.
!//================================================================================================
!//
!//  Alle aktuellen Raumvariablen werden dekodiert, alle Einstellungen werden im Klartext 
!//  ausgegeben.
!//  Anleitung um das Skript auszuführen: 
!//    WebUI der CCU öffnen
!//    Kopieren sie den Inhalt dieses Datei komplett und unverändert in das Fenster:
!//    => Programm und Verknüpfungen => Skripte Testen
!//    Betätigen sie den Button "Ausführen", im Ausgabe Fenster findest sich das Ergebnis
!//    das sich auch als Dokumentation eignet.

!//------------------------------------------------------------------------------------------
!// Heizliste dekodieren und prüfen

string vrp="";

!//#######---Ende Variabler Bereich---#############################################################
!//Im Folgenden Hier keine Veränderungen vornehmen!

string stemp="";
string stext="";
integer i=0;

!//------------------------------------------------------------------------------------------
!// Dekodieren der Schaltliste in Klartext

WriteLine("___________________________________________________________________________");

string SListe=dom.GetObject(vrp # "HK1-Schaltliste").State();
WriteLine("HK1-Schaltliste=" # SListe);
i = 0;
while (true) {
  stemp = SListe.StrValueByIndex(";",i);
  if(stemp==""){
    break;
  }
  if ((i%5)==0){
    WriteLine("\nSchaltzeiten Raum=" + stemp);
  } elseif ((i%5)==1) {
    WriteLine("Zeit Start=" + stemp.ToInteger().ToTime().ToString());
  } elseif ((i%5)==2) {
    WriteLine("Zeit Ende=" + stemp.ToInteger().ToTime().ToString());
  } elseif ((i%5)==3) {
    WriteLine("Temperatur=" + stemp);
  } elseif ((i%5)==4) {
    WriteLine("Heizen/Schalten=" + stemp);
  } 
  i=i+1;
}

!//------------------------------------------------------------------------------------------
!// Ausgabe Der Churchtools Raumliste und deren Raum Parameter

WriteLine("___________________________________________________________________________");

string hk1RaumListe=dom.GetObject(vrp # "HK1-R-Liste").State();
string hk1RaumListeNamen=dom.GetObject(vrp # "HK1-R-ListeNamen").State();
string hk2RaumListe=dom.GetObject(vrp # "HK2-HKG-Liste").State();

WriteLine("HK1-R-Liste=" # hk1RaumListe);
WriteLine("HK1-R-ListeNamen=" # hk1RaumListeNamen);
WriteLine("HK2-HKG-Liste=" # hk2RaumListe);

i=0;
string RListe;
string RName;
var Raum;
string RaumDef;
foreach(RListe, hk2RaumListe.Split(";")){
  WriteLine("_____________________________\nRaum: \t" # (i+1).ToString());
  WriteLine("Chruchtools Resource: \t" # hk1RaumListe.StrValueByIndex(";",i));
  if (RListe.Find("+")>=0){
    WriteLine ("Zugeordnete Raeume: \t" # RListe);
  }
  foreach(RName,RListe.Split("+")){
    WriteLine("\nRaum: " # RName);
    Raum = dom.GetObject(RName);
    if (!Raum){
      WriteLine("FEHLER!!! Raumvariable " # RName # " nicht vorhanden!!!")
    }else{
      RaumDef = Raum.State();
      
      !// Wert #0 Raumstatus
      stemp = RaumDef.StrValueByIndex(";",0);
      stext = "Raum Status: \t" # stemp;
      if(stemp=="0"){
          stext = stext # " ausgeschaltet";
      }elseif(stemp=="1"){
          stext = stext # " eingeschaltet";
      }elseif(stemp=="2"){
          stext = stext # " generell eingeschaltet";
      }elseif(stemp=="3"){
          stext = stext # " generell ausgeschaltet";
      }else{
          stext = stext # " UNBEKANNT!!! FEHLER!!!";
      }
      WriteLine(stext);
      
      !// Wert #1 Heizen Schalten
      stemp = RaumDef.StrValueByIndex(";",1);
      stext = "Modus:   \t" # stemp;
      if(stemp=="H"){
          stext = stext # " Heizen";
      }elseif(stemp=="S"){
          stext = stext # " Schalten";
      }else{
          stext = stext # " UNBEKANNT!!! FEHLER!!!";
      }
      WriteLine(stext);
      
      !// Wert #2 Gerätebauart
      stemp = RaumDef.StrValueByIndex(";",2);
      stext = "Geraetebauart: \t" # stemp;
      if(stemp=="IP"){
        stext = stext # "=IP-Thermostate-Aktoren-Geraetetyp (Kanal 1)";
      }elseif(stemp=="RT"){
        stext = stext # "=Klassik-Thermostate-Aktoren-Geraetetyp (Kanal 4)";
      }elseif(stemp=="TC"){
        stext = stext # "=Klassik-Thermostate-Aktoren-Geraetetyp (Kanal 2)";
      }elseif(stemp=="IT"){
        stext = stext # "=Klassik-Thermostate-Aktoren-Geraetetyp (Kanal 2)";
      }elseif(stemp=="SW"){
        stext = stext # "=Klassik-Schalter-Aktoren-Geraetetyp (Kanal 1/2)";
      }else{
        stext = stext # " UNBEKANNT!!! FEHLER!!!";
      }
      WriteLine(stext);

      !// Wert #3 Gerätebauart
      WriteLine("Temperatur: \t" # RaumDef.StrValueByIndex(";",3).ToInteger().ToString());
  
      !// Wert #4 Vorlaufzeit
      WriteLine("Vorlaufzeit: \t" # RaumDef.StrValueByIndex(";",4).ToInteger().ToString());
  
      !// Wert #5 Nachlaufzeit
      WriteLine("Nachlaufzeit: \t" # RaumDef.StrValueByIndex(";",5).ToInteger().ToString());
  
      !// Kanäle ausgeben
      string Aktor="";
      string Param="";
      stemp = RaumDef.StrValueByIndex(";",2);
      if(stemp=="RT"){Param="SET_TEMPERATURE";}
	  elseif(stemp=="TC"){Param="SETPOINT";}
	  elseif(stemp=="IP"){Param="SET_POINT_TEMPERATURE";}
      elseif(stemp=="IT"){Param="SET_TEMPERATURE";}
      else{Param="";}
      
      WriteLine("Aktoren:");
      foreach(Aktor,RaumDef.Split(";")){
	    if (Aktor.Length()>4){
          stemp = "\t" # Aktor;
          if (Param!=""){
            var obj = dom.GetObject(Aktor);
            if (obj){
              obj=obj.DPByHssDP(Param);
              if (obj){
                stemp = stemp # " \t " # Param # "=" # obj.State();
              }else{
                stemp = stemp # " \t FEHLER!!! Param " # Param # " unbekannt im System!";
              }
            }else{
              stemp = stemp # " \t FEHLER!!! Aktor unbekannt im System!";
            }
          }
          WriteLine(stemp);
        }
      }          
    }
  }
  i = i+1;
}   

!//------------------------------------------------------------------------------------------
!// Ausgabe aller Systemvariablen. Damit kann man Einstellungen protokollieren.

WriteLine("___________________________________________________________________________");
WriteLine("System Variablen zum Heizkalender\n");

string vrp="";
string svListStr = "";
string vid; 
var svIDs = dom.GetObject(ID_SYSTEM_VARIABLES).EnumIDs();
 
foreach(vid, svIDs){
    var sysVar = dom.GetObject(vid);
    if (sysVar.Name().StartsWith(vrp # "HK")) {
      svListStr = svListStr # sysVar.Name() # "=" #  sysVar.Value() # "\n";
    }
}
 
WriteLine(svListStr);

!//------------------------------------------------------------------------------------------
!// Logging überprüfen

WriteLine("___________________________________________________________________________");
WriteLine("Logging Status\n");

obj=dom.GetObject(vrp # "HK1-Logging");
if (obj){
  if (obj.State()!=0){
    stemp = "ein";
  }else{
    stemp = "aus";
  }
  WriteLine("HK1-Logging: Logging Script 1 ist " # stemp # "geschaltet");
}else{
  WriteLine("HK1-Logging: Log-Variable fuer Script 1 existiert nicht");
}
if (dom.GetObject(vrp # "HK2-Log")){
  WriteLine("HK1-Log: Log-Variable fuer Script 1 existiert");
}else{
  WriteLine("HK1-Log: Log-Variable fuer Script 1 existiert nicht");
}

obj=dom.GetObject(vrp # "HK2-Logging");
if (obj){
  if (obj.State()!=0){
    stemp = "ein";
  }else{
    stemp = "aus";
  }
  WriteLine("HK2-Logging: Logging Script 2 ist " # stemp # "geschaltet");
}else{
  WriteLine("HK2-Logging: Log-Variable fuer Script 1 existiert nicht");
}
if (dom.GetObject(vrp # "HK2-Log")){
  WriteLine("HK2-Log: Log-Variable fuer Script 2 existiert");
}else{
  WriteLine("HK2-Log: Log-Variable fuer Script 2 existiert nicht");
}

WriteLine("___________________________________________________________________________");
