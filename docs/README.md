# Multi-Mini-Games Template Documentation

**Author: Maxime CHAUVEAU**  
**Date: February 2026**

## How to Generate Documentation

### Prerequisites

- [Doxygen](https://www.doxygen.nl/) installed on your system
- (Optional) [Graphviz](https://graphviz.org/) for dependency graphs

### Installation

```bash
# Ubuntu/Debian
sudo apt-get install doxygen graphviz

# macOS
brew install doxygen graphviz

# Windows (with Chocolatey)
choco install doxygen.install graphviz
```

### Generating Documentation

From the `docs/` folder:

```bash
cd docs
doxygen Doxyfile
```

La documentation sera generee dans `docs/doxygen/html/`.

Ouvrir `docs/doxygen/html/index.html` dans un navigateur.

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
- **Module Tetris**: Regles, architecture (exemple)
- **API Reference**: Documentation de toutes les fonctions et structures
- **Graphiques**: Dependances, collaborations, hierarchies

## Configuration Doxygen

Le `Doxyfile` principal est configure pour:

| Option | Valeur | Description |
|--------|--------|-------------|
| `PROJECT_NAME` | Multi-Mini-Games Template | Nom du projet |
| `PROJECT_BRIEF` | Template pour creer des mini-jeux... | Description courte |
| `INPUT` | lobby, tetris, firstparty | Sources a documenter |
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

*Last updated: February 2026*
