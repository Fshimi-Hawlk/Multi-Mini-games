@page root_makefile Documentation du fichier racine

@ref index "Back to Home"

** Dernière vérification par rapport à la structure du projet :** 16 mars 2026

Cette page décrit le **root Makefile** dans le Multi Mini-Games monorepo.

La racine Makefile est responsable de:
- Construction de bibliothèques statiques pour tous les jeux intégrés (`libtetris.a`, etc.)
- Copier l'en-tête public d'API de chaque jeu (`tetrisAPI.h`, etc.)`firstparty/APIs/`
- Lier tout ensemble en un seul lobby exécutable
- Faire tout cela progressivement lorsque c'est possible (reconstruction seulement ce qui a changé)

Il prend en charge les mêmes modes de construction que le sous-projet Makefiles de sorte que tout le monorepo se sent cohérent.

## Modes de construction

Vous choisissez le mode en ajoutant`MODE=xxx`à n'importe quel ordre. Par défaut`release`.

Mode Compilateur Drapeaux principaux Notes supplémentaires / exigences
|-------------------|----------|----------------------------------------|-------------------------------|
Sortie d'un gcc-O2-O2 Optimisé, aucun symbole de débogage
Debug de base + avertissements
Debug strict -Worror -Wall -Wextra -pedantic -g-Les avertissements deviennent des erreurs
Clang-debug.Clang.Clang.Clang.Clang
valgrind-debug , gcc ,g -O0 (enveloppeur de valgrind run)

## Principaux objectifs

- Oui`make`/`all`/`bin`  
Constitue/mise à jour des bibliothèques + lie l'exécutable du lobby (incrémental)

- Oui`libs`  
Constitue ou met à jour uniquement les bibliothèques statiques + copie les en-têtes de l'API

- Oui`rebuild`  
Complètement propre + tout reconstruire à partir de zéro

- Oui`clean`  
Supprime la racine`build/`dossier (bibliothèques + exe)

- Oui`clean-all`  
Nettoyer racine + appels propres dans chaque sous-projet

- Oui`rebuild-exe`  
Relien des forces du lobby exécutable seulement (utile après les changements de lib)

- Oui`run-exe`  
Il court le hall de`build/bin/main`(ou quoi que soit MAIN NAME soit réglé)

- Oui`tests`/`run-tests`  
Construit et exécute tous les tests unitaires (lobby + chaque jeu)

- Oui`help`  
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

- Lors de la construction dans un dossier de jeu -> utilise`assets/…`
- Quand construire à partir de racine -> utilise`<gamename>/assets/…`(par exemple).`tetris/assets/`)

Ceci est contrôlé par le passage`-DASSET_PATH="..."`par`EXTRA_CFLAGS`dans le Makefile.

## Copie des en-têtes d'API

Pendant`libs`,`bin`,`rebuild-libs`cibles:
- Chaque partie`<gamename>API.h`est copié de`<gamename>/include/`à`firstparty/APIs/`
- Copie seulement si l'en-tête existe réellement

Cela laisse le hall`#include "APIs/tetrisAPI.h"`sans embrouille.

## Déboguage & Logs

- Tests créer des dossiers comme`logs/tests-2026-03-16_14-30/`
- Le mode Valgrind crée`logs/valgrind-<timestamp>/`
Ajouter`VERBOSE=1`à n'importe quelle commande make pour voir toutes les lignes gcc/clang

## Production de documentation

```bash
make docs
```

->`open docs/doxygen/index.html`


**Créé:** 2 mars 2025
**Dernière mise à jour :** 16 mars 2026
**Auteur:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"