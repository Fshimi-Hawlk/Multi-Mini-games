/**
 * @file shape.h (setups)
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Prefab initialization and variant handling.
 */

#ifndef SETUPS_SHAPE_H
#define SETUPS_SHAPE_H

#include "utils/userTypes.h"

/**
 * @brief Initializes the prefab bag with all prefabs and their variants.
 *
 * Loops over base prefabs (up to prefabCount or _prefabNameCount based on variant),
 * initializes each, then adds rotations and mirrors if applicable.
 *
 * @param game Pointer to the game's prefab manager.
 */
void initPrefabsAndVariants(PrefabManager_St* const manager);

/**
 * @brief Initializes a single prefab, computing orientations and mirroring.
 *
 * Rotates and compares offsets to find unique orientations. Checks for mirror duplicates.
 * Updates prefab->orientations and ->canMirror accordingly.
 *
 * @param prefab Pointer to the prefab to initialize.
 */
void initPrefab(Prefab_St* const prefab);

#endif // SETUPS_SHAPE_H