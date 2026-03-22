@page readme LIRE

@ref index "Back to Home"

** Dernière vérification par rapport à la structure du projet :** 16 mars 2026

# Mini-jeux multiples

## Aperçu du projet

**Projet de baccalauréat en informatique de deuxième année (2025-2026)**
Période de développement : janvier 2026 – avril 2026

### Membres :

- BAUDET Léandre: Leandre.Baudet.Etu@univ-lemans.fr
- BERGE Kimi: Kimi.Berge.Etu@univ-lemans.fr
Charles: Charles Caillon.Etu@univ-lemans.fr
- CHAUVEAU Maxime: Maxime.Chauveau.Etu@univ-lemans.fr

### Objectif

Créez plusieurs petits mini-jeux indépendants en C en utilisant **raylib**.
Plus tard, connectez-les à l'intérieur d'un seul programme à travers un commun **lobby** — un simple monde de plateforme de style hub.
Les joueurs se promènent, atteignent une porte ou une zone de collision -> le mini-jeu choisi commence juste là dans la même fenêtre (pas de nouveaux exécutables de frai, pas de processus supplémentaires).

- Un seul joueur pour l'instant
- Mode multijoueur prévu pour plus tard (coop local ou base en ligne)

## Flux de travail du projet

Nous utilisons une configuration **monorepo** (tout vit dans un dépôt) avec **une branche par mini-jeu**.

1. Code partagé stable + jeux fusionnés en direct sur branche **main**.
2. Chaque nouveau mini-jeu est développé sur sa propre branche dédiée** (exemples:`block-blast`,`snake-classic`,`tetris`,`memory-game`, etc.).
3. Lorsqu'un jeu est en bonne forme (jeu solide, visuels décents, tests de passage, API prêt), nous fusionnons cette branche en`main`.
4. Une fois fusionné, le dossier de jeu devient permanent sur`main`et nous travaillons à l'accrocher au hall.

- Nouveau modèle de démarrage de jeu : regardez le dossier`sub-project-example/`  
Il contient déjà la mise en page de dossier recommandée, Makefile, Doxyfile minimal,`makefile.md`Et un talon README.

## Structure du dépôt au niveau de la racine

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

Après plus de jeux obtenir fusionné vous verrez des dossiers de haut niveau supplémentaires comme`block-blast/`,`snake-classic/`, etc.

## Structure du niveau du sous-projet (chaque jeu / lobby)

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

## Construction et fonctionnement (Root Level – jeux fusionnés + lobby)

À partir de la racine du dépôt :

```bash
make help               # quick list of all useful targets
make bin                # build needed libs + link the lobby executable
make rebuild-exe        # force relink lobby exe (useful after lib changes)
make run-exe            # launch the lobby
make run-tests          # run tests from all modules (lobby + merged games)
```

Liste complète des commandes et modes (débogue, sanitizers, valgrind, etc.) expliqués dans @ref root_makefile.

## Construction et fonctionnement (dans un seul sous-projet / branche)

De l'intérieur`sub-project-example/`,`lobby/`,`tetris/`, ou tout dossier de jeu:

```bash
make help               # see available targets
make                    # default build (release mode)
make MODE=clang-debug   # debug + address/undefined sanitizers (needs clang)
make MODE=valgrind-debug run-main   # run with valgrind memcheck
make run-tests          # execute unit tests + save logs
make rebuild run-main   # clean build then run the standalone main
```

Note : le Makefile de niveau racine pour construire *tout* à la fois est toujours WIP — suivre les progrès dans @ref todo.

## Production de documentation

### Niveau racine (projet global)

```bash
make docs
```

-> ouvert`docs/doxygen/index.html`

### Par sous-projet / jeu

À l'intérieur de tout jeu ou dossier de lobby:

```bash
make docs
```

-> Ouvrir`./docs/html/index.html`.

**Créé :** 16 décembre 2025
**Dernière mise à jour :** 16 mars 2026
**Auteur:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"