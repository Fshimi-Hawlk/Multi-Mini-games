/**
 * @file shape.h (ui)
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Shape/slot rendering functions.
 */

#ifndef UI_SHAPE_H
#define UI_SHAPE_H

#include "utils/userTypes.h"

/**
 * @brief Draws a single active prefab shape.
 *
 * @param shape The active prefab shape to draw.
 */
void drawShape(const ActivePrefab_St shape);

/**
 * @brief Draws all prefab slots, skipping placed ones.
 *
 * @param slots Array of prefab slots.
 */
void drawSlots(const PrefabSlots_t slots);

#endif // UI_SHAPE_H