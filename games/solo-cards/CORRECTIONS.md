# Changements et Corrections - Solitaire

## Date: Mars 2026

### Auteur
Maxime CHAUVEAU

### Problèmes corrigés - Session Mars 2026:

#### 1. Bug du socle de pioche (stock pile)
- **Avant**: Le socle affichait un as animé quand la pile était vide
- **Après**: Le socle affiche maintenant un rectangle vide uniquement
- **Fichiers modifiés**: `src/ui/renderer.c`

#### 2. Cartes non distribuées aléatoirement
- **Avant**: Les cartes étaient toujours distribuées dans le même ordre (pas de srand)
- **Après**: Ajout de `srand((unsigned int)time(NULL))` dans `solitaire_init()`
- **Fichiers modifiés**: `src/core/game.c`

#### 3. Zone de clic pour recycler la pioche
- **Avant**: Zone de clic chevauchait entre le stock et le waste
- **Après**: Zone de clic unique sur le rectangle du stock uniquement
- **Fichiers modifiés**: `src/core/game.c`

#### 4. Double carte affichée lors du drag
- **Avant**: La carte cliquée apparaissait à la fois à sa position et sous la souris
- **Après**: La carte n'est affichée qu'à la position de la souris pendant le drag
- **Fichiers modifiés**: `src/ui/renderer.c`

#### 5. Cartes suivantes ne suivaient pas
- **Avant**: En cliquant sur une carte au milieu d'une pile, seules cette carte bougeait
- **Après**: Toutes les cartes au-dessus de la carte cliquée suivent maintenant
- **Fichiers modifiés**: `src/core/game.c`, `src/ui/renderer.c`

#### 6. Centrage de la carte sur la souris
- **Avant**: La carte était décalée par rapport à la position du clic
- **Après**: La carte est maintenant centrée sur le curseur de la souris
- **Fichiers modifiés**: `src/core/game.c`, `src/ui/renderer.c`

#### 7. Suppression incorrecte des cartes
- **Avant**: Le code ne retirait qu'une seule carte de la pile source
- **Après**: Le code retire maintenant le bon nombre de cartes (dragCount)
- **Fichiers modifiés**: `src/core/game.c`

#### 8. Détection de victoire
- **Vérification**: La détection de victoire est appelée à chaque frame
- **Fichiers modifiés**: `src/core/game.c`

#### 9. Détection de défaite
- **Ajout**: Vérification automatique toutes les 2 minutes
- **Logique**: Vérifie s'il reste des mouvements possibles
- **Fichiers modifiés**: `src/core/game.c`, `include/solitaire.h`, `src/ui/renderer.c`

#### 10. Compilation avec tous les modes
- **Vérification**: Le code compile avec release, debug, strict-debug, clang-debug, valgrind-debug
- **Fix**: Correction du warning unused parameter `scale` dans `DrawCardFromAtlas`
- **Fichiers modifiés**: `src/ui/renderer.c`

---

## Date: Février 2026

### Auteur
Maxime CHAUVEAU

### Problèmes corrigés:

#### 1. Problème de chargement des assets
- **Avant**: Le message d'erreur mentionnait "cardBack_blue5.png" alors que le fichier chargé était "cardBack_blue3.png"
- **Après**: Le message d'erreur correspond maintenant au nom du fichier réellement chargé
- **Avant**: La texture menuBackground était déclarée mais jamais chargée, causant un crash potentiel
- **Après**: menuBackground est maintenant initialisé correctement (texture vide) et le menu utilise un fond de couleur
- **Avant**: Les textures étaient chargées avec un chemin relatif incorrect depuis le répertoire build/bin/
- **Après**: Recherche de fichier avec plusieurs chemins relatifs pour trouver les assets correctement

#### 2. Optimisation du chargement des assets
- **Avant**: Assets chargés/déchargés à chaque frame (~60 fois/seconde)
- **Après**: Assets chargés une seule fois dans `solitaire_init()` et libérés dans `solitaire_cleanup()`

#### 3. Renommages de structures
- **SolitaireGame_St → SolitaireGameState**: Renommé partout dans le code pour plus de clarté
  - solitaire.h
  - game.c
  - main.c
  - renderer.h
  - renderer.c
  
- **SolitaireGame_Set → SolitaireGame_St**: Transformation complète de la structure API
  - solitaireAPI.h
  - solitaireAPI.c

#### 4. Nettoyage du code Tetris
Fichiers supprimés (code Tetris non pertinent pour le Solitaire):
- include/core/shape.h + src/core/shape.c
- include/core/board.h + src/core/board.c
- include/core/algo.h + src/core/algo.c
- include/ui/shape.h + src/ui/shape.c
- include/ui/board.h + src/ui/board.c
- include/ui/game.h + src/ui/game.c

Fichiers nettoyés:
- **utils/types.h**: Supprimé tetramino, boardShape_st, speed_st, inputRepeat_st, shapeId, board_t
- **utils/globals.h/c**: Supprimé tetraminosColors, tetraminosShapes, inputRepeat
- **solitaireAPI.c**: Complètement réécrit pour le Solitaire (avant: logique Tetris avec clearedLines, score de Tetris, etc.)

#### 5. Améliorations du rendu (renderer.c)
- Ajout de `DrawTexturePro` pour redimensionner correctement les cartes (140x190 → 80x110)
- Affichage des emplacements vides avec `DrawEmptySlot` et `DrawEmptySlotWide`
- Symboles des as animés dans les emplacements vides (stock et foundations)
- Zone de défausse élargie (`WASTE_ZONE_WIDTH`) avec affichage de 3 cartes max

#### 6. Améliorations de la documentation
- Ajout de commentaires détaillés dans tous les fichiers modifiés
- Commentaires en français pour mieux expliquer la logique
- Documentation des fonctions dans l'API
- Auteur mis à jour: Maxime CHAUVEAU

#### 7. Intégration Lobby
- API standardisée suivant le pattern du projet
- Structure `MiniGame_St` pour chargement dynamique des mini-jeux
- Gestion gracieuse des erreurs (jeu non disponible si API retourne NULL)

### Structure finale propre:
```
solitaire/
├── include/
│   ├── core/
│   │   └── game.h
│   ├── ui/
│   │   └── renderer.h
│   ├── utils/
│   │   ├── common.h
│   │   ├── configs.h
│   │   ├── globals.h      [nettoyé]
│   │   ├── types.h        [nettoyé]
│   │   └── utils.h
│   ├── solitaire.h
│   └── solitaireAPI.h     [réécrit]
├── src/
│   ├── core/
│   │   └── game.c         [commenté]
│   ├── ui/
│   │   └── renderer.c     [corrigé + amélioré]
│   ├── utils/
│   │   ├── globals.c      [nettoyé]
│   │   └── utils.c
│   ├── main.c
│   └── solitaireAPI.c     [réécrit]
└── assets/
    └── solitaire/
        ├── cardBack_blue3.png
        └── playingCards.png
```

### Notes importantes:
- Tous les fichiers ont été vérifiés et testés
- Le code compile sans erreur ni warning
- Les assets se chargent correctement
- La structure est maintenant propre et spécifique au Solitaire
- Plus aucune trace de code Tetris dans le projet
- Intégration complète avec le lobby
