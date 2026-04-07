/**
    @file save.h
    @author Fshimi Hawlk
    @date 2026-02-27
    @date 2026-04-07
    @brief Game state serialization, file I/O, and save-list management.

    @note Saves are stored in "./assets/saves/" (created automatically).
          Each file is a binary blob with magic/version + minimal metadata for fast listing.
*/
#ifndef SAVE_H
#define SAVE_H

#include "utils/userTypes.h"

u64 getSerializedGameStateSize(const GameState_St* const state);

/**
    @brief Serializes the game state to a binary buffer.

    Writes only the essential data needed for reconstruction:
    - Scoring: score, streakCount, streakGrace
    - Board: width, height, blocks (hitsLeft + colorIndex per cell)
    - Prefab manager: prefabVariant, runTimeWeights, per-size bags (shuffled prefabs), slots (placed, colorIndex, linked via bag size + index)
    - Scene state

    Other fields (e.g., board.pos, scoreText, prefab pointers, prefabsBag) are regenerated on deserialization.

    @param state  Pointer to the game state to serialize.
    @param buffer Output buffer (must be pre-allocated if not NULL). Pass NULL to compute required size only.
    @return       Size of the serialized data (in bytes). Returns 0 on error (e.g., invalid state).
*/
u64 serializeGameState(const GameState_St* const state, u8* buffer, const u64 bufferSize);

/**
    @brief Deserializes a binary buffer into a game state.

    Reconstructs the state by:
    1. Reading essentials (score, board, weights, etc.)
    2. Rebuilding prefabsBag via initPrefabsAndVariants (using loaded prefabVariant)
    3. Loading shuffled bags directly (overriding the default init)
    4. Relinking slot prefabs to the new bag items via saved size + bagIndex
    5. Regenerating UI texts, board position, gameOver flag, etc.

    Assumes the board is 8x8 (ignores loaded width/height if mismatched, but logs warning).
    Clears any existing dynamic arrays in state before loading.

    @param state  Pointer to the game state to fill (will be zeroed first).
    @param buffer Input buffer with serialized data.
    @param size   Size of the input buffer.
    @return       true if deserialization succeeded, false on error (e.g., invalid magic/version, buffer too small).
*/
bool deserializeGameState(GameState_St* const state, const u8* buffer, const u64 bufferSize, bool init);

bool saveGameToFile(const GameState_St* const state, const char* filename);

#endif // SAVE_H