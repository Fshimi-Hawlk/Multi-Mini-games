# Documentation Multi-Mini-Games

**Auteur: Maxime CHAUVEAU**  
**Date: Fevrier 2026**

## Comment generer la documentation

### Pre-requis

- [Doxygen](https://www.doxygen.nl/) installe sur votre systeme
- (Optionnel) [Graphviz](https://graphviz.org/) pour les graphiques de dependance

### Installation

```bash
# Ubuntu/Debian
sudo apt-get install doxygen graphviz

# macOS
brew install doxygen graphviz

# Windows (avec Chocolatey)
choco install doxygen.install graphviz
```

### Generation de la documentation

#### Documentation complete du projet

Depuis le dossier `Multi-Mini-games/docs/`:

```bash
cd Multi-Mini-games/docs
doxygen Doxyfile
```

La documentation sera generee dans `docs/doxygen/html/`.

Ouvrir `docs/doxygen/html/index.html` dans un navigateur.

#### Documentation d'un module specifique

Chaque module (lobby, suika) possede son propre `Doxyfile.min`:

```bash
# Documentation du lobby
cd Multi-Mini-games/lobby
doxygen Doxyfile.min

# Documentation du jeu Suika
cd Multi-Mini-games/suika
doxygen Doxyfile.min
```

### Via Makefile

```bash
# Depuis la racine du projet
make docs

# Depuis un module specifique
cd suika && make docs
```

## Structure de la documentation

```
docs/
├── Doxyfile              # Configuration Doxygen principale
├── mainpage.dox          # Page d'accueil de la documentation
├── README.md             # Ce fichier
├── API_Conversion.md     # Guide de conversion API
├── makefile.md           # Documentation Makefile
└── doxygen/              # Documentation generee (gitignore)
    └── html/
        └── index.html    # Page d'entree
```

## Contenu de la documentation

- **Page principale**: Description du projet, structure, compilation
- **Module Lobby**: Architecture, API, integration des mini-jeux
- **Module Suika**: Regles, physique, assets
- **API Reference**: Documentation de toutes les fonctions et structures
- **Graphiques**: Dependances, collaborations, hierarchies

## Configuration Doxygen

Le `Doxyfile` principal est configure pour:

| Option | Valeur | Description |
|--------|--------|-------------|
| `PROJECT_NAME` | Multi-Mini-Games | Nom du projet |
| `PROJECT_BRIEF` | Collection de mini-jeux... | Description courte |
| `INPUT` | lobby, suika, firstparty | Sources a documenter |
| `RECURSIVE` | YES | Parcourir les sous-dossiers |
| `EXTRACT_ALL` | YES | Documenter meme sans commentaires |
| `GENERATE_HTML` | YES | Generer la documentation HTML |
| `HAVE_DOT` | YES | Utiliser Graphviz |
| `CALL_GRAPH` | YES | Generer les graphes d'appels |

## Conventions de documentation

### Commentaires Doxygen

Utiliser le style Javadoc:

```c
/**
    @file monFichier.c
    @brief Description courte du fichier
    @author Maxime CHAUVEAU
    @date Fevrier 2026

    Description detaillee du fichier...
*/

/**
    @brief Description courte de la fonction

    Description detaillee si necessaire.

    @param param1 Description du parametre 1
    @param param2 Description du parametre 2
    @return Description de la valeur de retour
*/
int maFonction(int param1, const char* param2);
```

### Tags courants

| Tag | Usage |
|-----|-------|
| `@file` | Nom et description du fichier |
| `@brief` | Description courte |
| `@author` | Auteur du code |
| `@date` | Date de creation/modification |
| `@param` | Description d'un parametre |
| `@return` | Description de la valeur de retour |
| `@see` | Reference croisee |
| `@note` | Note importante |
| `@warning` | Avertissement |
| `@todo` | Tache a faire |

## Auteur

**Maxime CHAUVEAU**  
Maxime.Chauveau.Etu@univ-lemans.fr

---

*Derniere mise a jour: Fevrier 2026*
