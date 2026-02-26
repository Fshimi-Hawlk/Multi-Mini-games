#ifndef CORE_PLACEMENT_H
#define CORE_PLACEMENT_H

#include "utils/userTypes.h"

/**
    @brief Checks if the shape can be placed on the board without overlap.

    @param shape Pointer to the active shape.
    @param pos Position, on the board, of the shape to be placed.
    @return true if placeable, false otherwise.
*/
bool isShapePlaceable(const Shape_St *const shape, const s8Vector2 pos, const Board_St* const board);

/**
    @brief Places the shape on the board.

    @param shape Pointer to the active shape.
    @param pos   Top left position of the shape.
    @param board Pointer to the board.
*/
void placeShape(const Shape_St* const shape, const u8Vector2 pos, Board_St* const board);

bool canPlaceAll(Board_St* board, const ShapeSlots_t slots, const u8 order[3], u8 idx);

/**
    @brief Runs a brute-force simulation to pick the "best" set of three prefabs
           for the next turn based on simulated score after placement and clears.

    Performs up to 8 full simulation attempts:
      - Each attempt draws a fresh shuffled set of slots into a temporary game copy.
      - For each of the 6 placement permutations, simulates placing all three shapes
        while tracking the produced score.
      - Remembers the prefab combination that produced the highest recorded score
        (base score + large streak bonus).

    After all attempts the best three prefabs (in random order) are written back
    into the real `game->prefabManager.slots`.

    Temporarily redirects `contextArena` to `tempArena` and resets it when finished.

    @param[in,out] game  Game state whose slots will be replaced by the best
                         simulated set. Board and current score are not modified.
*/
void placementSimulation(GameState_St* const game);


/**
    @brief Releases a shape at a precise position, placing it if valid or resetting position.

    @param shape Pointer to the active shape.
    @param pos   Position on the board
    @param board Pointer to the board.
*/
void releaseShapeAt(Shape_St *const shape, s8Vector2 pos, Board_St *const board);

/**
    @brief Releases a dragged shape, placing it if valid or resetting position.

    @param shape Pointer to the active shape.
    @param board Pointer to the board.
*/
void releaseShape(Shape_St* const shape, Board_St* const board);


#endif // CORE_PLACEMENT_H