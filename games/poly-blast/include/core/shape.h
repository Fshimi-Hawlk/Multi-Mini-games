/**
    @file shape.h
    @author Kimi BERGE
    @date 2026-01-07
    @date 2026-04-14
    @brief Shape manipulation and interaction logic.
*/
#ifndef CORE_SHAPE_H
#define CORE_SHAPE_H

#include "utils/userTypes.h"

/**
    @brief Checks if the mouse clicked on the shape.

    @param[in]     shape        Pointer to the active shape.
    @return                     true if clicked, false otherwise.
*/
bool polyBlast_isShapeClicked(const Shape_St* const shape);

/**
    @brief Checks if the shape is within board bounds.

    @param[in]     shape        Pointer to the active shape.
    @param[in]     board        Pointer to the board.
    @return                     true if in bounds, false otherwise.
*/
bool polyBlast_isShapeInBound(const Shape_St* const shape, const Board_St* const board);

/**
    @brief Gets the center position of the shape.

    @param[in]     shape        The active shape.
    @return                     The center position vector.
*/
f32Vector2 polyBlast_getShapeCenter(const Shape_St shape);

/**
    @brief Gets the position of the i-th block in the shape.

    @param[in]     shape        The active shape.
    @param[in]     i            The block index.
    @return                     The block position vector.
*/
f32Vector2 polyBlast_getIthBlockPosition(const Shape_St shape, const u8 i);

/**
    @brief Maps the shape's center to board coordinates.

    @param[in]     shape        Pointer to the active shape.
    @param[in]     board        Pointer to the board.
    @return                     The mapped board position.
*/
s8Vector2 polyBlast_mapShapeToBoardPos(const Shape_St* const shape, const Board_St* const board);

/**
    @brief Handles user interaction with a shape (dragging, release).

    Integrates with global dragging flag.

    @param[in,out] game         Pointer to the current game state.
    @param[in,out] shape        Pointer to the active shape.
*/
void polyBlast_handleShape(PolyBlastGame_St* const game, Shape_St* const shape);

/**
    @brief Refills all three player slots with new random shapes and resets their state.

    @param[in,out] manager      Main game's prefab manager to update.
*/
void polyBlast_shuffleSlots(PrefabManager_St* const manager);

/**
    @brief Prints a graphical ASCII representation of a prefab shape to the terminal.

    @param[in]     shape        Pointer to the active shape.
*/
void polyBlast_displayShape(const Shape_St* const shape);

/**
    @brief Prints all current slots graphically to the terminal.

    @param[in]     slots        The three active prefab slots.
*/
void polyBlast_printSlotsGraphically(const ShapeSlots_t slots);

#endif // CORE_SHAPE_H