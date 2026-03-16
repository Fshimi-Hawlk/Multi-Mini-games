@page code_style_and_conventions Code Style & Conventions

@ref index "Back to Home"

**Last checked against project structure:** March 16, 2026  

This page lists the coding style and conventions we follow in the Multi Mini-Games project.  
It's meant to keep the code consistent across the team so merging branches and reading each other's work stays easy.

Last checked against project structure: March 2026

## Naming Conventions

Functions and variables -> camelCase starting with lowercase letter.

Typedef types are PascalCase with a suffix that shows what they are:

- No suffix for base/primitive-like types (even small structs) -> `f32Vector2`, `u8Vector2` in `baseTypes.h`
- `_St` for regular structs (including growable arrays) -> `PlayerArray_St`
- `_Et` for enums -> `GameScene_Et`
- `_Ut` for unions
- `_Ft` for function typedefs
- `_t` as fallback if nothing else fits

We have shorthand aliases in `baseTypes.h` for normal code (when exact size isn't critical):

```c
typedef unsigned int   uint;
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef float          f32;
typedef double         f64;
// plus vector shortcuts like iVector2, uVector2, etc.
```

Guideline:  
Use fixed-width types (`u32`, `s32`, etc.) when size/signedness matters (networking, serialization, bit ops).  
Use the short ones (`uint`, `iVector2`, etc.) everywhere else for cleaner gameplay/UI code.  
Avoid plain `int`/`unsigned int`/`float` in new code unless a 3rd-party lib forces it.

Parameter ordering in functions (helps readability):

- Context/handle first (arena, logger, player struct, etc.)
- Input params next
- Size/count right after their pointer (data, dataCount)
- Output pointers last

## Static Keyword Usage

Use `static` for anything that should stay private to one .c file:

- `static` functions if not part of public API
- `static` file-scope variables/globals

Prefer passing state as params over file-globals to avoid hidden dependencies.

## Formatting & Layout

Indentation: 4 spaces (no tabs).

Braces on same line for control structs, functions, typedefs:

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

Single-line trivial functions are ok if short:

```c
static u32 hashU32(u32 v) { return v * 2654435761u; }
```

Spacing rules:

- Space after keywords: `if (`, `while (`, `for (`
- Space around binary operators: `a + b`, `x = y`
- Pointer hints: `Type* ptr` (pointer to Type), `Type *array` (array of Type)
- const placement: prefer `const Type* ptr` when pointee shouldn't change
- No trailing comma in param lists, `void func()` for empty params

Line length: soft limit ~100 chars, break logically.

Declare variables near first use, or group at top of block if clearer.

Parenthesize mixed-operator expressions when precedence could confuse:

```c
// good
if ((flags & MASK) == VALUE) { ... }

// bad (surprising precedence)
if (flags & MASK == VALUE) { ... }
```

## Macros & Preprocessor

Constants: SCREAMING_CASE -> `#define MAX_PLAYERS 4`

Function-like macros: camelCase or prefixed, always parenthesize args + whole expr:

```c
#define MUL(a, b) ((a) * (b))
#define do { ... } while(0)   // for multi-statement
```

## File & Header Organization

File names: camelCase, prefer single words (baseTypes.h, lobbyAPI.c)

Header guards: `#ifndef PATH_LIKE_NAME_H` style -> `#ifndef LOBBY_API_H`

Include order in .c / .h:

1. Corresponding header first (in .c)
2. Other project headers
3. Utils/common headers
4. Standard/third-party only if really needed (prefer via common.h)

## Comments & Doxygen Documentation

Every .h and .c file starts with this header block:

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

Public functions (in .h) get full Doxygen block:

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

Structs/enums in .h get brief + field comments:

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

Short inline comments for obvious globals/macros:

```c
extern Rectangle skinButtonRect;        ///< Skin menu toggle area
#define APP_TEXT_FONT_SIZE 32           ///< Base UI font size
```

## Error Handling & Safety

- Use `UNREACHABLE` macro for impossible cases
- Defensive null/bounds checks
- Log with our logger (log_info, log_warn, etc.)
- `goto` ok for cleanup in error paths - add comment why
- No magic numbers -> use `#define` or enums

## Memory Management

Low allocation code can use plain `malloc`/`calloc`.  
Prefer arena allocators (`globalArena`, `tempArena`, `context_alloc`) for most things - easier leaks tracking.

**Created:** January 15, 2025  
**Last updated:** March 16, 2026  
**Author:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"