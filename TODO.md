# Offene Punkte

Diese Datei sammelt offene Punkte und noch zu klärende Fragen des Projekts, für
die (noch) kein eigenes GitHub-Issue existiert. Erledigte Punkte werden entfernt
(die Historie steht im Git-Log und im [CHANGELOG](CHANGELOG.md)).

## Heizkurve `HK2-Kurve`

- **Ist der ausgelieferte Default noch aktuell?** Der Installer legt aktuell die
  flache Kurve `162;130;100;59;50;41;30;20` an (max. 162 min ≈ 2,7 h). Die
  produktiv bewährte und in
  [Heizsteuerung-Vorheizzeit.md](Skripte/Dokumentation/Heizsteuerung-Vorheizzeit.md)
  als Beispiel dokumentierte Kurve ist deutlich steiler
  (`451;370;297;196;174;154;125;103`, max. 7,5 h). Zu klären: Soll der steilere
  Wert der neue Default werden? (Abstimmung mit Martin steht aus.)

## Installer-Dokumentation

- In [HeizkalenderInstallation/Dokumentation/Readme.md](HeizkalenderInstallation/Dokumentation/Readme.md)
  sind zwei Abschnitte noch unausgefüllt („Wechsel zu einer anderen
  Ressourcen-Variante" und „Möglichkeit Änderungen in Skripten anzuzeigen",
  jeweils als `- [ ]` markiert). Screenshots/Beschreibung ergänzen.
