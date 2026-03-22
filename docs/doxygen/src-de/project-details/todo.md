@page todo HINWEIS

@ref index "Back to Home"

**Letzte Überprüfung gegen Projektstruktur:* 16. März 2026

Diese Seite listet die wichtigsten offenen Aufgaben für die gesamte Monorepo auf.
Die meisten sind Dinge, die die Lobby, den gemeinsamen Code oder die gesamte Sammlung von Minispielen beeinflussen.

Aufgaben werden grob von "do this soon" sortiert, um "würde später kühl sein".
Wir aktualisieren diese Datei, wie wir gehen - fühlen Sie sich frei, Elemente hinzuzufügen oder Prioritäten nach Gruppendiskussion ändern.

## Hohe Priorität - tun dies bald

- [x] Erstellen Sie eine globale Makefile auf Wurzelebene
- Fertig. Es behandelt jetzt faul Bibliothek baut, kopiert API-Header auf Firstparty/APIs/, unterstützt inkrementelle Builds, Kraft-Wiederaufbau, Debug-Modi, Sanitizer, etc.

- Ende der Wurzel`docs/`ordner richtig
- > Bereits den Ordner erstellt und mehrere Anleitungen dorthin verschoben (`API_Conversion.md`,`CodeStyleAndConventions.md`,`makefile.md`, etc.) die wurzel sauber zu halten.
- Haupt-Doxyfile ist in`docs/`und dokumente lobby + geteilter code
- Nächster: Erweitern Sie die Dokumentation für jedes Spielmodul noch einmal integriert

## Nachdem das erste Spiel vollständig ins Haupt übergeführt ist

- (x) Schreiben Sie klare Regeln, um ein eigenständiges Spiel in eine Lobby-kompatible API zu verwandeln
- > Fertig. Siehe @ref api_conversion "API Conversion & Integration"

- [ ] Speichern / Fortschritt / Einstellungen System
- > Bisherige Optionen: Klartextdateien in`assets/<game-name>/saves/`Einfaches JSON oder etwas Brauches?
-> Plan: Fügen Sie gemeinsam geladene/sparende Helfer hinzu`firstparty/`so kann jedes spiel den gleichen code verwenden

- [ ] Definieren Sie lokale Leaderboard-Format
-> Halten Sie es jetzt einfach (lokale Dateien nur, keine online).
- > Vielleicht JSON oder binär in`assets/leaderboards/`?

## Schön zu haben / längerfristig

- (x) Eine sichtbare Fahrkarte hinzufügen
- > Fertig. Siehe [Gantt-Chart auf GitHub-Projekten](https://github.com/users/Fshimi-Hawlk/projects/1)
- > Zusätzlich zu @ref roadmap "Roadmap"

- [ ] Fügen Sie grundlegende Audioeinstellungen (Volume Slider, Mute Toggles) in der Lobby hinzu
- [ ] Fügen Sie persistente Haut entlocks / Kosmetik über Spiele
- [ ] Experiment mit sehr einfachen lokalen Multiplayer (Split-Screen oder Hotseat)
- [ ] Einfache Fehlerwiederherstellung in der Lobby hinzufügen (Spiel auf Absturz statt Ausstieg laden)

Große Veränderungen sollten im Gruppenchat zuerst diskutiert werden, so dass jeder einverstanden ist.

**Erstellt:* März 02, 2025
**Letzte Aktualisierung:* 16. März 2026
**Autor:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"