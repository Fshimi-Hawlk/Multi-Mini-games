#ifndef CONSTANT_H
#define CONSTANT_H

#define SKY_WIDTH 8000.0f
#define SKY_HEIGHT 2000.0f

#define X_LIMIT 1200.0f

#define GROUND_Y      500.0f
#define PLAT_H        20.0f
#define STEP_Y        100.0f
#define WALL_W        100.0f
#define DOOR_H        200.0f

#define GRAVITY 1200.0f
#define MOVE_SPEED 300.0f
#define JUMP_FORCE 500.0f

#define COYOTE_TIME 0.2f
#define JUMP_BUFFER_TIME 0.2f
#define MAX_JUMPS 2

#define FRICTION 2000

#define MAX_GRASS_BLADES 80000

#define MAX_FIREFLIES 46

// ── Tree canopy leaf spawn zone ─────────────────
#define CANOPY_CENTER_X        0.0f
#define CANOPY_CENTER_Y      -425.0f
#define CANOPY_RADIUS_MIN     800.0f
#define CANOPY_RADIUS_MAX    1200.0f
#define CANOPY_ARC_START_ANGLE -185.0f
#define CANOPY_ARC_END_ANGLE   -10.0f

// ── Leaf Constant ─────────────────
#define MAX_FALLING_LEAVES 80
#define LEAF_BASE_LIFE        42.0f      // extended so pushed leaves always reach ground
#define LEAF_GROUND_TIME      8.5f
#define LEAF_GRAVITY          265.0f     // significantly faster fall (no longer "very very slow")
#define LEAF_PLAYER_PUSH      178.0f     // gentle push, no space-flinging
#define LEAF_SPIN_DAMP_TIME   4.3f       // temporary strong drag after player push only
#define LEAF_ROT_DRAG_NORMAL  0.9992f     // very light constant drag
#define LEAF_ROT_DRAG_STRONG  0.935f     // strong but short-lived drag after push

#define LEAF_FLUTTER_FREQUENCY  1.5f      // higher = faster wiggle
#define LEAF_FLUTTER_AMPLITUDE  63.0f     // higher = stronger visible side-to-side movement
#define LEAF_DRIFT_FREQUENCY    0.25f     // slow, long curves (still visible even if flutter = 0)
#define LEAF_DRIFT_AMPLITUDE    135.0f     // strength of the gentle curving drift

#endif