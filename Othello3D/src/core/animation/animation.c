#include "core/animation/animation.h"
#include "utils/globals.h"
#include "core/animation/easing.h"
#include "utils/logger/logger.h"
#include "utils/types.h"

void initAnimation(Board_t board, s64Vector2_St initPos, CellState_Et toState, s64Vector2_St flipped[], u64_t flippedCount) {
    // Start animations instead of immediate place
    animationsPlaying = true;
    animCount = 0;

    // Place anim
    animPieces[animCount].pos = initPos;
    animPieces[animCount].type = ANIM_PLACE;
    animPieces[animCount].timer = 0.0f;
    animPieces[animCount].duration = 0.5f;
    animPieces[animCount].fromState = EMPTY;
    animPieces[animCount].toState = toState;
    animCount++;

    // Flip anims
    for (u64_t i = 0; i < flippedCount; i++) {
        s64Vector2_St fpos = flipped[i];
        CellState_Et from = board[fpos.y][fpos.x];  // current (opposite)
        CellState_Et to = toState;
        animPieces[animCount].pos = fpos;
        animPieces[animCount].type = ANIM_FLIP;
        animPieces[animCount].timer = 0.0f;
        animPieces[animCount].duration = 0.5f;
        animPieces[animCount].fromState = from;
        animPieces[animCount].toState = to;
        animCount++;
    }
}

bool advanceAnimation(Board_t board) {
    if (animationsPlaying) {
        // Update timers
        for (u64_t i = 0; i < animCount; i++) {
            animPieces[i].timer += GetFrameTime();
            if (animPieces[i].timer >= animPieces[i].duration) {
                // Finish anim: Apply final state
                if (animPieces[i].type == ANIM_PLACE || animPieces[i].type == ANIM_FLIP) {
                    board[animPieces[i].pos.y][animPieces[i].pos.x] = animPieces[i].toState;
                }
            }
        }

        // Check if all done
        bool allDone = true;
        for (u64_t i = 0; i < animCount; i++) {
            if (animPieces[i].timer < animPieces[i].duration) allDone = false;
        }

        if (allDone) {
            animCount = 0;
            animationsPlaying = false;
            return true;
        }
    }
    
    return false;
}
