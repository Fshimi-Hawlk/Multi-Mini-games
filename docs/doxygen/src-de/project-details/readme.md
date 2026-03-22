@page readme BEGRÜNDUNG

@ref index "Back to Home"

**Letzte Überprüfung gegen Projektstruktur:* 16. März 2026

# Multi Mini-Spiele

## Projektübersicht

**Second-year Informatik Bachelor-Projekt (2025-2026)*
Entwicklungszeit: Januar 2026 – April 2026

### Mitglieder:

- BAUDET Léandre: Leandre.Baudet.Etu@univ-lemans.fr
- BERGE Kimi: Kimi.Berge.Etu@univ-lemans.fr
- CAILLON Charles: Charles Caillon.Etu@univ-lemans.fr
- CHAUVEAU Maxime: Maxime.Chauveau.Etu@univ-lemans.fr

### Ziel

Erstellen Sie mehrere kleine unabhängige Minispiele in C mit **raylib**.
Später, verbinden Sie sie innerhalb eines einzigen Programms durch eine gemeinsame **lobby** — eine einfache Plattform-Stil Hubwelt.
Spieler gehen um, erreichen eine Tür oder Kollisionszone -> das gewählte Mini-Spiel beginnt direkt dort im selben Fenster (keine Spawning neue ausführbare, keine zusätzlichen Prozesse).

- Einzelspieler-Fokus für jetzt
- Multiplayer-Modus für später geplant (lokale co-op oder basis online)

## Projektablauf

Wir verwenden ein **monorepo** Setup (alles lebt in einem Repository) mit ** eine Filiale pro Mini-Spiel**.

1. Stabiler gemeinsamer Code + fusionierte Spiele live auf Zweig **main**.
2. Jedes neue Mini-Spiel wird auf seinem **own dedizierten Zweig** entwickelt (Beispiele:`block-blast`,`snake-classic`,`tetris`,`memory-game`, usw.).
3. Wenn ein Spiel in guter Form ist (Gameplay solid, Visuals anständig, Tests passieren, API bereit), wir verschmelzen diese Branche in`main`.
4. Sobald der Spielordner zusammengeführt wird dauerhaft auf`main`und wir arbeiten daran, es bis in die lobby anzuschließen.

- Neue Spielstartvorlage: Ordner ansehen`sub-project-example/`  
Es enthält bereits das empfohlene Ordnerlayout, Makefile, minimal Doxyfile,`makefile.md`, und ein README-Stub.

## Root-Level Repository Struktur

```text
.
├-- assets/                     # shared + per-game images, sounds, fonts
│   └-- fonts/                  # fonts used across games
├-- docs/                       # shared documentation + Doxygen output
│   ├-- API_Conversion.md       # how to turn a standalone game into lobby-compatible API
│   ├-- makefile.md             # root make commands explained
│   └-- ...                     # other guides
├-- firstparty/                 # our own reusable headers & single-file libs
├-- thirdparty/                 # external dependencies + NOTICE file for licenses
├-- lobby/                      # the hub/world code
├-- tetris/                     # example of already integrated game
├-- sub-project-example/        # copy-paste template for starting new games
├-- Makefile                    # root-level build system (libs + lobby exe)
├-- LICENSE.md
├-- CONTRIBUTING.md
├-- CHANGELOG.md
├-- TODO.md
└-- .gitignore
```

Nachdem mehr Spiele zusammengeführt werden, sehen Sie zusätzliche Top-Level-Ordner wie`block-blast/`,`snake-classic/`, usw.

## Sub-Project-Level Struktur (jedes Spiel / Lobby)

```text
sub-project/
├-- assets/                 # game-specific images, sounds, fonts, etc.
├-- docs/                   # per-game generated html (after make docs)
├-- include/                # all headers
│   ├-- core/               # game logic headers
│   ├-- ui/                 # rendering / drawing code headers
│   ├-- setups/             # init & cleanup functions
│   ├-- utils/              # helpers, globals, configs, common types
│   └-- subProjectAPI.h     # lobby-facing API header
├-- src/                    # implementation files (mirrors include/ + main.c)
├-- tests/                  # unit tests
├-- CHANGELOG.md            # detailed per-game changes
├-- Makefile                # build this sub-project standalone or as lib
├-- make/                   # split Makefile logic into smaller files
├-- makefile.md             # commands & modes explained for this sub-project
├-- Doxyfile.min            # minimal Doxygen config for local docs
└-- README.md               # per-game readme (template provided)
```

## Gebäude & Laufen (Root Level – fusionierte Spiele + Lobby)

Aus der Repository Wurzel:

```bash
make help               # quick list of all useful targets
make bin                # build needed libs + link the lobby executable
make rebuild-exe        # force relink lobby exe (useful after lib changes)
make run-exe            # launch the lobby
make run-tests          # run tests from all modules (lobby + merged games)
```

Volle Befehlsliste und -modi (Debug, Sanitizer, Valgrind, etc.) erläutert in @ref root_makefile.

## Building & Running (Inside a single subproject / branch)

Von innen`sub-project-example/`,`lobby/`,`tetris/`, oder jeder spielordner:

```bash
make help               # see available targets
make                    # default build (release mode)
make MODE=clang-debug   # debug + address/undefined sanitizers (needs clang)
make MODE=valgrind-debug run-main   # run with valgrind memcheck
make run-tests          # execute unit tests + save logs
make rebuild run-main   # clean build then run the standalone main
```

Hinweis: root-level Makefile for building *everything* auf einmal ist noch WIP — Track Progress in @ref todo.

## Dokumentation generieren

### Root-Level (Großprojekt)

```bash
make docs
```

- > offen`docs/doxygen/index.html`

### Pro Teilprojekt / Spiel

In jedem Spiel oder Lobby-Ordner:

```bash
make docs
```

- > Offen`./docs/html/index.html`.

**Erstellt:* 16. Dezember 2025
**Letzte Aktualisierung:* 16. März 2026
**Autor:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"