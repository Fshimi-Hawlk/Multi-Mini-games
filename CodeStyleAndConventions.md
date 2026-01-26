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

### Typedef Suffix Rules

All typedef’d types use PascalCase. Suffix indicates kind/complexity:

- **No suffix**: Base/primitive types that feel like built-ins, even if small structs (e.g., integer aliases, `f32Vector2`, `u8Vector2`). These live in `baseTypes.h` and are used as primitives.
- **_St**: Regular structs, including dynamic/growable arrays (e.g., `SomeStruct_St`, `PlayerArray_St`). For dynamic arrays, choose a name that clearly indicates growability.
- **_Et**: Enums (e.g., `SomeEnum_Et`).
- **_Ut**: Unions (e.g., `SomeUnion_Ut`).
- **_Ft**: Function typedefs (e.g., `SomeFunction_Ft`).
- **_t**: Fallback for other typedefs.

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

- **Function Documentation**: Doxygen-style `/** @brief ... */` blocks in headers, see [doxygen documetation](https://www.doxygen.nl/manual/docblocks.html).

- **Inline Comments**: `//` for short notes, `/* ... */` for multi-line or disabled code.

- **File Headers**: Include author, date, brief description.

- **TODO/Warnings**: Plain comments, `TODO` macro or `log_warn` macro.

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