#ifndef CONSTANT_H
#define CONSTANT_H

#define GRAVITY          1200.0f    ///< Downward acceleration applied every frame (pixels/second²).
#define MOVE_SPEED       300.0f     ///< Horizontal movement speed when holding left/right (pixels/second).
#define JUMP_FORCE       500.0f     ///< Upward velocity applied on jump (pixels/second).
#define COYOTE_TIME      0.2f       ///< Time window (seconds) after leaving ground where jump is still allowed.
#define JUMP_BUFFER_TIME 0.2f       ///< Time window (seconds) before landing where a pressed jump is still accepted.
#define MAX_JUMPS        2          ///< Maximum number of jumps allowed without touching ground (includes ground jump).
#define FRICTION         2000.0f    ///< Horizontal deceleration rate when no input is given (pixels/second²).

#define MAX_FALL_SPEED   920.0f     ///< Maximum downward speed (pixels/second). Caps y-velocity when airborne to prevent tunneling.
#define AIR_DRAG         0.3f       ///< Air resistance coefficient for falling (higher = stronger drag). Applied only when falling in air.

#define GROUND_Y         500.0f     ///< Y-position considered "ground level" for initial spawn / debug.
#define PLAT_H           20.0f
#define STEP_Y           100.0f
#define WALL_W           100.0f
#define DOOR_H           200.0f

#define PLAYER_SPAWN_X  -425.0f
#define PLAYER_SPAWN_Y  GROUND_Y - 20.0f

#define SKY_WIDTH 8000.0f
#define SKY_HEIGHT 2000.0f

#define X_LIMIT 1600.0f

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