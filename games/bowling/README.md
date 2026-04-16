# Bowling Mini-Game

## Description

Un mini-jeu de bowling classique en 3D avec physique réaliste.

## Auteur

- **Auteur:** Maxime Chauveau
- **Date:** 2026-03-16
- **Version:** 3.0 (Raylib3D)

## Fonctionnalités

- Physique réaliste pour la boule et les quilles
- Vue 3D avec caméra ajustable
- Système de score traditionnel (10 frames)
- Contrôles intuitifs à la souris
- Effets visuels et sonores
- Décor 3D complet avec environnement

## Contrôles

- **Clic gauche + Glisser** : Viser et ajuster la puissance
- **Relâcher** : Lancer la boule
- **Espace** : Réinitialiser la frame
- **ESC** : Retour au lobby

## Structure du Code

```
bowling/
├── src/
│   ├── main.c            # Point d'entrée
│   ├── bowlingAPI.c     # API principale du jeu
│   ├── physics.c        # Moteur physique (ball, quilles, particules)
│   ├── utils/
│   │   └── assets.c     # Chargement des assets
├── include/
│   ├── bowlingAPI.h     # API publique
│   ├── physics.h        # Déclarations du module physique
│   └── utils/
│       ├── types.h      # Types de base
│       └── configs.h    # Chemins des assets
├── assets/              # Textures (boules, quilles, piste)
├── docs/                # Documentation Doxygen
└── build/               # Objets compilés et binaires
```

## Compilation

```bash
make MODE=clang-debug
make run-main
```

## Tests

```bash
make run-tests
```
