#include "utils/globals.h"
#include "ui/shape.h"
#include "core/game/shape.h"
#include "setups/app.h"
#include "utils/userTypes.h"
#include "utils/utils.h"

Prefab_St bag[302] = {0};
u32 bagIdx = 0;

void printPrefabInfo(const Prefab_St prefab) {
    for (u8 i = 0; i < prefab.blockCount; ++i) {
        printf("(%d, %d) ", prefab.offsets[i].x, prefab.offsets[i].y);
    }

    f32Vector2 offsetCenter = getOffsetCenter(prefab);
    printf("| bc: %u | w: %u, h: %u | c: (%.1f, %.1f) | o: %d | canMirror: %s\n", 
        prefab.blockCount, prefab.width, prefab.height, offsetCenter.x, offsetCenter.y, prefab.orientations, boolStr(prefab.canMirror)
    );
}

bool8 haveSimilarOffsets(const Prefab_St prefab1, const Prefab_St prefab2) {
    bool8 hashmap[36] = {0};
    bool8 same = true;

    for (u8 i = 0; i < prefab1.blockCount; ++i) {
        u8 index = prefab1.offsets[i].x * 6 + prefab1.offsets[i].y;
        hashmap[index] = true;
    }

    for (u8 i = 0; i < prefab1.blockCount; ++i) {
        u8 index = prefab2.offsets[i].x * 6 + prefab2.offsets[i].y;
        same &= hashmap[index];
    }
    
    return same;
}

void initPrefab(Prefab_St* const prefab) {
    prefab->orientations = 0;
    setPrefabBoundingBox(prefab);

    Prefab_St prefabCmp = *prefab;
    Prefab_St prefabCmpMirror = *prefab;

    rotatePrefab(&prefabCmp, 1);

    mirrorPrefab(&prefabCmpMirror);
    prefab->canMirror = !haveSimilarOffsets(*prefab, prefabCmpMirror);
    
    while (!haveSimilarOffsets(*prefab, prefabCmp)) {
        prefab->orientations++;
        rotatePrefab(&prefabCmp, 1);
    }
    
    prefab->orientations += prefab->orientations > 0;

    if (!prefab->canMirror) return;

    bool8 foundDuplicate = false;
    for (u8 i = 0; i < prefab->orientations; ++i) {
        for (u8 j = 0; j < prefab->orientations; ++j) {
            if (haveSimilarOffsets(prefabCmp, prefabCmpMirror)) {
                foundDuplicate = true;
                break;
            }

            rotatePrefab(&prefabCmp, 1);
        }

        if (foundDuplicate) break;
        rotatePrefab(&prefabCmpMirror, 1);
    }
    
    if (foundDuplicate) {
        prefab->canMirror = false;
        // prefab->orientations /= 2;
    }
}

void addPrefab(Prefab_St prefab) {
    bag[bagIdx++] = prefab;

    for (u8 k = 1; k < prefab.orientations; ++k) {
        rotatePrefab(&prefab, 1);
        bag[bagIdx++] = prefab;
    }

    if (!prefab.canMirror) return;

    rotatePrefab(&prefab, 1);
    mirrorPrefab(&prefab);
    bag[bagIdx++] = prefab;

    for (u8 k = 1; k < prefab.orientations; ++k) {
        rotatePrefab(&prefab, 1);
        bag[bagIdx++] = prefab;
    }
}

int main(void) {
    initApp();

    for (u32 i = 0; i < prefabCount; ++i) {
        Prefab_St prefab = prefabs[i];
        
        if (prefab.orientations > -1) {
            setPrefabBoundingBox(&prefab);
        } else {
            initPrefab(&prefab);
        }

        addPrefab(prefab);
    }

    log_info("%u", bagIdx);

    // GameState_St game = {0};

    // for (u8 i = 0; i < 3; ++i) {
    //     ActivePrefab_St* const shape = &game.slots[i];
    //     shape->prefab = &bag[rand() % bagIdx];
    //     shape->pos = defaultPositions[i];
    //     shape->colorIndex = rand() % _blockColorCount;
    //     shape->placed = false;
    // }

    f32 offset = BLOCK_PX_SIZE * 8.0f;
    u8 prefabPerRow = (WINDOW_WIDTH - offset) / offset;
    u8 prefabPerCol = (WINDOW_HEIGHT - offset) / offset;

    log_info("%u, %u", prefabPerRow, prefabPerCol);

    ActivePrefab_St shapeBag[302] = {0};
    for (u32 i = 0; i < bagIdx; ++i) {
        shapeBag[i] = (ActivePrefab_St) {
            .prefab = &bag[i],
            .colorIndex = i % (_blockColorCount - 1),
            .pos = {
                .x = offset * (1 + i % prefabPerRow),
                .y = offset * (1 + i / prefabPerRow)
            }
        };
    }

    while (!WindowShouldClose()) {
        // if (IsKeyPressed(KEY_S)) {
        //     for (u8 i = 0; i < 3; ++i) {
        //         ActivePrefab_St* const shape = &game.slots[i];
        //         shape->prefab = &bag[rand() % bagIdx];
        //         shape->pos = defaultPositions[i];
        //         shape->colorIndex = rand() % _blockColorCount;
        //         shape->placed = false;
        //     }
        // }

        BeginDrawing(); {
            ClearBackground(GRAY);

            for (u32 i = 0; i < bagIdx; ++i) {
                drawShape(shapeBag[i]);
            }
            // drawSlots(game.slots);
        } EndDrawing();
    }

    freeApp();

    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"