/**
    @file shape.h (setups)
    @author Fshimi Hawlk
    @date 2026-01-07
    @brief Prefab initialization and variant handling.
*/

#ifndef SETUPS_SHAPE_H
#define SETUPS_SHAPE_H

#include "utils/userTypes.h"

/**
    @brief Initializes the prefab bag with all prefabs and their variants.

    Loops over base prefabs (up to prefabCount or _prefabNameCount based on variant),
    initializes each, then adds rotations and mirrors if applicable.

    @param prefabsBag Pointer to the bag of prefabs.
    @param variant    Game's prefab variant.
*/
void polyBlast_initPrefabsAndVariants(PrefabBagVec_St* const prefabsBag, GamePrefabVariant_Et variant);

#endif // SETUPS_SHAPE_H