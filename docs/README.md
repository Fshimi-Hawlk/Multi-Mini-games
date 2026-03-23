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

The documentation will be generated in `docs/doxygen/html/`.

Open `docs/doxygen/html/index.html` in a browser.

## Documentation Structure

```
docs/
├-- Doxyfile              # Main Doxygen configuration
├-- mainpage.dox          # Documentation homepage
├-- README.md             # This file
├-- API_Conversion.md     # API conversion guide
├-- makefile.md           # Makefile documentation
└-- doxygen/              # Generated documentation (gitignored)
    └-- html/
        └-- index.html    # Entry page
```

## Documentation Content

- **Main Page**: Project description, structure, compilation
- **Lobby Module**: Architecture, API, mini-games integration
- **Tetris Module**: Rules, architecture (example)
- **API Reference**: Documentation of all functions and structures
- **Graphics**: Dependencies, collaborations, hierarchies

## Doxygen Configuration

The main `Doxyfile` is configured as follows:

| Option | Value | Description |
|--------|-------|-------------|
| `PROJECT_NAME` | Multi-Mini-Games Template | Project name |
| `PROJECT_BRIEF` | Template to create mini-games... | Short description |
| `INPUT` | lobby, tetris, firstparty | Sources to document |
| `RECURSIVE` | YES | Traverse subdirectories |
| `EXTRACT_ALL` | YES | Document even without comments |
| `GENERATE_HTML` | YES | Generate HTML documentation |
| `HAVE_DOT` | YES | Use Graphviz |
| `CALL_GRAPH` | YES | Generate call graphs |

## Documentation Conventions

### Doxygen Comments

Use Javadoc style:

```c
/**
    @file myFile.c
    @brief Short description of the file
    @author Maxime CHAUVEAU
    @date February 2026

    Detailed description of the file...
*/

/**
    @brief Short description of the function

    Detailed description if necessary.

    @param param1 Description of parameter 1
    @param param2 Description of parameter 2
    @return Description of the return value
*/
int myFunction(int param1, const char* param2);
```

### Common Tags

| Tag | Usage |
|-----|-------|
| `@file` | File name and description |
| `@brief` | Short description |
| `@author` | Code author |
| `@date` | Creation/modification date |
| `@param` | Parameter description |
| `@return` | Return value description |
| `@see` | Cross reference |
| `@note` | Important note |
| `@warning` | Warning |
| `@todo` | Task to do |

## Author

**Maxime CHAUVEAU**  
Maxime.Chauveau.Etu@univ-lemans.fr

---

*Last updated: February 2026*
