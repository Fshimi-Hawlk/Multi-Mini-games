@page api_conversion Conversion et intégration des API

@ref index "Retour à la maison"

** Dernière vérification par rapport à la structure du projet :** 16 mars 2026

**Convertir un mini-jeu autonome en API compatible avec le lobby**

Cette page explique les étapes pour prendre un mini-jeu indépendant (par exemple le dossier `tetris/`) et le transformer en quelque chose que le lobby peut charger et exécuter directement dans la même fenêtre. Pas de nouveaux processus, pas de fenêtres supplémentaires — il suffit de changer de scène propre.

L'approche est basée sur la façon dont nous avons intégré Tetris, et suit les modèles dans `generalAPI.h`, l'exemple des fichiers API, et `lobby/src/main.c`.

** Principes fondamentaux que nous suivons**
- Utilisez des structures opaques pour que le lobby n'ait jamais accès aux internes du jeu
- Fournit exactement trois fonctions principales: init, loop, free
- Erreurs de retour en utilisant l'enum `Error Et` partagé
- Traitez chaque mini-jeu comme un "scène" temporaire le hall peut commencer et s'arrêter

## Préalables

Avant de commencer la conversion assurez-vous:
- Votre dossier de jeu correspond à la mise en page ``src/`, `include/`, `tests/`, `Makefile`)
- Le mode autonome fonctionne: `make rebuild run-main` exécute le jeu sans erreurs
- Raylib est lié correctement par le Makefile

Si l'un d'entre eux manque, copiez le dossier du modèle et obtenez un fonctionnement autonome en premier.

## Étape 1 – Créer l'en-tête public de l'API

Fichier: `include/<gameName>API.h` (exemple: `tetrisAPI.h`)

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
Dans l'implémentation (fichier.c), le premier membre de votre structure **doit** être `Game St base;` de sorte que le lobby puisse lancer en toute sécurité dans le type `Game St*` partagé.

Voir les fichiers réels dans `firstparty/API/` ou `tetris/include/tetrisAPI.h` pour référence.

## Étape 2 – Implémenter les fonctions de l'API

Fichier : `src/<gameName>API.c`

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

Ne jamais appeler `InitWindow`, `FermerWindow` ou gérer la fenêtre à l'intérieur de ces fonctions - le lobby a déjà fait cela.

## Étape 3 – Refactorer votre logique de jeu

Déplacez tout votre code de jeu dans les trois fonctions de l'API:
- Initialisation en 'init'
- Le travail par cadre va en boucle
- Le nettoyage est gratuit

Supprimez les appels de création/fermeture de fenêtres de votre code original.

## Étape 4 – Intégrer dans le hall

Dans «lobby/src/main.c»:
- Ajouter `#inclure "API/<gameName>API.h"`
- Ajouter votre jeu à l'enum `GameScene Et`
- Lorsque le joueur se heurte à la zone de déclenchement :
- Obtenez le pointeur : `Game St** miniRef = &game->subGameManager.miniGames[GAME SCENE YOURGAME];`
- Cast once: `<GameName>Game St** ref = (<GameName>Game St**) miniRef;`
- Si la première fois : appeler init et vérifier les erreurs
- Appelez la boucle chaque cadre
- Quand `!(*miniRef)->running`: appelez gratuitement et retournez sur la scène du lobby

Nous utilisons `miniRef` pour éviter le casting chaque fois que nous vérifions `cours'.

Voyez comment il est fait pour Tetris dans `lobby/src/main.c`.

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

- `Game St base` pas d'abord dans la structure -> casting échoue silencieusement
- Appels de jeu `InitWindow` / `FermerWindow` -> crash double init
- Textures/sons non libérés -> fuites de mémoire
- Aucun retour précoce en boucle quand `!running` -> jeu continue à courir après avoir arrêté
- Re-déclencheur instantané lors de la sortie -> ajouter le cooldown sur le bouton
- Oublier de copier l'en-tête de l'API -> lobby include failes

Si quelque chose casse, cochez @ref sub project makefile "Sub Project Makefile" page, comparez avec `tetrisAPI.c`, ou demandez dans le chat de groupe.

**Créé :** 10 février 2026
**Dernière mise à jour :** 16 mars 2026
**Auteur:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Retour à la maison"