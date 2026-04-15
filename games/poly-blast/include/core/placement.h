/**
    @file placement.h
    @author Kimi BERGE
    @date 2026-01-07
    @date 2026-04-14
    @brief Shape placement and simulation logic.
*/
#ifndef CORE_PLACEMENT_H
#define CORE_PLACEMENT_H

#include "utils/userTypes.h"

/**
    @brief Checks if the shape can be placed on the board without overlap.

    @param[in]     shape        Pointer to the active shape.
    @param[in]     pos          Position, on the board, of the shape to be placed.
    @param[in]     board        Pointer to the board.
    @return                     true if placeable, false otherwise.
*/
bool polyBlast_isShapePlaceable(const Shape_St *const shape, const s8Vector2 pos, const Board_St* const board);

/**
    @brief Places the shape on the board.

    @param[in]     shape        Pointer to the active shape.
    @param[in]     pos          Top-left position of the shape.
    @param[in,out] board        Pointer to the board.
*/
void polyBlast_placeShape(const Shape_St* const shape, const u8Vector2 pos, Board_St* const board);

/**
    @brief Recursively checks if all shapes in slots can be placed in a specific order.

    @param[in,out] board        Pointer to the board (simulated state).
    @param[in]     slots        The three active prefab slots.
    @param[in]     order        Array of indices representing placement order.
    @param[in]     idx          Current index in the order array.
    @return                     true if all remaining shapes can be placed.
*/
bool polyBlast_canPlaceAll(Board_St* board, const ShapeSlots_t slots, const u8 order[3], u8 idx);

/**
    @brief Runs a brute-force simulation to pick the "best" set of three prefabs
           for the next turn based on simulated score after placement and clears.

    Performs up to 8 full simulation attempts:
      - Each attempt draws a fresh shuffled set of slots into a temporary game copy.
      - For each of the 6 placement permutations, simulates placing all three shapes
        while tracking the produced score.
      - Remembers the prefab combination that produced the highest recorded score.

    @param[in,out] game         Game state whose slots will be replaced by the best
                                simulated set.
*/
void polyBlast_placementSimulation(PolyBlastGame_St* const game);

/**
    @brief Releases a shape at a precise position, placing it if valid or resetting position.

    @param[in,out] shape        Pointer to the active shape.
    @param[in]     pos          Position on the board.
    @param[in,out] board        Pointer to the board.
*/
void polyBlast_releaseShapeAt(Shape_St *const shape, s8Vector2 pos, Board_St *const board);

/**
    @brief Releases a dragged shape, placing it if valid or resetting position.

    @param[in,out] shape        Pointer to the active shape.
    @param[in,out] board        Pointer to the board.
*/
void polyBlast_releaseShape(Shape_St* const shape, Board_St* const board);

#endif // CORE_PLACEMENT_H