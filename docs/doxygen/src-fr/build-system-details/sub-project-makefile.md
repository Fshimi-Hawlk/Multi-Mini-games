@page sub_project_makefile Documentation sous-projet Makefile

@ref index "Retour à la maison"

** Dernière vérification par rapport à la structure du projet :** 16 mars 2026

Cette page décrit le Makefile utilisé dans chaque dossier de sous-projet (tétris/, lobby/, nouveaux jeux copiés à partir de sous-projet-exemple/, etc.).

Le Makefile gère la construction du jeu en tant qu'exécutable autonome pour les tests, produisant une bibliothèque statique pour l'intégration des lobbys, les tests d'unité en cours et supportant plusieurs modes de débogage/optimisation.

## Aperçu général

Principales responsabilités :
- Construire un exécutable autonome à partir de `src/main.c` (pour un test rapide)
- Construire la bibliothèque statique `lib<gameName>.a` (utilisée par root Makefile pour lier dans le lobby)
- Construisez et exécutez des essais unitaires à partir de "tests/"
- Prise en charge de plusieurs modes de construction (release / debug / sanitizers / valgrind)
- Suivi automatique de la dépendance à l'en-tête (-MMD -MP)
- Silencieux par défaut, verbe avec VERBOSE=1
- Drapeaux personnalisés via EXTRA CFLAGS / EXTRA LDFLAGS
- Log test et valgrind sortie dans les dossiers horodatés sous `logs/`

Chemins de sortie typiques (relatifs au dossier du sous-projet):
- Objets: `construction/obj`
- binaire autonome: `build/bin/main`
- Lib statique: `build/lib/lib<gameName>.a`
- Binaires d'essai: `build/bin/tests/<test-name>`

## Modes de construction pris en charge

Mode d'utilisation type
|-----------------|----------|------------------------------------------|--------------------|----------------------------------------|
Release de l'image gcc-O2-O2-O2
Debug (en anglais seulement)
Debug strict -Worror -Wall -Wextra -pedantic -g -O0--Catch avertissements avant fusion
Clang-debug.Clang.Clang.Clang.Clang.Clang.Clang.Clang.Clang.Clang.Clang.Clang
valgrind-debug ,gcc ,Werror -Wall -Wextra -pedantic -g -O0 , valgrind ,installé

Le mode par défaut est `release`.

## Objectifs les plus utiles

- `all` / cible par défaut : build standalone exécutable
- `static-lib` : construire une bibliothèque statique pour l`intégration des lobbys
- "tests" : construire tous les binaires de test
- `run-main` : build + run exécutable autonome
- `run-tests` : construire + exécuter tous les tests (avec sortie en direct + logs)
- `reconstruction` : propre + tout
- `clean` : supprimer build/ dossier
- `run-gdb` : exécuter un binaire autonome sous gdb

## Exemples rapides

Construction autonome de base + exécution:

```bash
make
make run-main
```

Débogue / mode strict:

```bash
make MODE=debug
make MODE=strict-debug rebuild run-main
```

Avec désinfectants ou Valgrind:

```bash
make MODE=clang-debug run-main
make MODE=valgrind-debug run-tests
```

Construisez seulement la lib statique (pour lobby):

```bash
make static-lib
```

Verbeux + avertissements supplémentaires:

```bash
make VERBOSE=1 EXTRA_CFLAGS="-Wshadow -Wconversion" MODE=strict-debug
```

## Notes d'exploitation et de sortie

- `run-tests` crée `logs/tests-<timestamp>/` avec un fichier .log par test (stdout + stderr)
- En mode `valgrind-debug`: dossiers supplémentaires `logs/valgrind-<timestamp>/` avec rapports Valgrind
- Logs seulement enregistrer la sortie non vide
- Fiabilité de sortie du crash: utilise `stdbuf --output=L --error=L` (ligne tamponnant) quand disponible si les dernières lignes printf() apparaissent même sur le crash SIGABRT ou ASan
- Si `stdbuf` est manquant -> avertissement imprimé une fois, la sortie pourrait tronquer lors d'un crash -> ajouter explicitement `fflush(stdout); fflush(stderr);` dans le code de test si nécessaire

## Exigences et transférabilité

- "clang-debug" a besoin de clang
- besoins en valgrind-debug
- `stdbuf` (à partir de coreutils) est facultatif mais recommandé pour une sortie de test propre sur Linux

## Pages connexes

@ref sub project internal makefile structure "Makefile Internal Structure": Comment le Makefile est **internement structuré**

**Créé :** 16 mars 2026
**Auteur:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Retour à la maison"