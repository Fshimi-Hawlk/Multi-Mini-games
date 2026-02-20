# **Code Style & Conventions**

## **Naming Conventions**

### **Functions & Variables**

camelCase, starting with lowercase.

### **Function Parameter Ordering**

To improve readability of function signatures and callsites:

- Context/handle first (e.g., arena, logger, global state struct).
- Input parameters next.
- Size/count parameters paired with pointers (e.g., `data, dataCount`).
- Output parameters last (typically pointers to results).

This ordering makes signatures easier to scan and reduces cognitive load.

### Typedef Suffix Rules + Shorthand Aliases

All typedef’d types use PascalCase. Suffix indicates kind/complexity:

- **No suffix**: Base/primitive types that feel like built-ins, even if small structs (e.g., integer aliases, `f32Vector2`, `u8Vector2`). These live in `baseTypes.h` and are used as primitives.
- **_St**: Regular structs, including dynamic/growable arrays (e.g., `SomeStruct_St`, `PlayerArray_St`). For dynamic arrays, choose a name that clearly indicates growability.
- **_Et**: Enums (e.g., `SomeEnum_Et`).
- **_Ut**: Unions (e.g., `SomeUnion_Ut`).
- **_Ft**: Function typedefs (e.g., `SomeFunction_Ft`).
- **_t**: Fallback for other typedefs.

**Additional shorthand aliases (in `baseTypes.h`)**

When exact bit-width or memory layout is **not** the main concern and readability matters more, use these convenient short aliases:

```c
typedef unsigned int   uint;    // instead of unsigned int everywhere
typedef unsigned char  uchar;
typedef unsigned short ushort;
// ... similarly for ulong, ullong, iVector2, uVector2, etc.
```

### Guideline for the team:

- Prefer the explicit fixed-width types (`u32`, `s32`, `u64`, etc.) when size or signedness is semantically important (networking, serialization, bit manipulation, performance-critical arrays, etc.).
- Prefer the short aliases (`uint`, `iVector2`, `uVector2`, etc.) in normal gameplay code, UI logic, counters, indices, etc. - this is exactly why they exist.
- Never fall back to plain `int` / `unsigned int` / `float` in new code unless there is a very specific reason (e.g. interacting with a third-party API that demands it).

## **Static Keyword Usage**

Use `static` to keep things private to a file/module unless sharing is required:

- Functions: `static` in .c files if not public API.
- Variables: `static` for file-scope globals. Prefer passing state via parameters over non-static globals.

This reduces name clashes.

## **Formatting**

### **Indentation**:

4 spaces

### **Braces**:

Opening brace on same line for control structures, types and function definitions.

```C
while (...) {
    ...
}

if (...) {
    ...
} else if (...) {
    ...
} else {
    ...
}

switch (...) {
    case ...: {
        ...
    } break;

    case ...: {
        ...
    } break;

    default: {
        ...
    }

    // Prefer braces, but if single-statement, the following is allowed:

    case ...: ...; break;

    default: ...
}

typedef struct {
    ...
} SomeStruct_St;

typedef enum {
    ...
} SomeEnum_Et;

typedef union {
    ...
} SomeUnion_Ut;

ReturnType someFunc(...) {
    ...
}
```

**Note on forward declarations**: If absolutely needed, use a tag temporarily:

```c
typedef struct SomeStruct_s SomeStruct_St;

struct SomeStruct_s {
    ...
};
```

### Single- vs Multi-Line Functions

Trivial functions (1–2 simple statements) may be written on a single line to reduce vertical noise:

```c
static u32 hashU32(u32 v) { return v * 2654435761u; }
```

Longer bodies must use multi-line formatting with braces on separate lines.

### **Spacing**:

- Space after keywords (`if (...)`, `while (...)`, `for (...; ...; ...)`).

- No space before `;` or `)`.

- Space around binary operators (`= + - < > * / % | & ^`).

- Pointer Semantic hints:  
`Type* var` -> **a pointer to the type**.  
`Type *array` -> **array of `Type`**.  

- const placement:

```c
void func(Type var);                  // Value can be changed
void func(const Type var);            // Value unchanged

void func(Type* var);                 // Pointer and pointee can be changed
void func(const Type* var);           // Pointee unchanged, pointer can change
void func(Type* const var);           // Pointee can change, pointer unchanged
void func(const Type* const var);     // Neither can change
```

- Return values: Use `const` for data the caller shouldn't modify.
`const char* getVersionString() { return "1.0"; }`

- **Parameter Lists**: void func(Type1 param1, Type2 param2) – no trailing comma, void for empty params.

- **Line Length**: Soft limit <100 characters. Break long lines (expressions, comments, string literals, macro definitions) logically.

- **Variables Declaration Placement**: Declare near first use; group at block top if clearer.

### Operator Precedence and Parentheses

Parenthesize non-trivial expressions with mixed operators to avoid precedence bugs and improve clarity:

```c
// Preferred
if ((flags & MASK) == VALUE) { ... }

// Avoid surprising precedence
if (flags & MASK == VALUE) { ... }
```

### Macros and Preprocessor

- Naming: 
    - SCREAMING_CASE for constants (`#define MAX_PLAYERS 4`).
    - camelCase or prefixed for function-like macros (e.g., `alignUpPow2`, `castTo`, `da_printContent`, `log_warn`).

- Safety: Always parenthesize arguments and the entire expression.
- Good: `#define MUL(a, b) ((a) * (b))`

- Macros using `do { ... } while(0)` for multi-statement blocks.

## Structure

### File Naming and Organization

- Naming: camelCase for files (e.g., `baseTypes.h`). Keep single-word where possible.
- Folders: Group by purpose (e.g., utils/, core/, setup/, core/game/, etc.).

### Header Files

- **Guards**: `#ifndef PATH_LIKE_NAME_H` / `#define` / `#endif // PATH_LIKE_NAME_H`.  
Example: `#ifndef CORE_GAME_BOARD_H`.

- **Self-containment**: Headers include what they need; avoid forward declarations.

- Content split:  
    - .h: Public types, function declarations, shared macros/constants, extern globals.
    - .c: Function definitions, file-static variables/functions, module-private types.

### Include Order

In .c and .h files:

1. Corresponding header first (in .c files).
2. Other project headers.
3. Utils headers.
4. Rare: Third-party/standard library (prefer via common.h).

**Goal**: Most files should at the very least need `#include "common.h"` (or a similar utils header) for basics. Only add specific includes if truly needed for that file and that is not in `common.h` in case it's very niche.

Example in a .c file:
```
#include "myGame.h"  // corresponding header

#include "core/game/board.h"  // other project header

#include "utils/gloabs.h"  // utils header

// Rare:
#include <someNicheLibrary.h>  // if not in common.h
```

## Comments & Documentation

Good documentation is what makes the difference between "this code is readable for a week" and "this code is still understandable six months later when someone new has to fix a bug at 2 a.m.".  
Most of us are not documentation wizards - that's fine. Just follow these patterns consistently and we'll all save each other a lot of time.

### File Headers - the mandatory part at the top

Every single `.h` and `.c` file starts with exactly this block. No exceptions.

```
/**
 * @file filename.c / filename.h
 * @author <Your name here> [or list multiple if you really co-authored from the start]
 * @date YYYY-MM-DD              // Creation date - never touch this one again
 * @date YYYY-MM-DD              // Last time someone meaningfully changed this file (update only when needed)
 * @brief One clear sentence that tells what this file is actually for.
 *
 * Contributors:
 *   - <Name>:
 *     - What you added / changed / fixed (keep it short)
 *   - <Name>:
 *     - ...
 *
 * If the file needs more context than fits in @brief, write 2-5 lines here.
 * Use @see `path/to/related/file.h` when this file depends heavily on another one.
 *
 * @note Put warnings, important limitations, "we know it's ugly but...", or future plans here
 */
```

Important notes so nobody gets confused:
- **Second date** => only update when you do **something important** (new feature, big refactor, removing dead code, serious bugfix). Don't bump it for typo fixes or reformatting.
- `@brief` => one line, no fluff. 
    *Good*: "Central tuning constants for physics and window setup." 
    *Bad*: "This file contains many useful defines that are used everywhere in the game."
- **Contributors block** => optional. If multiple people did real work, list them. 
    In *pure-header files* (`configs.h`, `userTypes.h`, `common.h`, `globals.h`, etc.) it's fine to have contributors here. 
    In *files that have a `.c` counterpart* (`globals.c`, `utils.c`, etc.), put the detailed contributor list in the `.c` file instead.
- `@note` => use it when something is surprising or fragile (example: "hardcoded platform positions - will be replaced by level files later").

### Function comments - only for things exposed in `.h`

If the function is public (i.e. declared in `.h`, not static in `.c`), it gets a proper Doxygen block right above the declaration in the header.

```c
/**
 * @brief One sentence that says what the function actually does.
 *
 * If there's anything subtle, non-obvious or important to know before calling it,
 * explain it here (2-6 lines max usually).
 *
 * @param[in]     player       Current player state - position, velocity, etc.
 * @param[out]    outRect      Filled with the world-space collision rectangle
 * @return                     Pointer to the texture, or NULL if not found
 *
 * @pre  player != NULL and player->radius > 0
 * @post outRect is always axis-aligned and fully contains the player circle
 * @note Uses radius*2 for width/height - assumes circle collision shape
 */
Rectangle getPlayerCollisionBox(const Player_st* player);
```

Quick cheat-sheet:
- `@brief` - always, one line.
- **Longer explanation** - only when the function has side-effects, tricky preconditions, or unusual return behavior.
- `@param` - mark `[in]`, `[out]`, `[in,out]` so people know what they can modify.
- `@return` - always explain what success/failure looks like, especially when returning Error_Et.
- `@pre` / `@post` / `@note` - add only when not calling the function correctly would crash or do something very wrong.

Static functions inside `.c` files usually only need a short `//` comment above them, or none if the name is obvious.  
*But that doesn't mean you can skip it entirely*.

### Structs, enums, typedefs - explain the why and the ownership

Whenever a type is public (in `.h`), give it a `@brief` and comment the **fields/values** that aren't screamingly obvious.

Struct example (from `userTypes.h` style):
```c
/**
 * @brief Visual and rendering state of the player - kept separate from physics on purpose.
 */
typedef struct {
    Rectangle defaultTextureRect;               ///< Default source rect when no animation/special state
    bool      isTextureMenuOpen;                ///< Is the skin selection overlay visible right now?
    Texture   textures[__playerTextureCount];   ///< Preloaded textures - indexed by PlayerTextureId_Et
} PlayerVisuals_St;
```

Enum example:
```c
/**
 * @brief All available font sizes - used to index fonts[] array.
 *
 * The sentinel _fontSizeCount is not a real size - it's just the array length.
 */
typedef enum {
    FONT8,
    FONT10, FONT12, FONT14, FONT16, FONT18,
    FONT20, FONT22, FONT24, FONT26, FONT28,
    FONT30, FONT32, FONT34, FONT36, FONT38,
    FONT40, FONT42, FONT44, FONT46, FONT48,
    _fontSizeCount
} FontSize_Et;
```

### Globals and macros - don't leave people guessing

- Extern globals or file-scope statics -> prefer short `///<` comment on the same line when the meaning is obvious
- When more explanation is needed (units, ownership, future plans, conditional compilation context), use a full multi-line `/** ... */` block just above the declaration

**Short single-line style** (globals.h style):
```c
extern Rectangle skinButtonRect;        ///< Clickable area for the skin menu toggle button
```

**Multi-line style when more description helps** (configs.h style):
```c
/**
 * @brief Default base font size for most UI text and in-game labels.
 *        Other sizes are usually derived from this via the fonts[] array.
 */
#define APP_TEXT_FONT_SIZE 32
```

**Conditional compilation case** - place the documentation inside the block if the define is conditional:
```c
#ifndef ASSET_PATH
/**
 * @brief Root directory for all game assets (textures, sounds, fonts, etc.).
 *        Can be overridden at compile time (e.g. -DASSET_PATH="/custom/assets/").
 */
#define ASSET_PATH "assets/"
#endif
```

**Macro-like helper** (utils.h style):
```c
/**
 * @brief Returns the full texture rectangle {0, 0, w, h}.
 *
 * Used everywhere before DrawTexture* calls.
 * Makes it easy to later support sprite sheets or trimmed textures.
 *
 * @param texture Valid Raylib Texture2D
 * @return Rectangle spanning the entire texture
 */
Rectangle getTextureRec(const Texture texture);
```

## Error Handling & Safety

- `UNREACHABLE` macro for impossible cases.

- Defensive checks (bounds, nulls).

- Logging via custom logger (`log_info`, `log_warn`, etc.).

- goto: Acceptable for error cleanup (e.g., resource release). Justify with comment.

- Avoid magic numbers: Use named constants (`#define` or `enum`).
  Example: `#define MAX_PLAYERS 4`

## Memory Management

- Simple `malloc`/`calloc` acceptable for low-allocation code

- Prefer arena-based allocation (`globalArena`, `tempArena`, `context_alloc`) for easier management.