/**
    @file golf.h
    @author Maxime CHAUVEAU
    @date 2026-04-14
    @date 2026-04-14
    @brief Core definitions and types for the Golf 3D mini-game.
*/
#ifndef GOLF_H
#define GOLF_H

#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ─── Asset path (overridden by root Makefile via -DASSET_PATH="...") ─────── */

#ifndef ASSET_PATH
/**
    @brief Root directory for all game assets.
*/
#define ASSET_PATH "assets/"
#endif

/* ─── Fenêtre ─────────────────────────────────────────────────────────────── */

#define SCREEN_W    1280    ///< Default screen width
#define SCREEN_H    720     ///< Default screen height
#define TARGET_FPS  60      ///< Target frame rate
#define GAME_TITLE  "Golf 3D" ///< Window title

/* ─── Règles officielles R&A / USGA (1 unité monde = 10 cm) ─────────────── */

#define BALL_RADIUS_CM     2.134f   ///< Ball radius in cm
#define HOLE_RADIUS_CM     5.4f     ///< Hole radius in cm
#define FLAGPOLE_HEIGHT_CM 213.36f  ///< Flagpole height in cm

#define SCALE  0.1f                 ///< World scale (1.0 = 1 meter)

#define BALL_R  (BALL_RADIUS_CM  * SCALE)   ///< Scaled ball radius
#define HOLE_R  (HOLE_RADIUS_CM  * SCALE)   ///< Scaled hole radius
#define FLAG_H  (FLAGPOLE_HEIGHT_CM * SCALE) ///< Scaled flag height

/* ─── Physique ────────────────────────────────────────────────────────────── */

#define GRAVITY          9.81f      ///< Gravity constant (m/s^2)
#define FRICTION_FAIRWAY 0.92f      ///< Friction coefficient for fairway
#define FRICTION_ROUGH   0.80f      ///< Friction coefficient for rough
#define FRICTION_GREEN   0.96f      ///< Friction coefficient for green
#define FRICTION_SAND    0.65f      ///< Friction coefficient for sand
#define BOUNCE_FACTOR    0.35f      ///< Bounce energy loss factor
#define SLOPE_INFLUENCE  0.4f       ///< Influence of slope on ball movement
#define STOP_THRESHOLD   0.04f      ///< Velocity threshold below which the ball stops

/**
    @brief Types of golf clubs available.
*/
typedef enum {
    CLUB_DRIVER = 0,    ///< Longest distance, low loft
    CLUB_WOOD3,         ///< Long distance from fairway
    CLUB_IRON5,         ///< Mid-range iron
    CLUB_IRON9,         ///< Short-range iron
    CLUB_WEDGE,         ///< High loft, for approach and bunkers
    CLUB_PUTTER,        ///< For the green
    CLUB_COUNT          ///< Total number of clubs
} ClubType;

/**
    @brief Specifications for a golf club.
*/
typedef struct {
    const char *name;       ///< Display name of the club
    float       max_power;  ///< Maximum power multiplier
    float       loft_deg;   ///< Loft angle in degrees
    float       accuracy;   ///< Accuracy modifier
    float       max_dist_m; ///< Estimated maximum distance in meters
} ClubSpec;

/**
    @brief Array of club specifications.
*/
extern const ClubSpec CLUBS[CLUB_COUNT];

/**
    @brief Types of terrain surfaces.
*/
typedef enum {
    SURF_FAIRWAY = 0,   ///< Regular fairway
    SURF_ROUGH,        ///< Tall grass, high friction
    SURF_GREEN,        ///< Low friction, around the hole
    SURF_SAND,         ///< Bunker, very high friction
    SURF_WATER,        ///< Water hazard (penalty)
    SURF_OOB           ///< Out of bounds (penalty)
} SurfaceType;

/* ─── Trou ────────────────────────────────────────────────────────────────── */

#define MAX_HOLES   9   ///< Maximum number of holes in a course
#define MAX_HAZARDS 8   ///< Maximum hazards per hole

/**
    @brief A hazard area on the course (e.g., bunker, pond).
*/
typedef struct {
    Vector3     pos;    ///< Center position of the hazard
    float       radius; ///< Radius of the hazard area
    SurfaceType surface; ///< Type of surface in this hazard
} Hazard;

/**
    @brief Data for a single golf hole.
*/
typedef struct {
    int       index;        ///< Hole number (0-indexed)
    int       par;          ///< Par for the hole
    float     distance_m;   ///< Total distance from tee to hole
    Vector3   tee_pos;      ///< Starting position of the ball
    Vector3   hole_pos;     ///< Target hole position
    Vector3   green_center; ///< Center of the green area
    float     green_radius; ///< Radius of the green area
    Hazard    hazards[MAX_HAZARDS]; ///< Array of hazards for this hole
    int       hazard_count; ///< Number of active hazards
    float     terrain_seed; ///< Seed for terrain generation
    Color     fairway_color; ///< Color of the fairway
    Color     rough_color;   ///< Color of the rough
} HoleData;

/**
    @brief States of the golf ball.
*/
typedef enum {
    BALL_IDLE = 0,      ///< Static on the ground
    BALL_FLYING,        ///< In the air
    BALL_ROLLING,       ///< Rolling on the ground
    BALL_IN_HOLE        ///< Successfully in the hole
} BallState;

/**
    @brief Ball state and physics data.
*/
typedef struct {
    Vector3     pos;        ///< Current position
    Vector3     vel;        ///< Current velocity
    BallState   state;      ///< Current movement state
    SurfaceType surface;    ///< Type of surface the ball is currently on
    float       spin;       ///< Ball spin (affects trajectory)
    float       rot_angle;  ///< Visual rotation angle
    int         strokes;    ///< Number of strokes for the current hole
    int         penalty;    ///< Cumulative penalties
    Vector3     last_valid; ///< Last safe position (for water/OOB recovery)
} Ball;

/**
    @brief Wind parameters.
*/
typedef struct {
    float   speed;      ///< Wind speed
    float   direction;  ///< Direction in degrees
    Vector3 vec;            ///< Pre-calculated wind vector
} Wind;

/**
    @brief Camera modes.
*/
typedef enum {
    CAM_ORBIT = 0,  ///< Orbiting around the ball
    CAM_FOLLOW,     ///< Following the ball in flight
    CAM_TOP,        ///< Overhead view
    CAM_HOLE        ///< View from the hole looking back
} CamMode;

/**
    @brief Camera state and configuration.
*/
typedef struct {
    Camera3D cam;       ///< Raylib 3D camera
    CamMode  mode;      ///< Current camera mode
    float    yaw;       ///< Rotation around Y axis
    float    pitch;     ///< Rotation around X axis
    float    distance;  ///< Distance from target
    Vector3  target;    ///< Point the camera is looking at
    float    fov;       ///< Field of view
} GolfCamera;

/**
    @brief Game states for the main loop.
*/
typedef enum {
    STATE_MENU = 0,     ///< Main menu
    STATE_HOLE_INTRO,   ///< Flyover or intro text
    STATE_AIMING,       ///< Player aiming the shot
    STATE_POWER,        ///< Player selecting power
    STATE_BALL_MOVING,  ///< Ball is in motion
    STATE_HOLE_RESULT,  ///< Result of the current hole
    STATE_SCORECARD,    ///< Full course scorecard
    STATE_PAUSED        ///< Game paused
} GameState;

/* ─── Score ───────────────────────────────────────────────────────────────── */

#define SCORE_HOLE_OUT 99 ///< Maximum score per hole

/**
    @brief Scorecard for the entire course.
*/
typedef struct {
    int strokes[MAX_HOLES]; ///< Strokes per hole
    int total;              ///< Total strokes
    int to_par;             ///< Total relative to par
} ScoreCard;

/**
    @brief Global state of the Golf game.
*/
typedef struct {
    GameState  state;       ///< Current game state
    GameState  prev_state;  ///< Previous state (for pause recovery)

    HoleData   holes[MAX_HOLES]; ///< Data for all holes
    int        current_hole;     ///< Current hole index

    Ball       ball;        ///< The golf ball
    ClubType   club;        ///< Currently selected club

    float      aim_angle;   ///< Current aiming angle
    float      power;       ///< Current shot power
    bool       power_rising; ///< Power bar animation direction

    Wind       wind;        ///< Wind conditions
    GolfCamera gcam;        ///< The 3D camera

    Texture2D  tex_ball;    ///< Ball texture
    Texture2D  tex_club;    ///< Club icon/texture

    ScoreCard  score;       ///< The scorecard

    float      state_timer; ///< Timer for state transitions
    float      intro_timer; ///< Timer for hole intro

    int        screen_w;    ///< Actual screen width
    int        screen_h;    ///< Actual screen height

    bool       show_trajectory; ///< Whether to show the predicted path

    Vector3    shot_positions[1000]; ///< History of positions for the last shot
    int        shot_pos_count;       ///< Number of points in the history
} GolfGame;

/**
    @brief Returns a human-readable name for a score differential.
    @param[in] diff Difference between strokes and par.
    @return          String description (e.g., "Birdie", "Par").
*/
static inline const char *score_name(int diff) {
    if (diff <= -3) return "Albatros";
    if (diff == -2) return "Eagle";
    if (diff == -1) return "Birdie";
    if (diff ==  0) return "Par";
    if (diff ==  1) return "Bogey";
    if (diff ==  2) return "Double Bogey";
    if (diff ==  3) return "Triple Bogey";
    return "Hors-jeu";
}

/**
    @brief Returns a color associated with a score differential.
    @param[in] diff Difference between strokes and par.
    @return          Raylib Color.
*/
static inline Color score_color(int diff) {
    if (diff <= -2) return (Color){255, 215,   0, 255};
    if (diff == -1) return (Color){ 65, 200,  65, 255};
    if (diff ==  0) return (Color){255, 255, 255, 255};
    if (diff ==  1) return (Color){255, 160,  50, 255};
    return              (Color){220,  50,  50, 255};
}

/* ─── Prototypes ─────────────────────────────────────────────────────────── */

/* golf_course.c */

/**
    @brief Initializes the course with all hole data.
    @param[in,out] g Golf game state.
    @return          None.
*/
void        Golf_InitCourse(GolfGame *g);

/**
    @brief Resets the ball and state for a specific hole.
    @param[in,out] g        Golf game state.
    @param[in]     hole_idx Index of the hole to start (0-indexed).
    @return                 None.
*/
void        Golf_StartHole(GolfGame *g, int hole_idx);

/**
    @brief Calculates the terrain height at a given (x, z) coordinate.
    @param[in] g Golf game state.
    @param[in] x World X coordinate.
    @param[in] z World Z coordinate.
    @return      Height at the specified position.
*/
float       Golf_GetTerrainHeight(GolfGame *g, float x, float z);

/**
    @brief Returns the surface type at a given (x, z) coordinate.
    @param[in] g Golf game state.
    @param[in] x World X coordinate.
    @param[in] z World Z coordinate.
    @return      SurfaceType (Fairway, Green, etc.).
*/
SurfaceType Golf_GetSurface(GolfGame *g, float x, float z);

/**
    @brief Calculates the terrain normal vector at a given (x, z) coordinate.
    @param[in] g Golf game state.
    @param[in] x World X coordinate.
    @param[in] z World Z coordinate.
    @return      Normalized Vector3 pointing upwards from the surface.
*/
Vector3     Golf_GetTerrainNormal(GolfGame *g, float x, float z);

/**
    @brief Renders the terrain geometry.
    @param[in] g Golf game state.
    @return      None.
*/
void        Golf_DrawTerrain(GolfGame *g);

/**
    @brief Renders all hazards (sand, water) on the current hole.
    @param[in] g Golf game state.
    @return      None.
*/
void        Golf_DrawHazards(GolfGame *g);

/**
    @brief Renders the flagpole at the hole position.
    @param[in] g Golf game state.
    @return      None.
*/
void        Golf_DrawFlag(GolfGame *g);

/**
    @brief Renders the physical hole cup.
    @param[in] g Golf game state.
    @return      None.
*/
void        Golf_DrawHoleCup(GolfGame *g);

/* golf_ball.c */

/**
    @brief Initializes ball physics and position.
    @param[out] b   Ball structure to initialize.
    @param[in]  pos Initial position.
    @return         None.
*/
void  Ball_Init(Ball *b, Vector3 pos);

/**
    @brief Launches the ball based on current power and aim.
    @param[in,out] g Golf game state.
    @return          None.
*/
void  Ball_Shoot(GolfGame *g);

/**
    @brief Updates ball physics for one frame.
    @param[in,out] g  Golf game state.
    @param[in]     dt Delta time in seconds.
    @return           None.
*/
void  Ball_Update(GolfGame *g, float dt);

/**
    @brief Renders the ball in the 3D world.
    @param[in] g Golf game state.
    @return      None.
*/
void  Ball_Draw(GolfGame *g);

/**
    @brief Checks if the ball has entered the hole cup.
    @param[in] g Golf game state.
    @return      True if the ball is in the hole.
*/
bool  Ball_IsInHole(GolfGame *g);

/**
    @brief Applies wind force to the ball's velocity.
    @param[in,out] g  Golf game state.
    @param[in]     dt Delta time in seconds.
    @return           None.
*/
void  Ball_ApplyWindEffect(GolfGame *g, float dt);

/**
    @brief Renders a predicted trajectory for the ball.
    @param[in] g Golf game state.
    @return      None.
*/
void  Ball_DrawTrajectory(GolfGame *g);

/* golf_camera.c */

/**
    @brief Initializes camera settings and target.
    @param[out] c      Golf camera to initialize.
    @param[in]  target Initial focus point.
    @return            None.
*/
void  GCam_Init(GolfCamera *c, Vector3 target);

/**
    @brief Updates camera position and orientation.
    @param[in,out] c  Golf camera.
    @param[in]     g  Golf game state.
    @param[in]     dt Delta time in seconds.
    @return           None.
*/
void  GCam_Update(GolfCamera *c, GolfGame *g, float dt);

/**
    @brief Changes the current camera mode.
    @param[in,out] c    Golf camera.
    @param[in]     mode New CamMode.
    @return             None.
*/
void  GCam_SetMode(GolfCamera *c, CamMode mode);

/**
    @brief Prepares Raylib for 3D rendering with this camera.
    @param[in] c Golf camera.
    @return      None.
*/
void  GCam_BeginMode3D(GolfCamera *c);

/**
    @brief Returns Raylib to 2D rendering mode.
    @return None.
*/
void  GCam_EndMode3D(void);

/* golf_ui.c */

/**
    @brief Renders the main heads-up display.
    @param[in] g Golf game state.
    @return      None.
*/
void  UI_DrawHUD(GolfGame *g);

/**
    @brief Renders the main menu overlay.
    @param[in] g Golf game state.
    @return      None.
*/
void  UI_DrawMenu(GolfGame *g);

/**
    @brief Renders the power selection bar.
    @param[in] g Golf game state.
    @return      None.
*/
void  UI_DrawPower(GolfGame *g);

/**
    @brief Renders the hole introduction text/overlay.
    @param[in] g Golf game state.
    @return      None.
*/
void  UI_DrawHoleIntro(GolfGame *g);

/**
    @brief Renders the result screen after completing a hole.
    @param[in] g Golf game state.
    @return      None.
*/
void  UI_DrawHoleResult(GolfGame *g);

/**
    @brief Renders the full course scorecard.
    @param[in] g Golf game state.
    @return      None.
*/
void  UI_DrawScorecard(GolfGame *g);

/**
    @brief Renders the pause menu overlay.
    @param[in] g Golf game state.
    @return      None.
*/
void  UI_DrawPaused(GolfGame *g);

/**
    @brief Renders a top-down minimap of the hole.
    @param[in] g Golf game state.
    @return      None.
*/
void  UI_DrawMinimap(GolfGame *g);

/**
    @brief Renders the wind speed and direction indicator.
    @param[in] g Golf game state.
    @return      None.
*/
void  UI_DrawWindIndicator(GolfGame *g);

/**
    @brief Renders the club selection menu/indicator.
    @param[in] g Golf game state.
    @return      None.
*/
void  UI_DrawClubSelector(GolfGame *g);

/* golf_main.c (anciennement main.c) */

/**
    @brief Initializes the full game state and assets.
    @param[out] g Golf game state to initialize.
    @return      None.
*/
void  Game_Init(GolfGame *g);

/**
    @brief Updates game logic for one frame.
    @param[in,out] g  Golf game state.
    @param[in]     dt Delta time in seconds.
    @return           None.
*/
void  Game_Update(GolfGame *g, float dt);

/**
    @brief Renders the entire game frame.
    @param[in] g Golf game state.
    @return      None.
*/
void  Game_Draw(GolfGame *g);

/**
    @brief Unloads all assets and cleans up memory.
    @param[in,out] g Golf game state.
    @return          None.
*/
void  Game_Cleanup(GolfGame *g);

/**
    @brief Generates a new random wind condition.
    @param[in,out] g Golf game state.
    @return          None.
*/
void  Game_NewWind(GolfGame *g);

#endif /* GOLF_H */
