@page sub_project_makefile Documentation sous-projet Makefile

@ref index "Back to Home"

** Dernière vérification par rapport à la structure du projet :** 16 mars 2026

Cette page décrit le Makefile utilisé dans chaque dossier de sous-projet (tétris/, lobby/, nouveaux jeux copiés à partir de sous-projet-exemple/, etc.).

Le Makefile gère la construction du jeu en tant qu'exécutable autonome pour les tests, produisant une bibliothèque statique pour l'intégration des lobbys, les tests d'unité en cours et supportant plusieurs modes de débogage/optimisation.

## Aperçu général

Principales responsabilités :
- Construire exécutable autonome à partir`src/main.c`(pour les essais rapides)
- Construire une bibliothèque statique`lib<gameName>.a`(utilisé par root Makefile pour se connecter au lobby)
- Construire et exécuter des essais unitaires à partir`tests/`
- Prise en charge de plusieurs modes de construction (release / debug / sanitizers / valgrind)
- Suivi automatique de la dépendance à l'en-tête (-MMD -MP)
- Silencieux par défaut, verbe avec VERBOSE=1
- Drapeaux personnalisés via EXTRA CFLAGS / EXTRA LDFLAGS
- Log test et valgrind sortie dans les dossiers horodatés sous`logs/`

Chemins de sortie typiques (relatifs au dossier du sous-projet):
- Objets :`build/obj/`
- binaire autonome :`build/bin/main`
- Lib statique :`build/lib/lib<gameName>.a`
- Binaires d'essai :`build/bin/tests/<test-name>`

## Modes de construction pris en charge

Mode d'utilisation type
|-----------------|----------|------------------------------------------|--------------------|----------------------------------------|
Release de l'image gcc-O2-O2-O2
Debug (en anglais seulement)
Debug strict -Worror -Wall -Wextra -pedantic -g -O0--Catch avertissements avant fusion
Clang-debug.Clang.Clang.Clang.Clang.Clang.Clang.Clang.Clang.Clang.Clang.Clang
valgrind-debug ,gcc ,Werror -Wall -Wextra -pedantic -g -O0 , valgrind ,installé

Le mode par défaut est`release`.

## Objectifs les plus utiles

- Oui`all`/ cible par défaut : build standalone exécutable
- Oui`static-lib`: construire une bibliothèque statique pour l'intégration des lobbys
- Oui`tests`: construire tous les binaires de test
- Oui`run-main`: build + exécuter exécutable autonome
- Oui`run-tests`: construire + exécuter tous les tests (avec sortie en direct + logs)
- Oui`rebuild`: propre + tout
- Oui`clean`: supprimer build/ dossier
- Oui`run-gdb`: exécuter un binaire autonome sous gdb

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

- Oui`run-tests`crée`logs/tests-<timestamp>/`avec un fichier .log par test (stdout + stderr)
- Dans`valgrind-debug`mode: supplémentaire`logs/valgrind-<timestamp>/`dossiers avec rapports Valgrind
- Logs seulement enregistrer la sortie non vide
- Fiabilité de sortie de crash: utilise`stdbuf --output=L --error=L`(ligne de tamponnage) lorsque disponible si les dernières lignes printf() apparaissent même sur le plantage SIGABRT ou ASan
- Si`stdbuf`est manquant -> avertissement imprimé une fois, la sortie pourrait tronquer lors d'un crash -> ajouter explicite`fflush(stdout); fflush(stderr);`dans le code d'essai si nécessaire

## Exigences et transférabilité

- Oui`clang-debug`besoin de clang
- Oui`valgrind-debug`besoins en valgrind
- Oui`stdbuf`(à partir de coreutils) est optionnel mais recommandé pour une sortie de test propre sur Linux

## Pages connexes

@ref sub_project_internal_makefile_structure "Makefile Internal Structure": Comment le Makefile est **structure interne**

**Créé :** 16 mars 2026
**Auteur:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"