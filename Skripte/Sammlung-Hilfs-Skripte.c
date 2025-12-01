!// Sammlung von Hilfscode
!//================================================================================================
!// Stand:    27.11.2025; 
!// Autor:    Martin Richter    (heizkalender@m-ri.de)
!// Projekt:  Helmut Diedrichs
!//================================================================================================
!// Dieser Code wurde im Rahmen der Heizkalender-Implementierung der Baptisten Gemeinde Hanau 
!// entwickelt.
!// Das Heizkalender-Team freut sich, dass Sie den kostenlosen Heizkalender anwenden und somit 
!// einen Beitrag zum Umweltschutz leisten. Und es wäre schön, wenn Sie die Nutzung per E-Mail an 
!// Info@Heizkalender.de melden. Dadurch ergäbe ich eine Übersicht und zudem die Möglichkeit auf 
!// wichtige Änderungen hinzuweisen. Gerne können Sie auch über Ihre Erfahrung mit dem Heizkalender 
!// berichten.
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
string pfad="/media/usb1/HK-Log_20251118.log";

command = "cat "+pfad;
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
system.Exec(command, &stemp, &error);
WriteLine(stemp+"\n");

!//------------------------------------------------------------------------------------------
!// Urlaub Start und Ende setzen
WriteLine(dom.GetObject("VirtualDevices.INT0000003:1.PARTY_TIME_START").State());
dom.GetObject("VirtualDevices.INT0000003:1.PARTY_TIME_START").State( @2000-01-01 00:00@);
WriteLine(dom.GetObject("VirtualDevices.INT0000003:1.PARTY_TIME_END").State());
dom.GetObject("VirtualDevices.INT0000003:1.PARTY_TIME_END").State(@2000-01-01 00:00@);
WriteLine("Test");