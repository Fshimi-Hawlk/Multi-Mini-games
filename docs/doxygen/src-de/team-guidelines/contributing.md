@page contributing Förderleitlinien

@ref index "Back to Home"

**Letzte Überprüfung gegen Projektstruktur:* 16. März 2026

Wir sind ein kleines Team von vier Studenten, die Git-Zusammenarbeit lernen, wie wir gehen.
Diese Seite beschreibt unseren aktuellen Workflow für Commits, Branchen, Pull Requests und Bewertungen (Stand: März 2026).
Obwohl das Projekt noch WIP ist, haben wir bereits Grundregeln definiert, um die Dinge organisiert und nachvollziehbar zu halten.

Das wichtigste Prinzip bleibt gleich: kommunizieren offen im Gruppenchat, wenn etwas unklar ist.

## Aktuelle aktive Branchen & Workflow Überblick

`main`ist derzeit pausiert und meist veraltet.
Aktive Entwicklung geschieht auf diesen Zweigen:

- Individuelle Spielzweige (z.`tetris`,`snake-classic`,`block-blast`, etc.) -> einzelspieler-spielentwicklung und politur
- Ja`docs-setup`-> dokumentation, code-stil, konventionen, setup guides
- Ja`sub-games-integration-test`(wir nennen es normalerweise`sgit`) -> lobby-verbesserungen, spielintegration, geteilte fehlerbehebungen

Typischer Fluss für ein Teammitglied:
1. Ziehen Sie die neueste aus der Branche, an der Sie arbeiten
2. Arbeiten Sie an Ihrer Funktion / Spiel / Fix
3. Commit mit klaren Nachrichten (siehe unten)
4. Drücken Sie Ihre Branche
5. Öffnen Sie eine Pull-Anfrage (in der Regel ansprechend)`sgit`für integrationen oder den betreffenden gemeinsamen zweig)
6. Fragen Sie nach Bewertungen in Gruppenchat
7. Feedback korrigieren -> einmal zugelassen und in der Lobby getestet ->

Einmal Spiele sind solide in Einzelspieler auf`sgit`, wir werden später alles in`main`(einschließlich zukünftiger multiplayer-teile).

Für geplante Schritte, aktueller Status und langfristige Vision siehe @ref roadmap "Roadmap"

## Nachrichten ausgeben

Wir folgen einem vereinfachten **Conventional Commits* Stil.
Es hilft jedem, Änderungen schnell während der Bewertungen und in git log zu verstehen.

Häufige Präfixe verwenden wir am häufigsten:

- Ja`feat:`neue funktion / sichtbare veränderung
- Ja`fix:`fehlerbehebung
- Ja`docs:`dokumentation, kommentare, anleitungen
- Ja`style:`formatierung, whitespace, benennung (keine logikänderung)
- Ja`refactor:`codereinigung / bessere struktur (keine verhaltensänderung)
- Ja`test:`tests hinzugefügt oder fixiert
- Ja`chore:`wartung, werkzeug, .gitignore, kleine aufräumarbeiten
- Ja`build:`Makefile, Flags erstellen, Compiler-Optionen

Wir verwenden manchmal auch:
- Ja`fetch`/`merge`- > veränderungen aus einem anderen zweig
- Ja`undo`- > deutliches entfernen von etwas früher hinzugefügt

Beispiele aus unseren jüngsten Verpflichtungen:
- Ja`feat: added hold piece mechanic in Tetris`
- Ja`fix: resolved player getting stuck on lobby platforms`
- Ja`docs: updated API conversion guide with new error codes`
- Ja`refactor: moved player globals into BallSystem_St`
- Ja`undo: removed temporary pause menu draft`

Für sehr kleine Veränderungen (Typo, Trailing Space) machen wir entweder eine winzige`style:`/`chore:`begehen oder falten sie es in die nächste reale verpflichtung - beide sind in ordnung.

Optional aber empfohlen: Fügen Sie eine **one-line-Zusammenfassung** an der Spitze längerer Commit-Nachrichten hinzu.
Es macht`git log --oneline`und GitHub Commit Listen viel einfacher zu scannen.

Beispiel guter neuer Commit-Nachricht:

```text
Docs update / less globals / temporary pause menu removal / fixes / new font
- docs: Updated creation date and last updated dates
- chore: Removed unused windowRect and appFont from globals
- refactor: Moved game globals to BallSystem_St fields
- undo: Removed pause menu draft - will reimplement later in lobby context
- fix: Fixed font size warning display
- fix: Can now place daub immediately when ball text appears
- chore: Switched font to NotoSansMono-Bold.ttf
```

Wir sind nicht streng über Präfixe — Klarheit und hilfreiche Geschichte Materie mehr als perfekte Kategorisierung.
Wenn Sie unsicher sind, wählen Sie den nächsten oder fragen Sie im Chat.

## Branch Naming für Pull Requests

Wir verwenden dieses Muster für PR-Zweigstellen:

`<target-branch>-PR<day-sequence-number>-DD/MM`

Beispiele:
- Ja`tetris-PR1-16/03`
- Ja`sgit-PR3-16/03`
- Ja`docs-setup-PR2-17/03`

Dies macht es offensichtlich, was die Branche Ziele, wenn es geschaffen wurde, und seine Ordnung an diesem Tag.

## Anfrage an Pull

PRs sind unsere wichtigste Möglichkeit, Änderungen sicher zu überprüfen, zu diskutieren und zu integrieren.

### Vor dem Öffnen einer PR
- Lauftests:`make MODE=clang-debug run-tests`(oder zumindest`make tests`)
- Stellen Sie sicher, dass es ohne Warnungen/Fehler baut

### Erstellung der PR-Branche (zwei häufige Fälle)

**Sache 1: Hilfe bei einem Spiel/Feature-Zweig** (z.B. Verbesserung`tetris`)

```bash
git fetch origin
git checkout tetris
git pull origin tetris
git checkout -b tetris-PR1-16/03
# work -> commit -> ...
git push origin tetris-PR1-16/03
```

**Sache 2: Integrieren in den gemeinsamen Zweig** (z.`sgit`,`docs-setup`)

```bash
git fetch origin
git checkout sub-games-integration-test
git pull origin sub-games-integration-test
git checkout -b sgit-PR1-16/03
# work -> commit -> ...
git push origin sgit-PR1-16/03
```

### Eröffnung und Überprüfung
- Titel: kurz und klar (z.B. "Fix Kollision in Lobby-Plattformen")
- Beschreibung: erklären, was + warum, link verwandte Punkte / Ausgaben, wenn eine
- Rezensionen hinzufügen:
- Spielzweige -> hauptsächlich der Filialeigentümer
- Geteilte Zweige -> alle anderen Teammitglieder
- Ping Group chat: "geöffnet PR #X für Rezension"

Wir öffnen uns oft als Draft PR früh, um Feedback zu erhalten, bevor es fertig ist.

### Fusionsregeln (Dampfkonvention)
- Game/feature branchen -> braucht mindestens Genehmigung von dem Filialeigentümer
- nur der filialeigentümer (oder delegierte) sollte sich zusammenfügen
- Geteilte Zweige (`sgit`,`docs-setup`,`main`, etc.) -> braucht zustimmung aller anderen teammitglieder
-> nur der PR-Autor fusioniert, wenn die Genehmigungen in (wenn nicht anders gefragt)
- Verwenden Sie **"Create a merge commit"** (Standard) -> hält vollständige Autorschaft und körnige Geschichte
- Nicht Squash oder Rebase, es sei denn, das Team stimmt ausdrücklich für diese PR

### Nach dem Zusammenschluss
- Löschen Sie die PR-Zweig von GitHub (Schläft Filialen Tab sauber)
- Neueste am Zielzweig ziehen
- Die Geschichte bleibt über Merger Commit vollständig nachverfolgbar

## Code Style

Siehe @ref code_style_and_conventions "Code Style & Conventions" für Namen, Formatierung, Dateilayout, etc.

Es basiert hauptsächlich auf dem Beginn des Projekts – Ziel ist die grundlegende Konsistenz, nicht die perfekte Uniformität.
Wenn etwas unannehmlich oder falsch fühlt, bringen Sie es in Chat oder öffnen Sie eine PR, um den Stilführer zu verbessern.

## Bewertungen und Fragen

- Ping the group chat beim Öffnen einer PR oder wenn Sie Eingabe benötigen
- Sie fragen: "Laut das gut aus?" oder "Was denkst du über diesen Ansatz?" ist ganz normal
- Konflikte zu verschmelzen — nur um Hilfe bitten, überhaupt kein Problem

Vielen Dank für die Zusammenarbeit – nehmen Sie Ihre Zeit, Fragen stellen, genießen Sie den Prozess!

**Erstellt:* 15. Januar 2026
**Letzte Aktualisierung:* 16. März 2026
**Autor:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"