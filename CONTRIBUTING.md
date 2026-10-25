# Beitragen zum HomeMatic-Heizungskalender

Danke für dein Interesse am Projekt! Dieser Leitfaden erklärt, wie du Fehler
meldest, Verbesserungen vorschlägst und Code beiträgst. Er orientiert sich an
den [GitHub Community Standards](https://docs.github.com/de/communities).

## Inhaltsverzeichnis

- [Fehler melden / Ideen einreichen](#fehler-melden--ideen-einreichen)
- [Entwicklungsumgebung](#entwicklungsumgebung)
- [Branching-Modell](#branching-modell)
- [Änderungs-Checkliste](#änderungs-checkliste)
- [Pull Request erstellen](#pull-request-erstellen)
- [Skript-Syntax (HomeMatic HSC)](#skript-syntax-homematic-hsc)
- [Neue Systemvariablen](#neue-systemvariablen)

---

## Fehler melden / Ideen einreichen

Bitte prüfe zuerst, ob bereits ein [Issue](../../issues) zu deinem Thema existiert.
Falls nicht, lege ein neues Issue an und beschreibe:

- Was du erwartest und was stattdessen passiert
- Welche CCU-Version und welche Skript-Variante (z.B. `HK-Skript 1_iCal`) du einsetzt
- Den Datums-Stand der Skripte (steht im Datei-Header als `!// Stand:`)
- Relevante Ausschnitte aus dem HomeMatic-Protokoll, falls vorhanden

---

## Entwicklungsumgebung

Das Projekt besteht aus drei unabhängig entwickelbaren Teilen:

**HomeMatic-Skripte (`.hsc`-Dateien):** Kein spezielles Build-Tooling nötig.
Die Skripte werden direkt auf der CCU ausgeführt. Zum Testen kannst du sie über
die CCU-WebUI unter "Einstellungen → Skripte → Skript testen" ausführen.

**HeizkalenderInstallation (Windows, C++/MFC):** Wird mit Visual Studio gebaut.
Projektdatei: `HeizkalenderInstallation/HeizkalenderInstallation.vcxproj`.
Näheres beschreibt die
[Entwickler-Dokumentation](Dokumentation/Entwickler.md#build-der-heizkalenderinstallationexe-windows-visual-studio).

**Heizkalender Addon (Tcl, CGI):** Gebaut mit `Addon/build.sh` (Linux/macOS).
Näheres beschreibt die
[Addon-Entwicklerdokumentation](Addon/docs/Entwickler.md).

---

## Branching-Modell

Änderungen an diesem Repository laufen ausschließlich über einen Fork und einen
Pull Request. Direkte Commits auf `main` oder auf andere Branches dieses
Repositories sind nicht vorgesehen.

Vorgehensweise:

1. Forke das Repository auf deinen eigenen GitHub-Account.
2. Lege in deinem Fork einen Feature-Branch an, z.B. `feat/meine-aenderung`.
3. Entwickle und committe dort.
4. Stelle einen Pull Request gegen `main` dieses Repositories.

---

## Änderungs-Checkliste

Jede inhaltliche Änderung an einer `.hsc`-Datei oder Installer-Quelle erfordert
**alle** folgenden Schritte (nicht nur die eigentliche Code-Änderung):

1. **Stand aktualisieren:** Datum in der `!// Stand:`-Zeile im Datei-Header auf
   das aktuelle Datum setzen (Format `TT.MM.JJJJ`).
2. **Changelog im Header fortschreiben:** Neuen `!// MRi: JJJJ-MM-TT`-Eintrag
   ganz oben in den Changelog-Block des Headers einfügen (neueste Einträge
   oben). Kurz beschreiben, was und warum geändert wurde.
3. **Dokumentation prüfen:** Neue oder umbenannte Systemvariablen müssen in der
   [Skript-Referenz](Skripte/Dokumentation/Readme.md) erscheinen.
4. **CHANGELOG.md fortschreiben:** Eintrag unter dem passenden Datums-Stand
   ergänzen (Keep-a-Changelog-Format, Kategorien Hinzugefügt/Geändert/
   Behoben/Entfernt). Die Doku muss vorher stimmen.

Ausführliche Hinweise stehen in der
[Entwickler-Dokumentation](Dokumentation/Entwickler.md#änderungs-workflow).

---

## Pull Request erstellen

Beim Erstellen eines Issues oder Pull Requests wird automatisch ein Template vorausgefüllt.

- Beschreibe kurz, **was** geändert wurde und **warum**.
- Verweise auf das zugehörige Issue, falls vorhanden (`Closes #42`).
- Stelle sicher, dass die [Änderungs-Checkliste](#änderungs-checkliste)
  vollständig abgearbeitet ist.
- Alle Skripte eines Releases müssen zum gleichen Datums-Stand passen.

---

## Skript-Syntax (HomeMatic HSC)

Ein paar Besonderheiten der HomeMatic-Skriptsprache, die Neueinsteigerinnen und
Neueinsteigern häufig auffallen:

- Kommentare mit `!` (CCU) bzw. `!//` (kombiniert für Editor-Syntaxhervorhebung)
- Stringverkettung mit `#` statt `+`
- `break` und `continue` werden unterstützt (CCU3/RaspberryMatic, nicht CCU2)
- **Variablennamen dürfen keine deutschen Sonderzeichen enthalten** (ä, ö, ü,
  Ä, Ö, Ü, ß). Erlaubte Zeichen: A-Z, a-z, 0-9, Unterstrich.

Mehr Details: [Entwickler-Dokumentation](docs/heizkalender/entwickler.md#skript-syntax-homematic-hsc).

---

## Neue Systemvariablen

Das Einführen neuer Systemvariablen ist aufwändig, da der Installer sie über
eigene, fest einkompilierte Meta-Skripte anlegt (nicht aus den `.hsc`-Dateien
liest). Vor der Einführung einer neuen Variablen bitte im Issue oder PR
beschreiben, warum ein Hardcode-Wert nicht ausreicht. Näheres:
[Entwickler-Dokumentation](Dokumentation/Entwickler.md#änderungs-workflow).
w).
