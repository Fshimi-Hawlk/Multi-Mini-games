@page root_makefile Root Makefile Dokumentation

@ref index "Back to Home"

**Letzte Überprüfung gegen Projektstruktur:* 16. März 2026

Diese Seite beschreibt die **root Makefile** im Multi Mini-Games monorepo.

Die Wurzel Makefile ist verantwortlich für:
- Gebäude statische Bibliotheken für alle integrierten Spiele (`libtetris.a`, usw.)
- Kopieren der öffentlichen API-Header jedes Spiels (`tetrisAPI.h`, usw.)`firstparty/APIs/`
- Alles zusammen in einer einzigen Lobby ausführbar
- All dies schrittweise tun, wenn möglich (nur wieder aufbauen, was geändert wird)

Es unterstützt die gleichen Build-Modi wie das Sub-Projekt Makefiles, so dass das gesamte Monorepo konsequent ist.

## Moden erstellen

Sie wählen den Modus durch Hinzufügen`MODE=xxx`auf jeden Befehl. Standard ist`release`.

| Modus | Compiler | Hauptfahnen | Extra Notizen / Anforderungen |
|-------------------|----------|----------------------------------------|-------------------------------|
| release | gcc | -O2 | Optimiert, keine Debug-Symbole |
| debug | gcc | -Wall -Wextra -g -O0 | Basic debug + warnungen |
| streng-debug | gcc | -Werror -Wall -Wextra -pedantic -g | Warnungen werden Fehler |
| clang-debug | clang | + -fsanitize=address,undefiniert | Braucht clang installiert |
| valgrind-debug | gcc | -g -O0 (valgrind run wrapper) | Benötigt valgrind installiert |

## Hauptziele

- Ja`make`/`all`/`bin`  
Erstellt/ aktualisiert Bibliotheken + verlinkt die Lobby ausführbar (incremental)

- Ja`libs`  
Erstellt oder aktualisiert nur die statischen Bibliotheken + Kopien API-Header

- Ja`rebuild`  
Voll sauber + alles neu aufbauen

- Ja`clean`  
Entfernt die Wurzel`build/`ordner (libraries + exe)

- Ja`clean-all`  
Reinigt Wurzel + Anrufe sauber in jedem Teilprojekt

- Ja`rebuild-exe`  
Kräfte verlinken nur die ausführbare Lobby (verfügbar nach lib-Änderungen)

- Ja`run-exe`  
Führt die Lobby aus`build/bin/main`(oder was auch immer MAIN NAME eingestellt ist)

- Ja`tests`/`run-tests`  
Erstellt und führt alle Einzeltests (lobby + jedes Spiel)

- Ja`help`  
Druckt diese Liste der Ziele + Modi

## Gemeinsame Befehle (was wir am meisten nutzen)

Normaler Schnellaufbau + Lauf:

```bash
make bin
make run-exe
```

Erzwingen Sie alles im Debug mit Sanitizern (gut für das Einfangen von Bugs):

```bash
make MODE=clang-debug rebuild run-exe
```

Nur Bibliotheken neu aufbauen (nach dem Hinzufügen/Verändern eines Spiels):

```bash
make rebuild-libs
```

Alle Tests mit zusätzlichen Kontrollen ausführen:

```bash
make MODE=clang-debug run-tests
```

## Umgang mit Ressourcen

Um Vermögenswerte zu machen arbeiten sowohl standalone als auch in der Lobby:

- Beim Bau in einem Spielordner -> verwendet`assets/…`
- Beim Aufbau von Wurzel -> Verwendungen`<gamename>/assets/…`(z.`tetris/assets/`)

Dies wird durch das Passieren gesteuert`-DASSET_PATH="..."`über`EXTRA_CFLAGS`in der Makefile.

## API Header Kopieren

Während`libs`,`bin`,`rebuild-libs`ziele:
- Jedes Spiel ist`<gamename>API.h`wird kopiert`<gamename>/include/`bis`firstparty/APIs/`
- Nur Kopien, wenn der Header tatsächlich existiert

Das lässt die Lobby`#include "APIs/tetrisAPI.h"`ohne pfad mess.

## Debugging & Logs

- Tests erstellen Ordner wie`logs/tests-2026-03-16_14-30/`
- Valgrind-Modus erstellt`logs/valgrind-<timestamp>/`
- Hinzufügen`VERBOSE=1`zu jedem befehl, um volle gcc/clang linien zu sehen

## Dokumentation generieren

```bash
make docs
```

- >`open docs/doxygen/index.html`


**Erstellt:* März 02, 2025
**Letzte Aktualisierung:* 16. März 2026
**Autor:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"