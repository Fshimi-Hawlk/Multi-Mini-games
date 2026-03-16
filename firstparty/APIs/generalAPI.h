/**
    @file generalAPI.h
    @author Fshimi Hawlk
    @author i-Charlys (CAILLON Charles)
    @author Maxime-CHAUVEAU
    @date 2026-01-08
    @date 2026-03-23
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
    ERROR_INVALID_SETTING,      ///< Setting parameters is invalid
    ERROR_TEXTURE_LOAD,         ///< Failed to load a required texture
    ERROR_AUDIO_LOAD,           ///< Failed to load audio resource
    ERROR_WINDOW_INIT,          ///< Failed to initialize window/display
    ERROR_ASSET_LOAD,           ///< Failed to load game assets
    // Future extension point: add more codes here as needed
} Error_Et;

// ────────────────────────────────────────────────
// Game interface
// ────────────────────────────────────────────────

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
typedef Error_Et (*freeGame_Ft)(void*);


/**
    @brief Available font sizes used for in-game UI and text rendering.

    Values are listed in ascending order.  
    `_fontSizeCount` is **not** a valid font size - it serves as array dimension / loop boundary.
*/
typedef enum {
                      FONT4,   FONT6,   FONT8,
    FONT10,  FONT12,  FONT14,  FONT16,  FONT18,
    FONT20,  FONT22,  FONT24,  FONT26,  FONT28,
    FONT30,  FONT32,  FONT34,  FONT36,  FONT38,
    FONT40,  FONT42,  FONT44,  FONT46,  FONT48,
    FONT50,  FONT52,  FONT54,  FONT56,  FONT58,
    FONT60,  FONT62,  FONT64,  FONT66,  FONT68,
    FONT70,  FONT72,  FONT74,  FONT76,  FONT78,
    FONT80,  FONT82,  FONT84,  FONT86,  FONT88,
    FONT90,  FONT92,  FONT94,  FONT96,  FONT98,
    FONT100, FONT102, FONT104, FONT106, FONT108,
    FONT110, FONT112, FONT114, FONT116, FONT118,
    FONT120, FONT122, FONT124, FONT126, FONT128,
    __fontSizeCount
} FontSize_Et;

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

    freeGame_Ft freeGame;       ///< Cleanup callback (must free the whole object)
};

/**
 * @struct GameTLVHeader
 * @brief Internal network structure (Payload).
 * Memory padding is disabled to match network bytes exactly (1+1+2 = 4 bytes).
 */
#pragma pack(push, 1)
typedef struct {
    uint8_t game_id;     /**< ID of the targeted mini-game (0 for Lobby). */
    uint8_t action;      /**< Game-specific action (e.g., 2 for LOBBY_MOVE). */
    uint16_t length;     /**< Size of the data following this header. */
} GameTLVHeader;
#pragma pack(pop)

/**
 * @struct MiniGameModule
 * @brief Interface contract that each client mini-game must respect.
 */
typedef struct MiniGameModule {
    uint8_t id;          /**< Network identifier of the game. */
    const char* name;    /**< Display name of the module. */
    
    /**
     * @brief Allocation and loading of textures/models (Raylib).
     */
    void (*init)(void);
    
    /**
     * @brief Reception of a routed network packet for this game.
     * @param player_id ID of the player who sent the data.
     * @param action Action code.
     * @param data Pointer to the received data.
     * @param len Length of the data.
     */
    void (*on_data)(int player_id, uint8_t action, void* data, uint16_t len);
    
    /**
     * @brief Update of local physical logic (Frame by Frame).
     * @param dt Delta time since last frame.
     */
    void (*update)(float dt);
    
    /**
     * @brief Graphical rendering to the screen (Raylib).
     */
    void (*draw)(void);
} MiniGameModule;

#endif // GENERAL_API_H
