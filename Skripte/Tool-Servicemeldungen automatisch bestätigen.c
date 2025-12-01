  !// Servicemeldungen automatisch bestätigen
  !//================================================================================================
  !// Stand:    29.11.2025; 
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

  ! HomeMatic-Script
  ! "KOMMUNIKATION GESTöRT" BEHEBEN
  ! http://www.christian-luetgens.de/homematic/hardware/funkstoerungen/servicemeldungen/Servicemeldungen.htm

  string itemID;
  string address;
  object aldp_obj;

  foreach(itemID, dom.GetObject(ID_DEVICES).EnumUsedIDs()) {
     address = dom.GetObject(itemID).Address();
     aldp_obj = dom.GetObject("AL-" # address # ":0.STICKY_UNREACH");
     if (aldp_obj) {
       if (aldp_obj.Value()) {
         aldp_obj.AlReceipt();
       }
     }
  }

  !  Ende des Scripts
