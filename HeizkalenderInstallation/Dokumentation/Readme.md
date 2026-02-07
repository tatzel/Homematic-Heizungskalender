# Heizkalender-Installer

Der Heizkalender -Installer ist ein Tool zur Erstellung von Heizkalendern für die HommMatic, basierend auf Daten aus ChurchTool, ChurchDesk, iCal oder Google Kalender.
Der Installer ermöglicht die Neuinstallation oder auch Updates bestehender Installationen. Er bietet eine benutzerfreundliche Oberfläche zur Konfiguration der Heizkalender und unterstützt die Generierung von Skripten und notwednigen Systemvariablen, die in der HomeMatic CCU oder ähnlichen Systemen verwendet werden können.

## Systemvoraussetzungen

- Betriebssystem: Windows
- Die CCU muss in den Sicherheitseinstellungen den Zugriff auf die Remote Homematic-Script API erlauben. Hier muss entweder ein eingeschränkter Zugriff auf die benötigten Funktionen oder ein vollständiger Zugriff gewährt werden, damit der Installer die notwendigen Skripte und Variablen erstellen kann.
- Ein Administrator Benutzer und das entsprechende Kennwort müssen bekannt sein.
- Alle Skripte die installiert und werden sollen müssen im Programmverezichnis des Heizkalender-Installers liegen. Die Namen sind vorgegeben und dürfen nicht verändert werden. Es können aber weitere Tool-Skripte hinzugefügt werden, die dann ebenfalls aktualisiert werden.
- Um auf Ressourcen und externe Kalender zugreifen zu können , muss der Rechner mit dem Internet verbunden sein.
- Eine Lauffähige Kopie des Installers liegt im Skripte Verzeichnis.

![Mit CCU verbinden](Bilder\HKI-Verbinden.png)

![Verbunden mit der CCU](Bilder\HKI-Verbunden.png)

![Allgemeinde Einstellungen](Bilder\HKI-Einstellungen.png)

![Kalenderzuordnung](Bilder\HKI-Kalenderzuordnung.png)

![Raumzuordnung zu den Kalenderressourcen](Bilder\HKI-Raumzuordnung.png)

![Übersicht der Programme](Bilder\HKI-Programme.png)

![Übersicht der Räume](Bilder\HKI-Räume.png)

![Raum-Eigenschaften](Bilder\HKI-Raum.png)

![Übersicht der Systemvariablen](Bilder\HKI-Systemvariablen.png)
