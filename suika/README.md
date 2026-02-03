# Suika Game (Watermelon Game)

## Description

Un clone du célèbre jeu Suika où vous combinez des fruits pour créer des fruits plus gros, jusqu'à obtenir une pastèque !

## Fonctionnalités

- Physique 2D réaliste avec Box2D
- 11 types de fruits différents (cerise → pastèque)
- Système de fusion : 2 fruits identiques = 1 fruit plus gros
- Score basé sur les fusions
- Game Over quand les fruits dépassent la ligne
- Effets visuels et particules

## Contrôles

- **Souris gauche/droite** : Déplacer le fruit
- **Clic gauche** : Lâcher le fruit
- **R** : Recommencer
- **ESC** : Retour au lobby

## Fruits (du plus petit au plus grand)

1. Cerise 🍒
2. Fraise 🍓
3. Raisin 🍇
4. Orange 🍊
5. Kaki 🟠
6. Pomme 🍎
7. Poire 🍐
8. Pêche 🍑
9. Ananas 🍍
10. Melon 🍈
11. Pastèque 🍉

## Règles

- Deux fruits identiques qui se touchent fusionnent
- Le score augmente à chaque fusion
- Plus le fruit est gros, plus il rapporte de points
- Partie terminée si un fruit dépasse la ligne rouge

## Compilation

```bash
make MODE=clang-debug
make run-main
```
