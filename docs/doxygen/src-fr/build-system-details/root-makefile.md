@page root_makefile Documentation du fichier racine

@ref index "Retour à la maison"

** Dernière vérification par rapport à la structure du projet :** 16 mars 2026

Cette page décrit le **root Makefile** dans le Multi Mini-Games monorepo.

La racine Makefile est responsable de:
- Construction de bibliothèques statiques pour tous les jeux intégrés (libtetris.a, etc.)
- Copier l'en-tête public d'API de chaque jeu (`tetrisAPI.h`, etc.) dans `firstparty/API/`
- Lier tout ensemble en un seul lobby exécutable
- Faire tout cela progressivement lorsque c'est possible (reconstruction seulement ce qui a changé)

Il prend en charge les mêmes modes de construction que le sous-projet Makefiles de sorte que tout le monorepo se sent cohérent.

## Modes de construction

Vous choisissez le mode en ajoutant `MODE=xxx` à n'importe quelle commande make. Par défaut est `release`.

Mode Compilateur Drapeaux principaux Notes supplémentaires / exigences
|-------------------|----------|----------------------------------------|-------------------------------|
Sortie d'un gcc-O2-O2 Optimisé, aucun symbole de débogage
Debug de base + avertissements
Debug strict -Worror -Wall -Wextra -pedantic -g-Les avertissements deviennent des erreurs
Clang-debug.Clang.Clang.Clang.Clang
valgrind-debug , gcc ,g -O0 (enveloppeur de valgrind run)

## Principaux objectifs

- `make` / `all` / `bin`
Constitue/mise à jour des bibliothèques + lie l'exécutable du lobby (incrémental)

- "libs"
Constitue ou met à jour uniquement les bibliothèques statiques + copie les en-têtes de l'API

- "reconstruction"
Complètement propre + tout reconstruire à partir de zéro

- "propre"
Supprime le dossier racine `build/` (bibliothèques + exe)

- "nettoyez tout"
Nettoyer racine + appels propres dans chaque sous-projet

- "reconstruction-exe"
Relien des forces du lobby exécutable seulement (utile après les changements de lib)

- "exe"
Exécute le lobby à partir de `build/bin/main` (ou de tout ce que MAIN NAME est réglé)

- "tests" / "tests de course"
Construit et exécute tous les tests unitaires (lobby + chaque jeu)

- "aide"
Imprime cette liste de cibles + modes

## Commandes courantes (ce que nous utilisons le plus)

Construction rapide normale + exécuter:

```bash
make bin
make run-exe
```

Forcez tout en débogage avec des désinfectants (bon pour attraper des insectes):

```bash
make MODE=clang-debug rebuild run-exe
```

Reconstruire les bibliothèques seulement (après avoir ajouté/changer un jeu):

```bash
make rebuild-libs
```

Effectuer tous les essais avec des vérifications supplémentaires:

```bash
make MODE=clang-debug run-tests
```

## Gestion de la trajectoire des biens

Pour faire fonctionner les biens à la fois autonome et dans le hall :

- Lors de la construction dans un dossier de jeu -> utilise `actifs/...`
- Lors de la construction à partir de root -> utilise `<gamename>/assets/...` (par exemple `tetris/assets/`)

Ceci est contrôlé en passant `-DASSET PATH... » via `EXTRA CFLAGS` dans le Makefile.

## Copie des en-têtes d'API

Au cours des cibles `libs`, `bin`, `reconstruction-libs`:
- Chaque jeu `<gamename>API.h` est copié de `<gamename>/include/` vers `firstparty/API/`
- Copie seulement si l'en-tête existe réellement

Cela permet au lobby `#incluez "API/tetrisAPI.h"` sans gâchis de chemin.

## Déboguage & Logs

- Tests créer des dossiers comme `logs/tests-2026-03-16 14-30/`
- Le mode Valgrind crée `logs/valgrind-<timestamp>/`
- Ajouter `VERBOSE=1` à n'importe quelle commande make pour voir toutes les lignes gcc/clang

## Production de documentation

```bash
make docs
```

-> `docs ouverts/doxygen/index.html`


**Créé:** 2 mars 2025
**Dernière mise à jour :** 16 mars 2026
**Auteur:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Retour à la maison"