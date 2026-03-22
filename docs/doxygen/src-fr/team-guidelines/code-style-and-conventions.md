@page code_style_and_conventions Code Style & Conventions

@ref index "Back to Home"

** Dernière vérification par rapport à la structure du projet :** 16 mars 2026

Cette page énumère le style de codage et les conventions que nous suivons dans le projet Multi Mini-Games.
Il est destiné à maintenir le code cohérent à travers l'équipe afin de fusionner les branches et de lire le travail de l'autre reste facile.

Dernière vérification par rapport à la structure du projet: mars 2026

## Conventions de désignation

Fonctions et variables -> camelCase commençant par la lettre minuscule.

Les types Typedef sont PascalCase avec un suffixe qui montre ce qu'ils sont :

- Pas de suffixe pour les types de base/primitive (même les petites structures) ->`f32Vector2`,`u8Vector2`dans`baseTypes.h`
- Oui`_St`pour les structures régulières (y compris les tableaux cultivables) ->`PlayerArray_St`
- Oui`_Et`pour les enums ->`GameScene_Et`
- Oui`_Ut`pour les syndicats
- Oui`_Ft`pour fonction typedefs
- Oui`_t`comme repli si rien d'autre ne correspond

Nous avons des pseudonymes`baseTypes.h`pour le code normal (lorsque la taille exacte n'est pas critique):

```c
typedef unsigned int   uint;
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef float          f32;
typedef double         f64;
// plus vector shortcuts like iVector2, uVector2, etc.
```

Ligne directrice:
Utiliser des types à largeur fixe (`u32`,`s32`, etc.) lorsque la taille/signalité est importante (réseautage, sérialisation, bit ops).
Utilisez les petites (`uint`,`iVector2`, etc.) partout ailleurs pour un gameplay plus propre / code UI.
Éviter la nature`int`/`unsigned int`/`float`dans un nouveau code à moins qu'un lib du 3e parti ne le force.

Ordre des paramètres dans les fonctions (aide à la lisibilité):

- Contexte / poignée d'abord (arena, logger, player struct, etc.)
- Paramètres d'entrée suivants
- Taille/compte juste après leur pointeur (données, dataCount)
- Pointeurs de sortie dernier

## Utilisation statique des mots clés

Utilisation`static`pour tout ce qui devrait rester privé d'un fichier .c:

- Oui`static`fonctions si elles ne font pas partie de l'API publique
- Oui`static`variables/globales

Préférez passer l'état comme params sur file-globals pour éviter les dépendances cachées.

## Formatage et mise en page

Indentation : 4 espaces (sans onglets).

Braces sur la même ligne pour structures de contrôle, fonctions, typesdefs:

```c
if (condition) {
    // code
} else {
    // other
}

while (running) {
    // loop body
}

typedef struct {
    int x;
    int y;
} Point_St;

void someFunc(int a) {
    // body
}
```

Les fonctions triviales monolignes sont bonnes si elles sont courtes :

```c
static u32 hashU32(u32 v) { return v * 2654435761u; }
```

Règles d'espacement:

- Espace après mots clés:`if (`,`while (`,`for (`
- Espace autour des opérateurs binaires:`a + b`,`x = y`
- Des conseils de pointeur :`Type* ptr`(pointeur du type),`Type *array`(pour mémoire)
- placement const: préférence`const Type* ptr`quand le pointé ne devrait pas changer
- Pas de virgules dans les listes,`void func()`pour les paramètres vides

Longueur de la ligne: limite douce ~100 chars, casser logiquement.

Déclarez les variables près de la première utilisation, ou groupe en haut du bloc si plus clair.

Les expressions parenthésiques d'opérateurs mixtes lorsque la préséance pourrait confondre:

```c
// good
if ((flags & MASK) == VALUE) { ... }

// bad (surprising precedence)
if (flags & MASK == VALUE) { ... }
```

## Macros & Préprocesseur

Constantes: CASE ÉCRANGE ->`#define MAX_PLAYERS 4`

Macros de fonction: camelCase ou préfixe, toujours entre parenthèses args + entier expr:

```c
#define MUL(a, b) ((a) * (b))
#define do { ... } while(0)   // for multi-statement
```

## Organisation des fichiers et en-têtes

Noms de fichiers : camelCase, préférez les mots simples (baseTypes.h, lobbyAPI. c)

En-têtes:`#ifndef PATH_LIKE_NAME_H`style ->`#ifndef LOBBY_API_H`

Inclure l'ordre dans .c / .h:

1. En-tête correspondant en premier (en .c)
2. Autres titres de projet
3. Utilisation/en-têtes communs
4. Standard/tiers seulement si nécessaire (préférez via common.h)

## Commentaires et documentation de Doxygen

Chaque fichier .h et .c commence par ce bloc d'en-tête :

```
/**
    @file filename.c
    @author Your Name [or list multiple]
    @date YYYY-MM-DD          // creation date - don't change
    @date YYYY-MM-DD          // last meaningful change - update only for big stuff
    @brief One clear sentence what this file does.

    More context if needed (2-5 lines).

    @note Any warnings, ugly parts we know about, future plans

    Contributors:
        - Name: what you did (keep short)
        - ...
*/
```

Fonctions publiques (en .h) obtenir le bloc Doxygen complet:

```
/**
    @brief Short what the function does.

    Longer explanation if tricky (side effects, special cases).

    @param[in] player Current player state
    @param[out] outRect Filled with collision rect
    @return Pointer to texture or NULL if missing

    @pre player != NULL
    @note Assumes circle shape for collision
*/
Rectangle getPlayerCollisionBox(const Player_st* player);
```

Structs/enums en .h obtenir bref + commentaires sur le terrain:

```
/**
    @brief Player visual state, separate from physics on purpose.
*/
typedef struct {
    Rectangle defaultTextureRect;   ///< Default source rect
    bool      isTextureMenuOpen;    ///< Skin menu visible?
    Texture   textures[__playerTextureCount];   ///< Indexed by PlayerTextureId_Et
} PlayerVisuals_St;
```

Commentaires courts en ligne pour les globals/macros évidents:

```c
extern Rectangle skinButtonRect;        ///< Skin menu toggle area
#define APP_TEXT_FONT_SIZE 32           ///< Base UI font size
```

## Gestion des erreurs et sécurité

- Utilisation`UNREACHABLE`macro pour les cas impossibles
- Contrôles défensifs null/bounds
- Connectez-vous à notre enregistreur (log info, log warn, etc.)
- Oui`goto`ok pour le nettoyage dans les chemins d'erreur - ajouter un commentaire pourquoi
- Pas de nombres magiques -> utilisation`#define`ou enums

## Gestion de la mémoire

Un faible code d'allocation peut utiliser simple`malloc`/`calloc`.
Préférez les arénas (`globalArena`,`tempArena`,`context_alloc`) pour la plupart des choses - fuites plus faciles de suivi.

**Créé :** 15 janvier 2025
**Dernière mise à jour :** 16 mars 2026
**Auteur:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"