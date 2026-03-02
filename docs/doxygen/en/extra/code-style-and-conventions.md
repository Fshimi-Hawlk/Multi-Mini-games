@page code_style Code Style and Conventions

# Code Style & Conventions

@section naming_conventions Naming Conventions

@subsection func_vars Functions & Variables

camelCase, starting with lowercase.

@subsection param_order Function Parameter Ordering

To improve readability of function signatures and callsites:

- Context/handle first (e.g., arena, logger, global state struct).
- Input parameters next.
- Size/count parameters paired with pointers (e.g., @c data, @c dataCount).
- Output parameters last (typically pointers to results).

This ordering makes signatures easier to scan and reduces cognitive load.

@subsection typedef_suffix Typedef Suffix Rules + Shorthand Aliases

All typedef’d types use PascalCase. Suffix indicates kind/complexity:

- @b No suffix: Base/primitive types that feel like built-ins, even if small structs (e.g., integer aliases, @c f32Vector2, @c u8Vector2). These live in @c baseTypes.h and are used as primitives.
- @b _St: Regular structs, including dynamic/growable arrays (e.g., @c SomeStruct_St, @c PlayerArray_St). For dynamic arrays, choose a name that clearly indicates growability.
- @b _Et: Enums (e.g., @c SomeEnum_Et).
- @b _Ut: Unions (e.g., @c SomeUnion_Ut).
- @b _Ft: Function typedefs (e.g., @c SomeFunction_Ft).
- @b _t: Fallback for other typedefs.

@subsection shorthand_aliases Additional Shorthand Aliases (in baseTypes.h)

When exact bit-width or memory layout is @b not the main concern and readability matters more, use these convenient short aliases:

```c
typedef unsigned int   uint;    // instead of unsigned int
typedef unsigned char  uchar;   // instead of unsigned char
typedef unsigned long  ulong;   // instead of unsigned long
typedef unsigned short ushort;  // instead of unsigned short

typedef signed int     sint;    // instead of int (when sign matters)
typedef signed char    schar;   // instead of signed char
typedef signed long    slong;   // instead of signed long
typedef signed short   sshort;  // instead of signed short

typedef float          f32;     // instead of float (32-bit)
typedef double         f64;     // instead of double (64-bit)

typedef size_t         size;    // instead of size_t (rare - use when size_t is too verbose)
typedef ptrdiff_t      ptrdiff; // instead of ptrdiff_t (rare - for signed offsets)
typedef intptr_t       intptr;  // instead of intptr_t (signed pointer-sized int)
typedef uintptr_t      uintptr; // instead of uintptr_t (unsigned pointer-sized int)
typedef intmax_t       intmax;  // instead of intmax_t (max signed int)
typedef uintmax_t      uintmax; // instead of uintmax_t (max unsigned int)
```

@note These shorthands are for convenience and readability in general code. For bit-exact requirements (serialization, bitfields, networking), always use the fixed-width types like @c u32, @c s64 from @c baseTypes.h.

@section commenting_rules Commenting Rules

@subsection general_comments General Comments

Use @c // for single-line comments.  
Use @c /* ... */ for multi-line comments, but prefer single-line where possible.

@subsection doxygen_comments Doxygen Comments for Headers

All public headers and APIs should have Doxygen-style comments.  
The project uses a root Doxyfile for generating documentation.

Example for a simple function:

```c
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

@subsection struct_comments Structs / Enums / Typedefs

Place the comment block before the typedef or struct definition.

Example:

```c
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

@subsection macro_comments Macros and Defines

For simple defines:

```c
#define MAX_PLAYERS 4  ///< Maximum number of players supported
```

For conditional compilation:

```c
#ifndef ASSET_PATH
/**
 * @brief Root directory for all game assets (textures, sounds, fonts, etc.).
 *        Can be overridden at compile time (e.g. -DASSET_PATH="/custom/assets/").
 */
#define ASSET_PATH "assets/"
#endif
```

For macro-like helpers:

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

@section error_handling Error Handling & Safety

- Use @c UNREACHABLE macro for impossible cases.
- Defensive checks (bounds, nulls).
- Logging via custom logger (@c log_info, @c log_warn, etc.).
- goto: Acceptable for error cleanup (e.g., resource release). Justify with comment.
- Avoid magic numbers: Use named constants (@c #define or enum).
  Example: @c #define MAX_PLAYERS 4

@section memory_management Memory Management

- Simple @c malloc / @c calloc acceptable for low-allocation code
- Prefer arena-based allocation (@c globalArena, @c tempArena, @c context_alloc) for easier management.

**Created:** March 02, 2026  
**Author:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)