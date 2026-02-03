# Bowling Mini-Game

## Description

Un mini-jeu de bowling classique en 3D avec physique réaliste.

## Fonctionnalités

- Physique réaliste pour la boule et les quilles
- Vue 3D avec caméra ajustable
- Système de score traditionnel (10 frames)
- Contrôles intuitifs à la souris
- Effets visuels et sonores

## Contrôles

- **Clic gauche + Glisser** : Viser et ajuster la puissance
- **Relâcher** : Lancer la boule
- **Espace** : Réinitialiser la frame
- **ESC** : Retour au lobby

## Structure du Code

```
bowling/
├── src/
│   ├── core/           # Logique du jeu
│   │   ├── physics.c   # Moteur physique
│   │   ├── scoring.c   # Système de score
│   │   └── game.c      # Boucle principale
│   ├── ui/             # Interface utilisateur
│   │   └── hud.c       # Affichage score/infos
│   ├── setups/         # Configuration
│   │   └── init.c      # Initialisation
│   └── utils/          # Utilitaires
│       └── common.c    # Fonctions communes
├── include/            # Headers correspondants
└── tests/              # Tests unitaires
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
