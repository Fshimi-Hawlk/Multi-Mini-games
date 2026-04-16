/**
    @file game.h
    @author Maxime CHAUVEAU
    @date 2026-04-07
    @date 2026-04-14
    @brief Core game structures and constants for Golf 3D.
*/
#ifndef CORE_GAME_H
#define CORE_GAME_H

#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef ASSET_PATH
/**
    @brief Root directory for all game assets.
*/
#define ASSET_PATH "assets/"
#endif

#define SCREEN_W    1280    ///< Default window width
#define SCREEN_H    720     ///< Default window height
#define TARGET_FPS  60      ///< Target frame rate for gameplay
#define GAME_TITLE  "Golf 3D" ///< Window title

#define BALL_RADIUS_CM     2.134f  ///< Official golf ball radius in cm
#define HOLE_RADIUS_CM     5.4f    ///< Official golf hole radius in cm
#define FLAGPOLE_HEIGHT_CM 213.36f ///< Typical flagpole height (7 feet)

#define SCALE  0.1f                            ///< World scale factor (1 unit = 10m)
#define BALL_R  (BALL_RADIUS_CM  * SCALE)      ///< Scaled ball radius
#define HOLE_R  (HOLE_RADIUS_CM  * SCALE)      ///< Scaled hole radius
#define FLAG_H  (FLAGPOLE_HEIGHT_CM * SCALE)   ///< Scaled flag height

#define GRAVITY          9.81f  ///< Earth gravity acceleration (m/s^2)
#define FRICTION_FAIRWAY 0.92f  ///< Friction coefficient for fairway grass
#define FRICTION_ROUGH   0.80f  ///< Friction coefficient for rough grass
#define FRICTION_GREEN   0.96f  ///< Friction coefficient for putting green
#define FRICTION_SAND    0.65f  ///< Friction coefficient for bunkers
#define BOUNCE_FACTOR    0.35f  ///< Energy conservation after bounce
#define SLOPE_INFLUENCE  0.4f   ///< Gravity effect on rolling ball
#define STOP_THRESHOLD   0.04f  ///< Minimum speed before ball stops

/**
    @brief Available golf clubs.
*/
typedef enum {
    CLUB_DRIVER = 0,    ///< Longest distance club
    CLUB_WOOD3,         ///< Long fairway club
    CLUB_IRON5,         ///< Mid-range iron
    CLUB_IRON9,         ///< Short-range iron
    CLUB_WEDGE,         ///< High-lofted club for short distance
    CLUB_PUTTER,        ///< Club for the green
    CLUB_COUNT          ///< Total number of clubs
} ClubType;

/**
    @brief Physical specifications for a specific golf club.
*/
typedef struct {
    const char *name;       ///< Display name of the club
    float       max_power;  ///< Maximum swing power
    float       loft_deg;   ///< Vertical angle of the shot
    float       accuracy;   ///< Accuracy factor (0.0 to 1.0)
    float       max_dist_m; ///< Theoretical maximum distance in meters
} ClubSpec;

/**
    @brief Global club definitions.
*/
extern const ClubSpec CLUBS[CLUB_COUNT];

/**
    @brief Types of terrain surfaces.
*/
typedef enum {
    SURF_FAIRWAY = 0,   ///< Normal fairway grass
    SURF_ROUGH,         ///< Thick rough grass
    SURF_GREEN,         ///< Putting green
    SURF_SAND,          ///< Sand bunker
    SURF_WATER,         ///< Water hazard (penalty)
    SURF_OOB            ///< Out of bounds area
} SurfaceType;

#define MAX_HOLES   9   ///< Maximum number of holes in a round
#define MAX_HAZARDS 8   ///< Maximum number of hazards per hole

/**
    @brief A hazard on the course (e.g., sand trap, water).
*/
typedef struct {
    Vector3     pos;        ///< Center position of the hazard
    float       radius;     ///< Radius of the hazard area
    SurfaceType surface;    ///< Type of surface for this hazard
} Hazard;

/**
    @brief Data structure representing a single golf hole's properties and layout.
*/
typedef struct {
    int       index;                    ///< Hole number (0 to MAX_HOLES-1)
    int       par;                      ///< Par for this hole
    float     distance_m;               ///< Total distance from tee to hole
    Vector3   tee_pos;                  ///< Starting position
    Vector3   hole_pos;                 ///< Hole position
    Vector3   green_center;             ///< Center of the green area
    float     green_radius;             ///< Radius of the green
    Hazard    hazards[MAX_HAZARDS];     ///< List of hazards
    int       hazard_count;             ///< Number of active hazards
    float     terrain_seed;             ///< Seed for procedural generation
    Color     fairway_color;            ///< Custom color for fairway
    Color     rough_color;              ///< Custom color for rough
} HoleData;

/**
    @brief Possible states of the golf ball.
*/
typedef enum {
    BALL_IDLE = 0,      ///< Stationary on the ground
    BALL_FLYING,        ///< Moving through the air
    BALL_ROLLING,       ///< Rolling on the ground
    BALL_IN_HOLE        ///< Successfully in the hole
} BallState;

/**
    @brief State and properties of the golf ball.
*/
typedef struct {
    Vector3     pos;            ///< Current world position
    Vector3     vel;            ///< Current velocity vector
    BallState   state;          ///< Movement state
    SurfaceType surface;        ///< Current surface type under the ball
    float       spin;           ///< Current backspin/topspin
    float       rot_angle;      ///< Visual rotation angle
    int         strokes;        ///< Number of strokes on current hole
    int         penalty;        ///< Number of penalty strokes
    Vector3     last_valid;     ///< Last valid position on fairway/green
} Ball;

/**
    @brief Environmental wind conditions.
*/
typedef struct {
    float   speed_kmh;      ///< Wind speed in km/h
    float   direction_deg;  ///< Wind direction in degrees
    Vector3 vec;            ///< Pre-calculated wind force vector
} Wind;

/**
    @brief Camera behavior modes.
*/
typedef enum {
    CAM_ORBIT = 0,  ///< Orbiting around the ball
    CAM_FOLLOW,     ///< Following the ball in flight
    CAM_TOP,        ///< Top-down view of the course
    CAM_HOLE        ///< View from the hole looking back
} CamMode;

/**
    @brief Golf camera state.
*/
typedef struct {
    Camera3D cam;       ///< Raylib 3D camera
    CamMode  mode;      ///< Current camera mode
    float    yaw;       ///< Horizontal rotation
    float    pitch;     ///< Vertical rotation
    float    distance;  ///< Distance from target
    Vector3  target;    ///< Look-at target
    float    fov;       ///< Field of view
} GolfCamera;

/**
    @brief Main game lifecycle states.
*/
typedef enum {
    STATE_MENU = 0,     ///< Main menu
    STATE_HOLE_INTRO,   ///< Introduction to the current hole
    STATE_AIMING,       ///< Player is aiming their shot
    STATE_POWER,        ///< Power meter is active
    STATE_BALL_MOVING,  ///< Ball is in motion
    STATE_HOLE_RESULT,  ///< Hole completed, showing result
    STATE_SCORECARD,    ///< Showing full scorecard
    STATE_PAUSED        ///< Game is paused
} GameState;

#define SCORE_HOLE_OUT 99 ///< Special score value for retired player

/**
    @brief Player's score tracking.
*/
typedef struct {
    int strokes[MAX_HOLES]; ///< Strokes per hole
    int total;              ///< Total strokes
    int to_par;             ///< Total relative to par
} ScoreCard;

/**
    @brief Global state for the Golf 3D game.
*/
typedef struct {
    GameState  state;           ///< Current game state
    GameState  prev_state;      ///< State before pausing/menu

    HoleData   holes[MAX_HOLES];///< Data for all holes
    int        current_hole;    ///< Current hole index

    Ball       ball;            ///< The golf ball
    ClubType   club;            ///< Currently selected club

    float      aim_angle;       ///< Current aiming direction
    float      power;           ///< Current swing power (0.0 to 1.0)
    bool       power_rising;    ///< Direction of power meter oscillation

    Wind       wind;            ///< Current wind conditions
    GolfCamera gcam;            ///< The 3D camera

    Texture2D  tex_ball;        ///< Ball texture
    Texture2D  tex_club;        ///< Club texture/model
    Texture2D  tex_fairway;     ///< Fairway terrain texture
    Texture2D  tex_rough;       ///< Rough terrain texture
    Texture2D  tex_green;       ///< Green terrain texture
    Texture2D  tex_sand;        ///< Sand terrain texture
    Texture2D  tex_water;       ///< Water terrain texture

    ScoreCard  score;           ///< Current score

    float      state_timer;     ///< Time spent in current state
    float      intro_timer;     ///< Timer for hole intro animation

    int        screen_w;        ///< Window width
    int        screen_h;        ///< Window height

    bool       show_trajectory; ///< Whether to draw the flight path

    Vector3    shot_positions[1000]; ///< History of ball positions for trail
    int        shot_pos_count;       ///< Number of positions in history
} GolfGame;

/**
    @brief Returns the name of a score based on the difference to par.

    @param[in]  diff  The difference between strokes and par.
    @return           The name of the score (e.g., "Birdie", "Par").
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
    @brief Returns the display color for a score based on the difference to par.

    @param[in]  diff  The difference between strokes and par.
    @return           The Color associated with that score.
*/
static inline Color score_color(int diff) {
    if (diff <= -2) return (Color){255, 215,   0, 255};
    if (diff == -1) return (Color){ 65, 200,  65, 255};
    if (diff ==  0) return (Color){255, 255, 255, 255};
    if (diff ==  1) return (Color){255, 160,  50, 255};
    return              (Color){220,  50,  50, 255};
}

#endif /* CORE_GAME_H */

