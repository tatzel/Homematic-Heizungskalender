# HomeMatic-Heizungskalender
Heizkalender für die Steuerung der Homematic über ChurchTool, ChurchDesk, iCal oder Google.
Es umfasst Skripte, Tools un eine Installations- und Update Software. Intern gesteuert werden die Skripte über Systemvariablen.

# Skripte

Die Skripte sind das Kernstück, des Heizkalenders. Sie umfassen 3 Gruppen.
1. Einlesen von Terminen aus unterschiedlichen Quellen (Google, iCal, ChurchDesk iCal/API und ChurchTools)
2. Schalten der Themrostate und Hilfsmittel zur Überwachung
3. Diverse andere optionale Hilfsmittel

Intern gesteuert werden die Skripte durch Systemvariablen, die das Verhalten der Skripte kontrollieren. Die Skripte selbst sind auf allen Installationen austauschbar.

Die Hauptskripte und eine ausführbare Version der HeizkalenderInstallation findet sich [hier](/Skripte). Eine Zusammenfassung der Skripte und eine Beschreibung ist [hier](/Skripte/Readme.md) zu finden
Die Tools und weitere Beispielskripte finden sich [hier](/Tools). Eine Zusammenfassung der Tools und eine Beschreibung ist [hier](/Tools/Readme.md) zu finden

# Heizkalender-Installation

Der Heizkalender-Installer ist ein Tool zur Einrichtung und Updates der Heizkalendern Software auf einer CCU. 
Der Installer ermöglicht die Neuinstallation oder auch Updates bestehender Installationen. Er bietet eine benutzerfreundliche Oberfläche zur Konfiguration der Heizkalender und unterstützt die Generierung von Skripten und notwendigen Systemvariablen, die in der HomeMatic CCU oder ähnlichen Systemen verwendet werden können.

[Weitere Informationen und eine Anleitung finden sich hier.](/HeizkalenderInstallation/Dokumentation/Readme.md)

## Systemvoraussetzungen für die Heizkalender-Installation

- Betriebssystem: Windows
- Die CCU muss in den Sicherheitseinstellungen den Zugriff auf die Remote Homematic-Script API erlauben. Hier muss entweder ein eingeschränkter Zugriff auf die benötigten Funktionen oder ein vollständiger Zugriff gewährt werden, damit der Installer die notwendigen Skripte und Variablen erstellen kann.
- Ein Administrator Benutzer und das entsprechende Kennwort müssen bekannt sein.
- Alle Skripte die installiert und werden sollen müssen im Programmverezichnis des Heizkalender-Installers liegen. Die Namen sind vorgegeben und dürfen nicht verändert werden. Es können aber weitere Tool-Skripte hinzugefügt werden, die dann ebenfalls aktualisiert werden.
- Um auf Ressourcen und externe Kalender zugreifen zu können , muss der Rechner mit dem Internet verbunden sein.
- Eine Lauffähige Kopie des Installers liegt im Skripte Verzeichnis.

# Allgenmeines

Der Heizkalender ist eine Idee von Helmut W. Diedrichs und wurde erstmals 2019 in der
Stadtmission Arheilgen angewendet Lukas Helduser entwickelte 2023 auf der Bais von Homematic
das Heizkalender-Programm für die Allgemeinheit, inkl. Varianten.
Dank an die seitherigen Anwender für ihre Verbesserungsvorschläge, insbesondere an die Pilot-
Gemeinden. Dieser Code wurde im Rahmen der Heizkalender-Implementierung der Baptisten Gemeinde
Hanau von Martin Richter optimiert.

Das Heizkalender-Team freut sich, dass Sie den kostenlosen Heizkalender anwenden und somit einen
Beitrag zum Umweltschutz leisten. Es wäre schön, wenn Sie die Nutzung per E-Mail anzeigen an: info@heizkalender.de

Dadurch ergäbe ich eine Übersicht und die Möglichkeit auf Änderungen hinzuweisen. Bitte
berichten auch Sie über Ihre Erfahrung mit dem Heizkalender.

## Lizenz

Der Heizkalender-Installer ist freie Software.

Copyright (C) 2026 by Martin Richter (xMRi-Software)

Weitere Teile Copyright (C) 2026 by Team Heizkalender: Lukas Helduser, Martin Richter (xMRi-Software), Helmut Diedrichs
Im Detail ist dies in den Köpfen der Skripte und Sourcecodes zu lesen.

Dieses Programm wird unter den Bedingungen der
**GNU General Public License Version 3 (GPLv3)**
oder – nach Ihrer Wahl – jeder späteren Version veröffentlicht.

Sie dürfen das Programm verwenden, verändern und weiterverbreiten,
sofern alle abgeleiteten Werke ebenfalls unter der GPL lizenziert
werden und der Quellcode verfügbar bleibt.

Dieses Programm wird OHNE JEGLICHE GEWÄHRLEISTUNG bereitgestellt,
auch ohne die implizite Gewährleistung der Marktfähigkeit oder
Eignung für einen bestimmten Zweck.

Der vollständige Text der Lizenz ist in der Datei `LICENSE`
enthalten oder unter folgender Adresse abrufbar:
https://www.gnu.org/licenses/


## Hinweis zur Nutzung mit Homematic / CCU / IoT-Systemen

Diese Software wurde für die unterstützende Konfiguration und
Verwaltung von zeitbasierten Heizungssteuerungen entwickelt,
insbesondere im Umfeld von Homematic-, CCU- und vergleichbaren
IoT-Systemen.

Der Heizkalender-Installer steht in keiner Verbindung zur
eQ-3 AG oder anderen Herstellern von Smart-Home-Komponenten.

Die Software:
- ersetzt keine sicherheitsrelevanten Schutzmechanismen
- greift nicht eigenständig in Regelungs- oder Notfallfunktionen ein
- übernimmt keine Überwachung von Temperatur-, Frost- oder
  Sicherheitsgrenzwerten

Skripte, Programme oder Konfigurationen, die durch diese Software
erstellt oder verändert werden, sollten vor dem Einsatz in
produktiven Systemen sorgfältig geprüft werden.

Der Autor übernimmt keine Haftung für Schäden an:
- Homematic-Zentralen (CCU, RaspberryMatic, debmatic)
- Aktoren, Sensoren oder Heizungsanlagen
- angebundene IoT- oder Smart-Home-Systeme

Die Nutzung erfolgt ausschließlich auf eigene Verantwortung.
Vor jeder Änderung wird dringend empfohlen, ein vollständiges
Backup der CCU-Konfiguration anzufertigen.
