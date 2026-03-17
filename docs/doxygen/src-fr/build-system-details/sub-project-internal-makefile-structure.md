@page sub_project_internal_makefile_structure Sous-projet Structure interne de Makefile

@ref index "Retour à la maison"

** Dernière vérification par rapport à la structure du projet :** 16 mars 2026

Cette page explique la structure interne ** du système Makefile utilisé dans chaque sous-projet (jeux, lobby, etc.).

Nous utilisons une conception **modulaire split-Makefile** pour maintenir les choses à jour, lisibles et plus faciles à étendre plus tard (surtout pour les différentes plateformes).

Toute la logique Makefile réelle vit dans le dossier `make/`.
La racine `Makefile` dans chaque sous-projet est maintenue très courte — elle ne comprend que les fichiers modulaires dans un ordre fixe et déclare des cibles `.PHONY`.

## Présentation du répertoire

```text
sub-project-root/
├── Makefile                  # very short: includes + .PHONY
├── make/
│   ├── 00-config.mk          # compiler, modes, flags, paths, OS detection
│   ├── 10-sources.mk         # finds .c files in src/ and tests/, builds object lists
│   ├── 20-build-rules.mk     # pattern rules (.o from .c, linking, static-lib)
│   ├── 30-targets-main.mk    # main program targets (all, rebuild, run-main, etc.)
│   ├── 40-targets-tests.mk   # test targets (tests, run-tests with logs)
│   ├── 50-tools.mk           # verbosity control, clean, help, stdbuf handling
│   ├── 99-overrides.mk       # optional local overrides (git-ignored)
│   └── platform/             # OS-specific settings (included from 00-config.mk)
│       ├── linux.mk
│       ├── darwin.mk
│       └── mingw.mk
├── makefile.md               # user-facing guide (make help, modes, examples)
├── src/
├── tests/
├── build/                    # generated files
└── ...
```

## Ordre d'inclusion (très important)

Le fichier racine Makefile inclut ces fichiers **exactement dans cette séquence**:

1. `00-config.mk` - ensembles compilateurs, drapeaux, MODE, répertoires, détecte OS, comprend plate-forme/*.mk
2. `10-sources.mk` - scanne src/ et test/ pour les fichiers .c, crée des listes d'objets et de dépendance
3. `20-build-rules.mk` - définit la façon de compiler des fichiers .o et de lier les exécutables/libs statiques
4. `30-cibles-main.mk` - cibles utilisateur pour le programme principal (tous, reconstruction, run-main, run-gdb, statique-lib)
5. `40-cibles-tests.mk` - cibles liées aux essais (essais, essais d ' exécution avec des journaux horodatés)
6. `50-tools.mk` - helper stuff: mode silencieux/verbeux, tout propre, sortie d'aide
7. `99-overrides.mk` - dernier fichier (facultatif) pour les modifications de machines locales, jamais commis

Les premiers fichiers définissent principalement les variables.
Les fichiers ultérieurs utilisent ces variables pour créer des règles et des cibles réelles.

## Pourquoi diviser et numéroter ainsi ?

- Faire évaluer les fichiers dans l'ordre où ils sont inclus — les forces de numérotation comportement prévisible
- Une séparation claire des préoccupations facilite la recherche/modification d'une partie sans briser les autres
- 00 = configuration et environnement
- 10 = découverte de la source
- 20 = mécanique de construction du noyau
- 30/40 = ce que les gens exécutent réellement (faire run-main, faire des tests, etc.)
- 50 = outils de confort du développeur
- 99 = dépassements personnels sans polluant git
- Des trucs spécifiques à la plate-forme restent isolés dans `plateforme/` afin que nous puissions ajouter les différences de Windows/macOS/Linux proprement plus tard

## Tableau de référence rapide

Fichier Principale tâche Principales choses qu'il définit / crée
|-----------------------|--------------------------------------------------------|----------------------------------------------------|
Compilateur, drapeaux, modes, chemins, détection d'OS
10-sources.mk-Scans pour les fichiers sources, crée des listes LIB SOURCES, LIB OBJECTS, TEST BINS, DEPS
20-build-rules.mk
30-cibles-main.mk
40-cibles-tests.mk Construction d'essais et fonctionnement avec des tests logs, tests d'exécution
50-tools.mk
99-overrides.mk
Plate-forme/*.mk

## Pages connexes

- @ref sub project makefile "Guide Makefile orienté vers l'utilisateur" — comment utiliser réellement les cibles et les modes

**Créé :** 23 février 2026
**Dernière mise à jour :** 16 mars 2026
**Auteur:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Retour à la maison"