#include <assert.h>

#include "utils/userTypes.h"

extern GameState_St game;
GameState_St game = {0}; 

#include "core/game/shape.h"
#include "core/game/shape.c"

s32 main(void) {
    printf("Running tests for core/game/shape...\n");

    // Setup mocks
    game.board.width = 8;
    game.board.height = 8;

    // Test haveSimilarOffsets
    {
        Prefab_St p1 = {0};
        p1.blockCount = 2;
        p1.offsets[0] = (u8Vector2) {0, 0};
        p1.offsets[1] = (u8Vector2) {1, 0};

        Prefab_St p2 = p1;  // Identical
        assert(haveSimilarOffsets(p1, p2) == true && "Identical offsets should match");

        p2.offsets[1] = (u8Vector2) {0, 1};  // Different
        assert(haveSimilarOffsets(p1, p2) == false && "Different offsets should not match");
    }

    // Test isShapeClicked (requires mouse mock; skip or mock GetMousePosition if Raylib included, but it's hard—note as integration test)
    // Skipped: Depends on Raylib's GetMousePosition().

    // Test isShapeInBound
    {
        Prefab_St prefab = {
            .blockCount = 1,
            .offsets = {{0, 0}}
        };

        ActivePrefab_St shape = {0};
        shape.prefab = &prefab;
        shape.center = (f32Vector2) {BLOCK_PX_SIZE * 4, BLOCK_PX_SIZE * 4};  // Center in board

        assert(isShapeInBound(&shape) == true && "Shape in bound should return true");

        shape.center.x = -BLOCK_PX_SIZE;  // Out of bound
        assert(isShapeInBound(&shape) == false && "Shape out of bound should return false");
    }

    // Test isShapePlaceable (depends on board state)
    {
        Prefab_St prefab = {
            .blockCount = 1,
            .offsets = {{0, 0}}
        };
        
        ActivePrefab_St shape = {0};
        shape.prefab = &prefab;

        game.board.blocks[0][0].hitsLeft = 0;  // Empty
        assert(isShapePlaceable(&shape, (s8Vector2) {0, 0}) == true && "Placeable on empty tile");

        game.board.blocks[0][0].hitsLeft = 1;  // Occupied
        assert(isShapePlaceable(&shape, (s8Vector2) {0, 0}) == false && "Not placeable on occupied tile");
    }

    // Test setPrefabBoundingBox
    {
        Prefab_St prefab = {
            .blockCount = 2,
            .offsets = {
                {0, 0}, {1, 2}
            }
        };

        setPrefabBoundingBox(&prefab);
        assert(prefab.width == 2 && prefab.height == 3 && "Bounding box should be 2x3");
    }

    // Test getShapeTopLeftCorner
    {
        Prefab_St prefab = {
            .width = 2,
            .height = 3
        };

        ActivePrefab_St shape = {0};
        shape.prefab = &prefab;
        shape.center = (f32Vector2){100, 100};

        f32Vector2 corner = getShapeTopLeftCorner(&shape);
        assert(corner.x == 100 - BLOCK_PX_SIZE * (2 - 1) / 2.0f && "Top-left x calculation");
        assert(corner.y == 100 - BLOCK_PX_SIZE * (3 - 1) / 2.0f && "Top-left y calculation");
    }

    // Test getShapeCenter
    {
        Prefab_St prefab = {
            .width = 2,
            .height = 3
        };
        
        ActivePrefab_St shape = {0};
        shape.prefab = &prefab;
        shape.center = (f32Vector2){50, 50};

        f32Vector2 center = getShapeCenter(shape);
        assert(center.x == 50 && center.y == 50 && "getShapeCenter should return shape.center");
    }

    // Test getOffsetCenter
    {
        Prefab_St prefab = {0};
        prefab.blockCount = 2;
        prefab.offsets[0] = (u8Vector2){0, 0};
        prefab.offsets[1] = (u8Vector2){2, 2};

        f32Vector2 center = getOffsetCenter(prefab);
        assert(center.x == 1.0f && center.y == 1.0f && "Offset center should be average");
    }

    // Test getIthBlockPosition
    {
        Prefab_St prefab = {
            .offsets = {{1, 1}}
        };

        ActivePrefab_St shape = {0};
        shape.prefab = &prefab;
        shape.center = (f32Vector2){0, 0};

        f32Vector2 pos = getIthBlockPosition(shape, 0);
        assert(pos.x == BLOCK_PX_SIZE * 1 && pos.y == BLOCK_PX_SIZE * 1 && "Block position calculation");
    }

    // Test mapShapeToBoardPos
    {
        ActivePrefab_St shape = {0};
        shape.center = (f32Vector2){BLOCK_PX_SIZE / 2.0f, BLOCK_PX_SIZE / 2.0f};

        s8Vector2 boardPos = mapShapeToBoardPos(&shape);
        assert(boardPos.x == 0 && boardPos.y == 0 && "Maps to board (0,0)");
    }

    // Test addPrefabAndVariants (complex; test basic addition)
    {
        PrefabBagVec_St bag = {0};
        Prefab_St prefab = {0};
        prefab.orientations = 1;
        prefab.canMirror = false;

        addPrefabAndVariants(prefab, &bag);
        assert(bag.count > 0 && "Bag should have added prefab");

        // Free bag if needed
    }

    // Test handleShape (depends on input/mouse; integration test—skipped)

    // Test shuffleSlots (depends on globals/PRNG; check if it shuffles without crash)
    {
        GameState_St testGame = {0};
        shuffleSlots(&testGame);
        // Assert no crash; manual verification for shuffle
    }

    // Test placeShape
    {
        Board_St board = {0};
        board.width = 8; board.height = 8;
     
        Prefab_St prefab = {
            .blockCount = 1,
            .offsets = {{0, 0}},
        };
     
        ActivePrefab_St shape = {0};
        shape.prefab = &prefab;
        shape.colorIndex = 1;

        placeShape(&shape, (u8Vector2) {0, 0}, &board);
        assert(board.blocks[0][0].hitsLeft == 1 && board.blocks[0][0].colorIndex == 1 && "Shape placed correctly");
    }

    // Test rotatePrefab
    {
        Prefab_St prefab = {
            .blockCount = 2,
            .offsets = {
                {0, 0}, {1, 0}
            }
        };

        rotatePrefab(&prefab, 1);  // 90 degrees
        assert(prefab.offsets[0].x == 0 && prefab.offsets[0].y == 1 && "Rotated correctly (assuming CCW)");
    }

    // Test mirrorPrefab
    {
        Prefab_St prefab = {
            .blockCount = 2,
            .offsets = {
                {0, 0}, {1, 0}
            }
        };

        mirrorPrefab(&prefab);
        assert(prefab.offsets[1].x == 1 && prefab.offsets[1].y == 0 && "Mirrored over y-axis");
    }

    // Test releaseShape
    {
        Prefab_St prefab = {
            .blockCount = 1,
        };

        Board_St board = {0};
        board.width = 8; board.height = 8;
        ActivePrefab_St shape = {0};
        shape.prefab = &prefab;
        shape.dragging = true;

        releaseShapeAt(&shape, (s8Vector2) {0, 0}, &board);
        assert(shape.placed == true && shape.dragging == false && "Shape released and placed");
    }

    // Test printPrefabInfo (outputs to stdout; check no crash)
    {
        Prefab_St prefab = {0};
        printPrefabInfo(prefab);  // Manual verify output
    }

    printf("All tests passed for core/game/shape!\n");
    return 0;
}