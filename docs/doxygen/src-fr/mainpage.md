@mainpage Page d'accueil

# Mini-jeux multiples

** Dernière vérification par rapport à la structure du projet :** 16 mars 2026

Ce monorepo abrite une collection de petits mini-jeux indépendants construits en C avec raylib.
L'idée principale est un seul exécutable : un centre **lobby** (simple plateforme 2D hub) où vous marchez et déclenchez des jeux en marchant dans les zones de collision. Les jeux se déroulent dans le hall, pas de fenêtres ou de processus supplémentaires.

En mars 2026, le projet a :
- Lobby de travail avec mouvement, physique, et l'interface utilisateur de base (sélection de peau).
- Un jeu intégré : Tetris.
- Logage partagé, types, utilitaires et la couche API pour ajouter plus de jeux.
- Construisez le système et la documentation.

@ref détails du projet "Détails du projet"
@ref build system "Build system"
@ref team guidelines "Lignes directrices de l'équipe"

## Démarrer rapidement

À partir de la racine du dépôt :

```bash
make libs      # build shared libs
make bin       # link lobby executable
make run-exe   # run the lobby
make tests     # run all tests
```

Commandes et modes complets (debug, sanizers, valgrind, sous-projet buildings) -> @ref root makefile "Documentation Root Makefile"

Dans n'importe quel dossier de jeu (tétris/, nouveau jeu, etc.):

```bash
make rebuild run-main
make run-tests
```

Les détails du sous-projet restent dans chaque jeu.

## Structure du répertoire

```text
.
├── assets/
├── docs/                       # this site source
├── firstparty/                 # shared utils, logger, types, generalAPI
├── thirdparty/
├── lobby/
├── tetris/                     # example integrated game
├── sub-project-example/        # copy for new games
├── Makefile                    # root
├── LICENSE.md
├── CONTRIBUTING.md
├── CHANGELOG.md
└── TODO.md
```

Après plus de jeux sont fusionnés, ils deviennent des dossiers de haut niveau comme tetris/.

## Modules actuels

### Module de lobby
Point d'entrée principal et gestionnaire de scène.
La détection de collision charge des jeux via API.
Voir : @ref lobby "Lobby Module Overview" (ajouter la page plus tard)

### Module Tetris
Tetris classique avec score, niveaux, aperçu de la pièce suivante.
Intégré par une structure opaque + init/loop/free.
Voir : @ref tetris "Tetris Module Overview" (ajouter la page plus tard)

Les choses partagées (logger, baseTypes, configs, utils, generalAPI) vivent en première partie/.

## Flux de travail et rappels de développement

- Nouveau jeu: copie sous-projet-exemple/, branche, fusion lorsque l'API est prêt.
- Étapes de conversion de l'API -> @ref api conversion
- Commit style -> @ref contribuant
- Style de code -> @ref code style and conventions
- Tests + désinfectants avant toute fusion.
- Mettre à jour @ref changelog et @ref todo après les gros changements.

Guide complet: @ref contribuant "CONTRIBUTING.md"

## Feuille de route et tâches ouvertes

Court terme : plus de jeux, d'économies, de classements.
Les idées à long terme dans TODO.md.

Détails: @ref todo "TODO.md"

## Licence

zlib/libpng. Texte complet: @ref licence "LICENSE.md"

Grâce à raylib, nob.h, etc. (crédits en en-têtes).

**Créé :** le 1er mars 2026
**Dernière mise à jour :** 16 mars 2026
**Auteur:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)