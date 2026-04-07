/**
    @file core/shape.h
    @author Fshimi Hawlk
    @date 2026-01-07
    @date 2026-02-25
    @brief Shape manipulation, and interaction logic.
*/

#ifndef CORE_SHAPE_H
#define CORE_SHAPE_H

#include "utils/userTypes.h"

/**
    @brief Checks if the mouse clicked on the shape.

    @param shape Pointer to the active shape.
    @return true if clicked, false otherwise.
*/
bool isShapeClicked(const Shape_St* const shape);

/**
    @brief Checks if the shape is within board bounds.

    @param board Pointer to the board.
    @param shape Pointer to the active shape.
    @return true if in bounds, false otherwise.
*/
bool isShapeInBound(const Shape_St* const shape, const Board_St* const board);

/**
    @brief Gets the center position of the shape.

    @param shape The active shape.
    @return The center position vector.
*/
f32Vector2 getShapeCenter(const Shape_St shape);

/**
    @brief Gets the position of the i-th block in the shape.

    @param shape The active shape.
    @param i The block index.
    @return The block position vector.
*/
f32Vector2 getIthBlockPosition(const Shape_St shape, const u8 i);

/**
    @brief Maps the shape's center to board coordinates.

    @param shape Pointer to the active shape.
    @param board Pointer to the board.
    @return The mapped board position.
*/
s8Vector2 mapShapeToBoardPos(const Shape_St* const shape, const Board_St* const board);

/**
    @brief Handles user interaction with a shape (dragging, release).

    Integrates with global dragging flag.

    @param shape Pointer to the active shape.
*/
void handleShape(GameState_St* const game, Shape_St* const shape);

/**
    @brief Refills all three player slots with new random shapes and resets their state.

    For each of the three slots:
      - Sets .id = slot index (0,1,2)
      - Calls randomizeShape() -> new prefab, color, position, etc.

    After filling all slots, calls refillShapeBags() to repopulate the size-grouped
    index bags from the master prefabsBag (usually needed because randomizeShape
    consumed indices).

    Typical usage:
      - Called when all three shapes have been placed (end of turn)
      - Called on shuffle key press (debug / cheat)
      - Called at game start

    @param manager   Main game's prefabs manager
*/
void shuffleSlots(PrefabManager_St* const manager);

/**
    @brief Prints a graphical ASCII representation of a prefab shape to the terminal.

    Uses ANSI true-color escape codes + Unicode full blocks "██" for occupied cells.
    Empty cells inside the bounding box are rendered as two spaces (so the shape stays aligned).
    Offsets are automatically normalized (works even if a prefab isn't shifted to 0,0).

    Add this to shape.c (or utils.c) and declare in the corresponding header.
*/
void displayShape(const Shape_St* const shape);

void printSlotsGraphically(const ShapeSlots_t slots);

#endif // CORE_GAME_SHAPE_H