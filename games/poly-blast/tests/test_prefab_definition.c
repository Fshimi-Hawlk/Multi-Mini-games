/**
    @file test_prefab_definition.c
    @author Fshimi-Hawlk
    @date 2026-04-10
    @brief Unit tests for prefab shapes and variants.
*/

#include "core/prefab.h"
#include "setups/shape.h"
#include "utils/globals.h"

#include <assert.h>

static void test_init_prefab(void) {
    Prefab_St testPrefab = prefabs[PREFAB_L4];
    initPrefab(&testPrefab);
    assert(testPrefab.orientations > 0);
    log_info("OK");

    assert(testPrefab.canMirror == true); // Valid
    log_info("OK");
}

static void test_rotate_prefab(void) {
    Prefab_St testPrefab = prefabs[PREFAB_1x2];
    rotatePrefab(&testPrefab, 1); // To horizontal
    assert(testPrefab.offsets[1].x == 1 && testPrefab.offsets[1].y == 0);
    log_info("OK");
}

static void test_mirror_prefab(void) {
    Prefab_St testPrefab = prefabs[PREFAB_L4];
    mirrorPrefab(&testPrefab);
    // Check flipped
    assert(testPrefab.offsets[1].x == 0); // Assuming normalization
    log_info("OK");
}

static void test_have_similar_offsets(void) {
    Prefab_St p1 = {.blockCount = 2, .offsets = {{0,0}, {1,0}}};
    Prefab_St p2 = p1;
    assert(haveSimilarOffsets(p1, p2) == true);
    log_info("OK");

    p2.offsets[1].x = 0; p2.offsets[1].y = 1;
    assert(haveSimilarOffsets(p1, p2) == false);
    log_info("OK");
}

static void test_init_variants(void) {
    PrefabBagVec_St testManagerBag = {0}; // renamed to avoid confusion with PrefabManager_St
    initPrefabsAndVariants(&testManagerBag, GAME_PREFAB_VARIANT_DEFAULT);
    assert(testManagerBag.count > 0);
    log_info("OK");

    // Check duplicates avoided
    for (u32 i = 0; i < testManagerBag.count; ++i) {
        for (u32 j = i+1; j < testManagerBag.count; ++j) {
            assert(
                !haveSimilarOffsets(testManagerBag.items[i], testManagerBag.items[j]) 
               || testManagerBag.items[i].orientations != testManagerBag.items[j].orientations
            );
        }
    }
    log_info("OK");
}

int main(void) {
    test_init_prefab();
    test_rotate_prefab();
    test_mirror_prefab();
    test_have_similar_offsets();
    test_init_variants();
    log_info("Prefab definition tests passed");
    return 0;
}