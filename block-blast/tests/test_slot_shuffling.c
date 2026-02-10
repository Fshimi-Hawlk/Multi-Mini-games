/**
 * @file test_slots_shuffling.c
 * @brief Unit tests for active slots and shuffling.
 */

#include "setups/shape.h"
#include "core/game/shape.h"
#include "utils/utils.h"

#include <assert.h>

static void test_shuffle_slots(void) {
    GameState_St testGame = {0};
    initPrefabsAndVariants(&testGame.prefabManager);
    da_reserve(&testGame.prefabManager.prefabsBag, 200);
    initPrefabsAndVariants(&testGame.prefabManager);

    for (u32 i = 0; i < testGame.prefabManager.prefabsBag.count; ++i) {
        u8 size_idx = testGame.prefabManager.prefabsBag.items[i].blockCount - 1;
        da_append(&testGame.prefabManager.bags[size_idx], i);
    }

    for (u8 s = 0; s < MAX_SHAPE_SIZE; ++s) {
        PrefabIndexBagVec_St* bag = &testGame.prefabManager.bags[s];
        if (bag->count == 0) continue;
        da_shuffle(bag);
    }

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

    da_shuffle(&testBag);
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

#define LOGGER_IMPLEMENTATION
#include "logger.h"

#define CONTEXT_ARENA_IMPLEMENTATION
#include "contextArena.h"

#define RAND_IMPLEMENTATION
#include "rand.h"