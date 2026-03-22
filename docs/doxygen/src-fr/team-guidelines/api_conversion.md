@page api_conversion Conversion et intégration des API

@ref index "Back to Home"

** Dernière vérification par rapport à la structure du projet :** 16 mars 2026

**Convertir un mini-jeu autonome en API compatible avec le lobby**

Cette page explique les étapes pour prendre un mini-jeu indépendant (par exemple`tetris/`dossier) et le transformer en quelque chose que le lobby peut charger et courir directement dans la même fenêtre. Pas de nouveaux processus, pas de fenêtres supplémentaires — il suffit de changer de scène propre.

L'approche est basée sur la façon dont nous avons intégré Tetris, et suit les modèles dans`generalAPI.h`, les fichiers API exemples, et`lobby/src/main.c`.

** Principes fondamentaux que nous suivons**
- Utilisez des structures opaques pour que le lobby n'ait jamais accès aux internes du jeu
- Fournit exactement trois fonctions principales: init, loop, free
- Erreurs de retour en utilisant le partage`Error_Et`enum
- Traitez chaque mini-jeu comme un "scène" temporaire le hall peut commencer et s'arrêter

## Préalables

Avant de commencer la conversion assurez-vous:
- Votre dossier de jeu correspond au`sub-project-example/`présentation (`src/`,`include/`,`tests/`,`Makefile`)
- Le mode autonome fonctionne :`make rebuild run-main`exécute le jeu sans erreurs
- Raylib est lié correctement par le Makefile

Si l'un d'entre eux manque, copiez le dossier du modèle et obtenez un fonctionnement autonome en premier.

## Étape 1 – Créer l'en-tête public de l'API

Dossier & #160;:`include/<gameName>API.h`(exemple:`tetrisAPI.h`)

Cet en-tête est la seule chose que le lobby verra. Garder le minimum: type opaque, déclarations de fonction, structure optionnelle de configuration.

Contenu typique:

```c
#ifndef TETRIS_API_H
#define TETRIS_API_H

#include "baseTypes.h"
#include "APIs/generalAPI.h"

typedef struct TetrisGame_St TetrisGame_St;

typedef struct {
    int fps;            // target FPS, 0 = use default
    // add other options later if needed
} TetrisConfigs_St;

Error_Et tetris_initGame__full(TetrisGame_St** game, TetrisConfigs_St configs);

#define tetris_initGame(game, ...) \
    tetris_initGame__full(game, (TetrisConfigs_St){ __VA_ARGS__ })

void tetris_gameLoop(TetrisGame_St* const game);

void tetris_freeGame(TetrisGame_St** game);

#endif
```

**Note importante**
Dans l'implémentation (fichier.c), le premier membre de votre struct **doit** être`Game_St base;`afin que le lobby puisse jeter en toute sécurité à la`Game_St*`type.

Voir les vrais fichiers dans`firstparty/APIs/`ou`tetris/include/tetrisAPI.h`pour référence.

## Étape 2 – Implémenter les fonctions de l'API

Dossier & #160;:`src/<gameName>API.c`

Ici vous définissez la structure complète et écrivez les trois fonctions.

Squelette basique:

```c
struct TetrisGame_St {
    Game_St base;       // MUST be first member
    // your game-specific fields
    // Board board;
    // int score;
    // ...
};

Error_Et tetris_initGame__full(TetrisGame_St** game_out, TetrisConfigs_St configs) {
    TetrisGame_St* game = calloc(1, sizeof(TetrisGame_St));
    if (!game) return ERROR_ALLOC;

    game->base.running = true;
    // initialize your data, load assets, etc.
    // if (configs.fps > 0) SetTargetFPS(configs.fps);

    *game_out = game;
    return OK;
}

void tetris_gameLoop(TetrisGame_St* const game) {
    if (!game->base.running) return;

    // handle input
    // update logic
    BeginDrawing();
        // render everything
    EndDrawing();

    // set running = false when game should end
}

void tetris_freeGame(TetrisGame_St** game) {
    if (!game || !*game) return;
    // free textures, unload sounds, release memory
    free(*game);
    *game = NULL;
}
```

Ne jamais appeler`InitWindow`,`CloseWindow`ou de gérer la fenêtre à l'intérieur de ces fonctions — le lobby l'a déjà fait.

## Étape 3 – Refactorer votre logique de jeu

Déplacez tout votre code de jeu dans les trois fonctions de l'API:
- Initialisation`init`
- Le travail par cadre va dans`loop`
- Le nettoyage entre`free`

Supprimez les appels de création/fermeture de fenêtres de votre code original.

## Étape 4 – Intégrer dans le hall

Dans`lobby/src/main.c`:
Ajouter`#include "APIs/<gameName>API.h"`
- Ajoutez votre jeu au`GameScene_Et`enum
- Lorsque le joueur se heurte à la zone de déclenchement :
- Prends le pointeur :`Game_St** miniRef = &game->subGameManager.miniGames[GAME_SCENE_YOURGAME];`
- Une fois :`<GameName>Game_St** ref = (<GameName>Game_St**) miniRef;`
- Si la première fois : appeler init et vérifier les erreurs
- Appelez la boucle chaque cadre
- Quand`!(*miniRef)->running`: appeler gratuitement et retourner sur la scène du hall

Nous utilisons`miniRef`pour éviter la coulée chaque fois que nous vérifions`running`.

Voyez comment c'est fait pour Tetris en`lobby/src/main.c`.

## Étape 5 – Construire et vérifier

À partir de la racine du dépôt :

```bash
# Normal full rebuild + run
make MODE=strict-debug rebuild run-exe

# With sanitizers (good for catching bugs)
make MODE=clang-debug rebuild run-exe

# Or valgrind if you don't have clang
make MODE=valgrind-debug rebuild run-exe
```

De l'intérieur de votre dossier de jeu (pour les vérifications autonomes):

```bash
make MODE=strict-debug rebuild run-main
```

Regardez les journaux de console si quelque chose échoue.

## Les problèmes communs auxquels nous sommes confrontés

- Oui`Game_St base`pas d'abord dans la structure -> casting échoue silencieusement
- Appels de jeux`InitWindow`/`CloseWindow`-> crash à double init
- Textures/sons non libérés -> fuites de mémoire
- Pas de retour en boucle`!running`-> jeu continue de courir après avoir arrêté
- Re-déclencheur instantané lors de la sortie -> ajouter le cooldown sur le bouton
- Oublier de copier l'en-tête de l'API -> lobby include failes

Si quelque chose casse, cochez la page @ref sub_project_makefile "Sub Project Makefile", comparez avec`tetrisAPI.c`, ou demander dans le chat de groupe.

**Créé :** 10 février 2026
**Dernière mise à jour :** 16 mars 2026
**Auteur:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"