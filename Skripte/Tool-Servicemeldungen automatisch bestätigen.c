!// Servicemeldungen automatisch bestätigen
!//================================================================================================
!// Stand:    01.12.2025; 
!// Autor:    Martin Richter    (heizkalender@m-ri.de)
!// Projekt:  Helmut Diedrichs  (helmut@diedrichs.de)
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

!//Eingabe eines Namens Präfix
!//Dies ist nur erforderlich wenn die Namensvorgabe beim erstellen den Systemvariablen geändert wurde.
!//Wird hier ein Präfix eingeben so muss dieser in allen Skripten auch angegeben werden.
string vrp="";

!//Logging in "Log" mit 1 zwingend einschalten oder mit 0 Ausschalten
boolean log=0;

!//#######---Ende Variabler Bereich---#############################################################
!//Im Folgenden Hier keine Veränderungen vornehmen!

var logObj=dom.GetObject(vrp+"HK-Log");
var loggingObj=dom.GetObject(vrp+"HK-Logging");

!// Prüfe logging erwartet wird
if ((!log) && loggingObj){
  if (loggingObj.State()!=0){
    log = true;
  }
}
  
!// Logging auschalten, wenn keine Variable vorhanden
if (!logObj){
  log = false;
}

! HomeMatic-Script
! "KOMMUNIKATION GESTöRT" BEHEBEN
! http://www.christian-luetgens.de/homematic/hardware/funkstoerungen/servicemeldungen/Servicemeldungen.htm

string itemID;
string address;
string name;
object aldp_obj;

if(log){
  logObj.State("Beginn Servicemeldungen automatisch bestätigen");
}

foreach(itemID, dom.GetObject(ID_DEVICES).EnumUsedIDs()) {
  address = dom.GetObject(itemID).Address();
  name = dom.GetObject(itemID).Name();
  aldp_obj = dom.GetObject("AL-" # address # ":0.STICKY_UNREACH");
  if (aldp_obj) {
    if (aldp_obj.Value()) {
      aldp_obj.AlReceipt();
      if(log){
        logObj.State("Kommunikationsstörung:" # name # " Objekt: " # aldp_obj # " Adresse: "# address);
      }
    }
  }
}

if(log){
  logObj.State("Ende Servicemeldungen automatisch bestätigen");
}

!  Ende des Scripts
