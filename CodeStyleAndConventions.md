# **Code Style & Conventions**

## **Naming Conventions**

 ### **Functions & Variables**:  

   camelCase, i.e. starting with lowercase.  

 ### **Types & Declarations**:  

 - Custom integer aliases in `baseTypes.h` (`u8`, `s8`, `u32`, etc.).

 - `bool8` for boolean (raylib compatibility).

 - Vector types: `u8Vector2`, `f32Vector2`, etc. (no suffix – treated as base types).

 - Heavy use of `typedef struct { ... } Name_St;`.

 - `const` correctness heavily enforced on parameters.

 - `void` for functions with no parameters.


### Typedef Suffix Rules

All typedef’d types use PascalCase. Suffix indicates kind/complexity:

 - **No suffix**: Base/primitive types that feel like built-ins, even if they are small structs (e.g. integer aliases, `f32Vector2`, `u8Vector2`, etc.). These live in `baseTypes.h` and are used everywhere as if they were primitives.

 - **_St**: Regular structs that represent game concepts or complex data (e.g. `Board_St`, `GameState_St`).

 - **_Et**: Enums (e.g. `BlockColor_Et`, `SceneState_Et`).

 - **Vec_St**: Dynamic array containers created with `typeDA` (e.g. `PrefabVec_St`, `PrefabIndexVec_St`). The `Vec_St` explicitly marks it as a growable vector while keeping the `_St` consistency.

 - **_Ft**: Function typedefs (general purpose, not callbacks) – reserved for future use.

 - **_Ct**: Callback function typedefs (event handlers, etc.) – reserved for future use.

 - **_t**: Fallback for any other typedef that doesn’t fit the above categories (rarely used).

This keeps the code instantly readable: you see `Vec_St` → it’s a dynamic array, you see plain `_St` → normal struct, no suffix → primitive/base type.

## **Formatting**

 - ### **Indentation**:

    4 spaces
 - ### **Braces**:

    - Control structures (if/while/for/switch) -> opening brace on new line.

        ```C
        while (!WindowShouldClose()) {
            ...
        }
        ```
    - Function definitions -> opening brace on new line.

    - Single-statement bodies often omit braces when safe, but multi-statement always use them.

    - Macros using `do { ... } while(0)` for multi-statement blocks.


 - ### **Spacing**:

    - Space after keywords (`if (...)`, `while (...)`, `for (...; ...; ...)`).

    - No space before `;` or `)`.

    - Space around binary operators (`= + - < > * / % | & ^`).

    - Pointer:  
    If `*` sticks to the variable type (`Type* var`)  , then the infered type by the programmer is **a pointer to the type**.  
    If `*` sticks to the variable name (`Type *array`), then the infered type by the programmer is **array of `Type`**.  
    **Note**: It doesn't change anything wherever it is, though it can be of help to infer what the programmer is working with rather than guessing based on the name of the variable.
    
    - const placement: `const Type* const ptr` pattern common.

    - **Line Length**: Generally kept reasonable (<100 chars); long lines avoided.

    - **Parameter Lists**: void func(Type param) – no trailing comma, void for empty params.


## Header Files & Includes

 - **Guards**: `#ifndef PATH_LIKE_NAME_H` / `#define` / `#endif // PATH_LIKE_NAME_H`.

Example: `#ifndef CORE_GAME_BOARD_H`.

 - **Self-containment**: Headers include what they need; forward declarations avoided.


## Comments & Documentation

 - **Function Documentation**: Doxygen-style `/** @brief ... */` blocks in headers, see [doxygen documetation](https://www.doxygen.nl/manual/docblocks.html).

 - **Inline Comments**: `//` for short notes, `/* ... */` for multi-line or disabled code.

 - **File Headers**: Include author, date, brief description.

 - **TODO/Warnings**: Plain comments, `TODO` macro or `log_warn` macro.


## Macros & Utilities

 - Heavy macro usage for:

    - `min`/`max`/`clamp`, `swaps`, vector ops.

    - Dynamic arrays (`da_append`, `da_free`, `typeDA`, etc.).

    - Shuffling, printing, defer-style returns (`return_defer`).

    - Debug helpers (`dcall`, `UNREACHABLE` implied).

    - logging (`log_info`, `log_warn`, `log_debug`, etc.)


 - All utility macros are in `utils.h`.


## Types & Declarations

 - Custom integer aliases in `baseTypes.h` (`u8`, `s8`, `u32`, etc.).

 - `bool8` for boolean (raylib compatibility).

 - Vector types: `u8Vector2`, `f32Vector2` (typdef'd for raylib compatibility), etc.

 - Heavy `typedef struct { ... } Name_St;`.

 - `const` correctness emphasized on parameters.

 - `void` for functions with no parameters.


## Error Handling & Safety

 - `UNREACHABLE` macro for impossible cases.

 - Defensive checks (bounds, nulls).

 - Logging via custom logger (`log_info`, `log_warn`, etc.).


## Memory Management

 - Arena-based allocation (`globalArena`, `tempArena`, `context_alloc`).

 - Dynamic arrays via custom macros (nob.h derived).
