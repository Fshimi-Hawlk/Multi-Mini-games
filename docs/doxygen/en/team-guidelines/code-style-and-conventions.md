@page code_style_and_conventions Code Style and Conventions

## Naming Conventions

### Functions & Variables

camelCase, starting with lowercase.

### Function Parameter Ordering

To improve readability of function signatures and callsites:

- Context/handle first (e.g., arena, logger, global state struct).
- Input parameters next.
- Size/count parameters paired with pointers (e.g., `data`, `dataCount`).
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

### Additional Shorthand Aliases (in baseTypes.h)

When exact bit-width or memory layout is **not** the main concern and readability matters more, use these convenient short aliases:

```
typedef unsigned int   uint;    // instead of unsigned int
typedef unsigned char  uchar;   // instead of unsigned char
typedef unsigned long  ulong;   // instead of unsigned long
typedef unsigned short ushort;  // instead of unsigned short

typedef float          f32;     // instead of float (32-bit)
typedef double         f64;     // instead of double (64-bit)
```

**Note**: These shorthands are for convenience and readability in general code. For bit-exact requirements (serialization, bitfields, networking), always use the fixed-width types like `u32`, `s64` from `baseTypes.h`.

## Commenting Rules

### General Comments

Use `//` for single-line comments.  
Use `/* ... */` for multi-line comments, but prefer single-line where possible.

### Doxygen Comments for Headers

All public headers and APIs should have Doxygen-style comments.  
The project uses a root Doxyfile for generating documentation.

Example for a simple function:

```
/**
 * @brief Brief description of the function.
 *
 * Longer description if needed.
 * Can span multiple lines.
 *
 * @param[in] inputParam Description of input parameter.
 * @param[out] outputParam Description of output parameter.
 * @return Description of return value.
 *
 * @note Any important notes.
 * @warning Any warnings.
 * @see RelatedFunction
 */
int someFunction(int inputParam, int* outputParam);
```

### Structs / Enums / Typedefs

Place the comment block before the typedef or struct definition.

Example:

```
/**
 * @brief Brief description of the struct.
 *
 * Detailed description.
 */
typedef struct {
    int field1;  ///< Description of field1
    float field2; ///< Description of field2
} SomeStruct_St;
```

### Macros and Defines

For simple defines:

```
#define MAX_PLAYERS 4  ///< Maximum number of players supported
```

For conditional compilation:

```
#ifndef ASSET_PATH
/**
 * @brief Root directory for all game assets (textures, sounds, fonts, etc.).
 *        Can be overridden at compile time (e.g. -DASSET_PATH="/custom/assets/").
 */
#define ASSET_PATH "assets/"
#endif
```

For macro-like helpers:

```
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

**Created:** March 02, 2026  
**Author:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref mainpage "Back to Home"