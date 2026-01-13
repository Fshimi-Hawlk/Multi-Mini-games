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
 * @param prefabBag Pointer to the prefab bag dynamic array.
 */
void initPrefabsAndVariants(PrefabBag_St* const prefabBag);

/**
 * @brief Initializes a single prefab, computing orientations and mirroring.
 *
 * @param prefab Pointer to the prefab to initialize.
 */
void initPrefab(Prefab_St* const prefab);

#endif // SETUPS_SHAPE_H