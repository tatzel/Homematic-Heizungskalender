!// Gestörte Kommunikation beheben
!//================================================================================================
!// Stand:    01.12.2025; 
!// Autor:    Martin Richter    (heizkalender@m-ri.de)
!// Projekt:  Helmut Diedrichs  (helmut@diedrichs.de)
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
string channel;
var x;
integer max=5;

foreach(itemID, dom.GetObject(ID_DEVICES).EnumUsedIDs()) {
  address = dom.GetObject(itemID).Address();
  name = dom.GetObject(itemID).Name();
  aldp_obj = dom.GetObject("AL-" # address # ":0.UNREACH");
  if (aldp_obj) {
    if (aldp_obj.Value()) {
      foreach (channel, dom.GetObject(itemID).Channels().EnumUsedIDs()) {
        if (max > 0) { 
          if(log){
            logObj.State("Kommunikationstest:" # name # " Objekt: " # aldp_obj # " Adresse: "# address);
          }
          x = dom.GetObject(channel).State();
          max = max - 1;
        }
      }
    }
  }
}

!  Ende des Scripts
