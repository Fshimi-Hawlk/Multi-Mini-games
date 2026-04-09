/**
    @file generalAPI.h
    @author Fshimi Hawlk
    @author i-Charlys (CAILLON Charles)
    @author Maxime-CHAUVEAU
    @date 2026-01-08
    @date 2026-04-09
    @brief Shared core types and error codes used across all games and the lobby.
           Standardized interface for Client-side mini-games (Raylib Frontend).

    This header defines:
        - a standard set of error codes used across game subsystems
        - the common base structure that every mini-game state must embed
        - a function pointer type for game cleanup

    Design goals:
        - Allow the lobby to interact with any mini-game in a type-safe, generic way
        - Enable uniform error handling across different game implementations
        - Keep the common interface as small as possible (only what truly belongs to every game)

    Important rules for mini-game implementations:
        - The concrete game state struct **must** have `BaseGame_St base;` as its **first member**
        - This guarantees that `BaseGame_St* base = (BaseGame_St*)specificGame;` is always valid
        - The `freeGame` callback must free the entire game object (including the specific part)

    Typical usage in the lobby:
        - `BaseGame_St* current = miniGames[scene];`
        - `if (current && current->running) { ... }`
        - On exit: `if (current && current->freeGame) current->freeGame(current);`
 */

#ifndef GENERAL_API_H
#define GENERAL_API_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/**
    @brief Standardized error codes for initialization, operations and resource loading.

    Convention:
        - OK = 0 means success
        - All other values indicate a specific failure reason

    Mini-games and subsystems should return one of these whenever possible.
    More specific codes can be added later, but prefer generic ones when reasonable.
 */
typedef enum {
    OK = 0,                     ///< Operation succeeded
    ERROR_NULL_POINTER,         ///< Required pointer argument was NULL
    ERROR_ALLOC,                ///< Memory allocation failed
    ERROR_INVALID,              ///< Argument value or state is invalid/illegal
    ERROR_INVALID_ENUM_VAL,     ///< Enum value is invalid
    ERROR_INVALID_SETTING,      ///< Setting parameters is invalid
    ERROR_WINDOW_INIT,          ///< Failed to initialize window/display
    ERROR_ASSET_LOAD,           ///< Failed to load any kind of asset
    ERROR_TEXTURE_LOAD,         ///< Failed to load a texture
    ERROR_AUDIO_LOAD,           ///< Failed to load an audio
    ERROR_FONT_LOAD,            ///< Failed to load a font
    // Future extension point: add more codes here as needed
} Error_Et;

// ────────────────────────────────────────────────
// Game interface
// ────────────────────────────────────────────────

/**
    @brief Available font sizes used for in-game UI and text rendering.

    Values are listed in ascending order.  
    `_fontSizeCount` is **not** a valid font size - it serves as array dimension / loop boundary.
*/
typedef enum {
    FONT16, FONT24, FONT32, 
    FONT48, FONT64, FONT96, 
    FONT128,
    __fontSizeCount
} FontSize_Et;


/**
    @brief Identifiers of the different playable scenes / mini-games.

    Used both as array indices and as state identifiers.
*/
typedef enum {
    MINI_GAME_ID_LOBBY,       ///< Main lobby / hub world with platformer movement
    MINI_GAME_ID_TETRIS,
    MINI_GAME_ID_SOLITAIRE,
    MINI_GAME_ID_SUIKA,
    MINI_GAME_ID_BOWLING,
    MINI_GAME_ID_GOLF,
    MINI_GAME_ID_SNAKE,
    MINI_GAME_ID_BINGO,
    MINI_GAME_ID_BLOCKBLAST,
    MINI_GAME_ID_BATTLESHIP,
    MINI_GAME_ID_CONNECT_4,
    MINI_GAME_ID_KFF,
    MINI_GAME_ID_MINIGOLF,
    MINI_GAME_ID_MORPION,
    MINI_GAME_ID_OTHELLO,
    __miniGameIdCount
} MiniGameId_Et;

/**
    @brief Opaque forward declaration of the concrete game state.

    Actual definition lives in the specific mini-game header (.e.g. tetrisGame.h).
 */
typedef struct BaseGame_St BaseGame_St;

/**
    @brief Function pointer type for game cleanup.

    Must free the entire game object (including any internal allocations)
    and return an Error_Et code (usually OK).

    Signature: `Error_Et freeMyGame(void* game);`
 */
typedef Error_Et (*FreeGame_Ft)(void*);

/**
    @brief Common base structure that **every** mini-game state must embed as its first member.

    Layout guarantee:
      struct MyGame {
          BaseGame_St base;
          // MyGame-specific fields...
      };

    This allows safe downcasting from `BaseGame_St*` to concrete type only when needed,
    while the lobby can safely access `running`, `paused`, `score`, and call `freeGame`.
 */
struct BaseGame_St {
    bool        running;        ///< true = game should continue updating/rendering
    bool        paused;         ///< true = game is paused (no update, optional dimmed render)
    long        score;          ///< Score accumulated during the current/last session

    FreeGame_Ft freeGame;       ///< Cleanup callback (must free the whole object)
};

#endif // GENERAL_API_H
