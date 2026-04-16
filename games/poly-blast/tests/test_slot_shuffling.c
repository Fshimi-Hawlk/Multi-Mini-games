/**
    @file test_slot_shuffling.c
    @author Fshimi-Hawlk
    @date 2026-04-10
    @brief Unit tests for active slots and shuffling.
*/

#include "setups/game.h"
#include "setups/shape.h"

#include "core/shape.h"

#include "utils/globals.h"

#include "utils/random.h"

#include <assert.h>
#include <stdlib.h>

static void test_shuffle_slots(void) {
    GameState_St testGame = {0};
    initPrefabsAndVariants(&prefabsBag, GAME_PREFAB_VARIANT_DEFAULT);
    initPrefabManager(&testGame.prefabManager);

    shuffleSlots(&testGame.prefabManager);

    for (u8 i = 0; i < 3; ++i) {
        assert(testGame.prefabManager.slots[i].prefab != NULL);
        log_info("OK");
        assert(testGame.prefabManager.slots[i].placed == false);
        log_info("OK");
    }
}

static void test_shuffle_bag(void) {
    PrefabIndexBagVec_St testBag = {0};
    for (u32 i = 0; i < 10; ++i) {
        da_append(&testBag, i);
    }

    da_shuffleT(u32, &testBag, rand);
    // Statistical: check not sorted (probabilistic)
    bool sorted = true;
    for (u32 i = 1; i < testBag.count; ++i) {
        if (testBag.items[i-1] > testBag.items[i]) sorted = false;
    }
    assert(sorted == false);
    log_info("OK");
}

int main(void) {
    test_shuffle_slots();
    test_shuffle_bag();
    log_info("Slots shuffling tests passed");
    return 0;
}