@page readme README

@ref index "Back to Home"

** Última comprobación contra la estructura del proyecto:** 16 de marzo de 2026

# Multi Mini-Games

## Panorama general del proyecto

**Proyecto de Bachelor en Ciencias de la Computación (2025-2026)**
Período de desarrollo: enero 2026 – abril 2026

### Miembros:

- BAUDET Léandre: Leandre.Baudet.Etu@univ-lemans.fr
BERGE Kimi: Kimi.Berge.Etu@univ-lemans.fr
- CAILLON Charles: Charles Caillon.Etu@univ-lemans.fr
- CHAUVEAU Maxime: Maxime.Chauveau.Etu@univ-lemans.fr

### Objetivo

Cree varios pequeños minijuegos independientes en C usando **raylib**.
Más adelante, conéctelos dentro de un solo programa a través de un mundo común **lobby** — un simple mundo de hub de estilo de plataforma.
Los jugadores caminan alrededor, llegan a una puerta o zona de colisión - título el mini-juego elegido comienza allí en la misma ventana (sin desove de nuevos ejecutables, sin procesos extras).

- Enfoque de un jugador por ahora
- Modo multijugador planeado para más tarde (coop local o básico en línea)

## Project Workflow

Estamos usando una configuración **monorepo** (todo vive en un repositorio) con **una rama por mini-juego**.

1. Stable código compartido + juegos fusionados viven en rama **main**.
2. Cada nuevo minijuego se desarrolla en su rama dedicada ** (ejemplos:`block-blast`,`snake-classic`,`tetris`,`memory-game`, etc.).
3. Cuando un juego está en buena forma (juego sólido, visuales decentes, pruebas de paso, API listo), fusionamos esa rama en`main`.
4. Una vez fusionado, la carpeta del juego se vuelve permanente`main`y trabajamos en conectarlo al vestíbulo.

- Nueva plantilla de arranque del juego: mira la carpeta`sub-project-example/`  
Ya contiene el diseño de carpeta recomendado, Makefile, mínimo Doxyfile,`makefile.md`Y un problema.

## Estructura del repositorio giratorio

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

Después de que se fusionen más juegos verás carpetas adicionales de alto nivel como`block-blast/`,`snake-classic/`, etc.

## Estructura de subproyecto-Nivel (cada juego / lobby)

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

## Edificio " Running (nivel de arranque – juegos fusionados + lobby)

Desde la raíz del repositorio:

```bash
make help               # quick list of all useful targets
make bin                # build needed libs + link the lobby executable
make rebuild-exe        # force relink lobby exe (useful after lib changes)
make run-exe            # launch the lobby
make run-tests          # run tests from all modules (lobby + merged games)
```

Lista de comandos completos y modos (debug, sanitizers, valgrind, etc.) explicados en TOTOK0/ Conf.

## Building " Running (Inside a single sub-project / branch)

Desde dentro`sub-project-example/`,`lobby/`,`tetris/`, o cualquier carpeta del juego:

```bash
make help               # see available targets
make                    # default build (release mode)
make MODE=clang-debug   # debug + address/undefined sanitizers (needs clang)
make MODE=valgrind-debug run-main   # run with valgrind memcheck
make run-tests          # execute unit tests + save logs
make rebuild run-main   # clean build then run the standalone main
```

Nota: Perfil de nivel raíz para la construcción *todo* a la vez sigue siendo WIP – seguimiento de progreso en <TOK0/ Conf.

## Generación de documentación

### Root-level (whole project)

```bash
make docs
```

- está abierto`docs/doxygen/index.html`

### Por subproyecto / juego

Dentro de cualquier carpeta de juego o lobby:

```bash
make docs
```

- Abierto`./docs/html/index.html`.

**Creado:** 16 de diciembre de 2025
** Última actualización:** 16 de marzo de 2026
**Author:**

@ref index "Back to Home"