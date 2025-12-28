!// Sammlung von Hilfscode
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

!//------------------------------------------------------------------------------------------
!// Ausgabe aller Systemvariablen. Damit kann man Einstellungen protokollieren.

string svListStr = "";
string vid; 
var svIDs = dom.GetObject(ID_SYSTEM_VARIABLES).EnumIDs();
 
foreach(vid, svIDs){
    var sysVar = dom.GetObject(vid);
    if (sysVar.Name().StartsWith("HK")) {
      svListStr = svListStr # sysVar.Name() # "=" #  sysVar.Value() # "\n";
    }
}
 
WriteLine(svListStr);

!//------------------------------------------------------------------------------------------
!// Dekodieren der Schaltliste in Klartext

string SListe=dom.GetObject("HK1-Schaltliste").State();
string stemp="";
integer i=0;
WriteLine(SListe);
while (SListe.StrValueByIndex(";",i)!="") {
  stemp = SListe.StrValueByIndex(";",i);
  if ((i%5)==0){
    WriteLine("\nRaum=" + stemp);
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
!// Wenn man das alte Logging auf USB benutzt kann man sich so die Datei komplett 
!// anzeigen lassen.

string command;
string stemp;
string error;
string pfad="/media/usb1/HK-Log_20251118.log";

command = "cat "+pfad;
stemp="";
system.Exec(command, &stemp, &error);
WriteLine(stemp+"\n");

!//------------------------------------------------------------------------------------------
!// Wenn man das alte Logging benutzt kann man sich so die letzten 100 Zeilen anzeigen 
!// lassen von der Datei, die normalerweise auf dem USB Stick erzeugt wird.

string stemp;
string command;
string pfad="HK-Log_20251117.log";

command = "tail -n 100 '"+pfad+"'";
WriteLine(command);
stemp="";
system.Exec(command, &stemp, &error);
WriteLine(stemp+"\n");

!//------------------------------------------------------------------------------------------
!// Urlaub Start und Ende setzen
WriteLine(dom.GetObject("VirtualDevices.INT0000003:1.PARTY_TIME_START").State());
dom.GetObject("VirtualDevices.INT0000003:1.PARTY_TIME_START").State( @2000-01-01 00:00@);
WriteLine(dom.GetObject("VirtualDevices.INT0000003:1.PARTY_TIME_END").State());
dom.GetObject("VirtualDevices.INT0000003:1.PARTY_TIME_END").State(@2000-01-01 00:00@);
WriteLine("Test");

!//------------------------------------------------------------------------------------------
!// Schaltparameter ermitteln

!// IP- Thermostate-Aktoren-Gerätetyp (Kanal 1)
!//   BWTH_V1, BWTH_V2, TRVB_V1, TRV-C_V1, TRV, TRV-V1, TRV-V2, TRV-V3, TRV-V4, C_V2, WTH-2_V1, WTH-2_V2, WTH2_V3, WTH-BV1, WTH_V1, WT-V1
string AGFParamIP="SET_POINT_TEMPERATURE";

!// RT- Kennung Kanal Klassik-Thermostate-Aktoren-Gerätetyp (Kanal 4)
!//   HM-CC-RT-DN HM-CC-RT-DN
string AGFParamRT="SET_TEMPERATURE";

!// TC- Kennung Kanal Klassik-Thermostate-Aktoren-Gerätetyp (Kanal 4)
!//   HM-CC-TC
string AGFParamTC="SETPOINT";

!// IT- Kennung Kanal Klassik-Thermostate-Aktoren-Gerätetyp (Kanal 4)
!//   HM-TC-IT-WM-W-EU
string AGFParamIT="SET_TEMPERATURE";

!// SW- Kennung Kanal Klassik-Schalter-Aktoren-Gerätetyp (Kanal 1 bzw. 2)
!//   HM-LC-Sw1-FM HM-LC-Sw1PBU-FM, HM-LC-Sw2-FM, HM-ES-PMSw1-DR HM-LC-Sw1-PCB
!// Kennung Kanal IP-Schalter-Aktoren-Gerätetyp (Kanal 1)
!//   SW  Noch unerprobt
string AGFParamSW="STATE";

string AGFs="IP;RT;TC;IT;SW;falsch";
string AGF;
foreach(AGF,AGFs.Split(";")){
  string AGFParam = "AGFParam"#AGF.ToUpper();
  string Param = AGFParamIP;
  if (system.IsVar(AGFParam)){
    Param = system.GetVar(AGFParam);
  }
  WriteLine("AGF=" # AGF # "->" # Param);
}
