/**
 * @file shape.h (core/game)
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Shape manipulation, placement, and interaction logic.
 */

#ifndef CORE_GAME_SHAPE_H
#define CORE_GAME_SHAPE_H

#include "utils/userTypes.h"

/**
 * @brief Checks if two prefabs have identical block layouts via offsets.
 *
 * Uses a small hashmap (36 bytes, for 6x6 grid) to mark positions from
 * the first prefab, then verifies all positions from the second are marked.
 *
 * @note The largest offsets is have a 6 either in the x or y (and 1 in the other component), 
 *       hence the 6x6 hashmap.
 * @note Assumes offsets are normalized.
 *
 * @param prefab1 First prefab.
 * @param prefab2 Second prefab.
 * @return true if offsets match, false otherwise.
 */
bool8 haveSimilarOffsets(const Prefab_St prefab1, const Prefab_St prefab2);

/**
 * @brief Checks if the mouse clicked on the shape.
 *
 * @param shape Pointer to the active shape.
 * @return true if clicked, false otherwise.
 */
bool8 isShapeClicked(const ActivePrefab_St* const shape);

/**
 * @brief Checks if the shape is within board bounds.
 *
 * @param shape Pointer to the active shape.
 * @return true if in bounds, false otherwise.
 */
bool8 isShapeInBound(const ActivePrefab_St* const shape);

/**
 * @brief Checks if the shape can be placed on the board without overlap.
 *
 * @param shape Pointer to the active shape.
 * @return true if placeable, false otherwise.
 */
bool8 isShapePlaceable(const ActivePrefab_St* const shape);

/**
 * @brief Sets the bounding box (width/height) for a prefab.
 *
 * @param prefab Pointer to the prefab.
 */
void setPrefabBoundingBox(Prefab_St* const prefab);


/**
 * @brief Gets the top-left corner position of the shape.
 *
 * @param shape Pointer to the active shape.
 * @return The top-left position vector.
 */
f32Vector2 getShapeTopLeftCorner(const ActivePrefab_St* const shape);

/**
 * @brief Gets the center position of the shape.
 *
 * @param shape The active shape.
 * @return The center position vector.
 */
f32Vector2 getShapeCenter(const ActivePrefab_St shape);

/**
 * @brief Gets the center of the prefab's offsets.
 *
 * @param prefab The prefab.
 * @return The offset center vector.
 */
f32Vector2 getOffsetCenter(const Prefab_St prefab);

/**
 * @brief Gets the position of the i-th block in the shape.
 *
 * @param shape The active shape.
 * @param i The block index.
 * @return The block position vector.
 */
f32Vector2 getIthBlockPosition(const ActivePrefab_St shape, const u8 i);

/**
 * @brief Maps the shape's center to board coordinates.
 *
 * @param shape Pointer to the active shape.
 * @return The mapped board position.
 */
s8Vector2 mapShapeToBoardPos(const ActivePrefab_St* const shape);


/**
 * @brief Adds a prefab and its rotated/mirrored variants to the bag.
 *
 * Generates variants only if orientations > 0 or canMirror is true, avoiding duplicates.
 * Each variant is a separate Prefab_St entry in the bag.
 *
 * @param prefab The base prefab.
 * @param prefabsBag Pointer to the prefab bag.
 */
void addPrefabAndVariants(Prefab_St prefab, PrefabBagVec_St* const prefabsBag);

/**
 * @brief Handles user interaction with a shape (dragging, release).
 *
 * Integrates with global dragging flag.
 *
 * @param shape Pointer to the active shape.
 */
void handleShape(ActivePrefab_St* const shape);

/**
 * @brief Refills all three player slots with new random shapes and resets their state.
 *
 * For each of the three slots:
 *   - Sets .id = slot index (0,1,2)
 *   - Calls randomizeShape() -> new prefab, color, position, etc.
 *
 * After filling all slots, calls refillShapeBags() to repopulate the size-grouped
 * index bags from the master prefabsBag (usually needed because randomizeShape
 * consumed indices).
 *
 * Typical usage:
 *   - Called when all three shapes have been placed (end of turn)
 *   - Called on shuffle key press (debug / cheat)
 *   - Called at game start
 *
 * @param game   Main game state
 */
void shuffleSlots(GameState_St* const game);

/**
 * @brief Places the shape on the board.
 *
 * @param shape Pointer to the active shape.
 * @param board Pointer to the board.
 */
void placeShape(const ActivePrefab_St* const shape, Board_St* const board);


/**
 * @brief Rotates the prefab's offsets by 90° × rotateBy times (clockwise).
 *
 * Rotation is done in-place on prefab->offsets[].
 * The function uses standard 2D rotation formulas:
 *   new_x =  old_y
 *   new_y = -old_x
 *
 * After rotation the bounding box (width/height) is **not** automatically updated —
 * call setPrefabBoundingBox() afterwards if needed.
 *
 * @note Does **not** check for duplicate orientations — that's done during initPrefab().
 *
 * @param prefab    Prefab to modify (offsets are mutated)
 * @param rotateBy  Number of 90° clockwise rotations (0 = no-op, negative = counterclockwise)
 */
void rotatePrefab(Prefab_St* const prefab, u8 rotateBy);

/**
 * @brief Mirrors the prefab horizontally.
 *
 * @param prefab Pointer to the prefab.
 */
void mirrorPrefab(Prefab_St* const prefab);

/**
 * @brief Releases a dragged shape, placing it if valid or resetting position.
 *
 * @param shape Pointer to the active shape.
 * @param board Pointer to the board.
 */
void releaseShape(ActivePrefab_St* const shape, Board_St* const board);

/**
 * @brief Prints debug information about a prefab (block count, dimensions, etc.).
 *
 * @param prefab The prefab to print.
 */
void printPrefabInfo(const Prefab_St prefab);

#endif // CORE_GAME_SHAPE_H