# **Code Style & Conventions**

## **Naming Conventions**

 - **Functions & Variables**: camelCase, starting with lowercase.  
 - **Examples**: buildScoreText, calculateScore, isShapeClicked, windowRect, mouseDeltaFromShapeCenter.  
 - **Types**:  
    - Structs → PascalCase + `_St` suffix (e.g., `Board_St`, `GameState_St`, `ActivePrefab_St`).
    - Enums → PascalCase + `_Et` suffix (e.g., `BlockColor_Et`, `SceneState_Et`).
    - Typedef’d dynamic arrays → PascalCase + `_St` (e.g., `PrefabBag_St`).

 - **Enum Members**: ALL_CAPS with underscores (e.g., `BLOCK_COLOR_RED`, `SCENE_STATE_GAME`).  
 - **Macros**: ALL_CAPS with underscores or snake_case (e.g., `INVALID_PTR`, `ARRAY_LEN`, da_append).  
 - **Globals**: camelCase, same as local variables (e.g., `windowRect`, `appFont`, `game`).  
 - **Constants**: Either ALL_CAPS macros or stored in const arrays, camelCase (e.g., `blockColors`, `defaultPositions`, `WINDOW_TITLE`).  

No Hungarian notation except the `_St`/`_Et` suffixes for types.

## **Formatting**

 - ### **Indentation**:
    4 spaces
 - ### **Braces**:  
    - Control structures (if/while/for/switch) → opening brace on new line.
        ```C
        while (!WindowShouldClose()) {
            ...
        }
        ```
    - Function definitions → opening brace on new line.  
    - Single-statement bodies often omit braces when safe, but multi-statement always use them.  
    - Macros using `do { ... } while(0)` for multi-statement blocks.  

 - ### **Spacing**:
    - Space after keywords (`if (`, `while (`, `for ()`).
    - No space before `;` or `)`.
    - Space around binary operators (`= + - < > * / % | & ^`).
    - Pointer: `*` sticks to the type (`Type* var`) the type infered for the programmers is **a pointer to the type**. In contrary, if sticks to the variable name, then the infered type for the programmer is an **array of `Type`** (`Type *array`).  
    **Note**: At compilation it doesn't change anything, it's just a little convention to know what we're working with.
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