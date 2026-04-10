#ifndef CORE_GAME_H
#define CORE_GAME_H

#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef ASSET_PATH
#define ASSET_PATH "assets/"
#endif

#define SCREEN_W    1280
#define SCREEN_H    720
#define TARGET_FPS  60
#define GAME_TITLE  "Golf 3D"

#define BALL_RADIUS_CM     2.134f
#define HOLE_RADIUS_CM     5.4f
#define FLAGPOLE_HEIGHT_CM 213.36f

#define SCALE  0.1f
#define BALL_R  (BALL_RADIUS_CM  * SCALE)
#define HOLE_R  (HOLE_RADIUS_CM  * SCALE)
#define FLAG_H  (FLAGPOLE_HEIGHT_CM * SCALE)

#define GRAVITY          9.81f
#define FRICTION_FAIRWAY 0.92f
#define FRICTION_ROUGH   0.80f
#define FRICTION_GREEN   0.96f
#define FRICTION_SAND    0.65f
#define BOUNCE_FACTOR    0.35f
#define SLOPE_INFLUENCE  0.4f
#define STOP_THRESHOLD   0.04f

typedef enum {
    CLUB_DRIVER = 0,
    CLUB_WOOD3,
    CLUB_IRON5,
    CLUB_IRON9,
    CLUB_WEDGE,
    CLUB_PUTTER,
    CLUB_COUNT
} ClubType;

typedef struct {
    const char *name;
    float       max_power;
    float       loft_deg;
    float       accuracy;
    float       max_dist_m;
} ClubSpec;

extern const ClubSpec CLUBS[CLUB_COUNT];

typedef enum {
    SURF_FAIRWAY = 0,
    SURF_ROUGH,
    SURF_GREEN,
    SURF_SAND,
    SURF_WATER,
    SURF_OOB
} SurfaceType;

#define MAX_HOLES   9
#define MAX_HAZARDS 8

typedef struct {
    Vector3     pos;
    float       radius;
    SurfaceType surface;
} Hazard;

typedef struct {
    int       index;
    int       par;
    float     distance_m;
    Vector3   tee_pos;
    Vector3   hole_pos;
    Vector3   green_center;
    float     green_radius;
    Hazard    hazards[MAX_HAZARDS];
    int       hazard_count;
    float     terrain_seed;
    Color     fairway_color;
    Color     rough_color;
} HoleData;

typedef enum {
    BALL_IDLE = 0,
    BALL_FLYING,
    BALL_ROLLING,
    BALL_IN_HOLE
} BallState;

typedef struct {
    Vector3     pos;
    Vector3     vel;
    BallState   state;
    SurfaceType surface;
    float       spin;
    float       rot_angle;
    int         strokes;
    int         penalty;
    Vector3     last_valid;
} Ball;

typedef struct {
    float   speed_kmh;
    float   direction_deg;
    Vector3 vec;
} Wind;

typedef enum {
    CAM_ORBIT = 0,
    CAM_FOLLOW,
    CAM_TOP,
    CAM_HOLE
} CamMode;

typedef struct {
    Camera3D cam;
    CamMode  mode;
    float    yaw;
    float    pitch;
    float    distance;
    Vector3  target;
    float    fov;
} GolfCamera;

typedef enum {
    STATE_MENU = 0,
    STATE_HOLE_INTRO,
    STATE_AIMING,
    STATE_POWER,
    STATE_BALL_MOVING,
    STATE_HOLE_RESULT,
    STATE_SCORECARD,
    STATE_PAUSED
} GameState;

#define SCORE_HOLE_OUT 99

typedef struct {
    int strokes[MAX_HOLES];
    int total;
    int to_par;
} ScoreCard;

typedef struct {
    GameState  state;
    GameState  prev_state;

    HoleData   holes[MAX_HOLES];
    int        current_hole;

    Ball       ball;
    ClubType   club;

    float      aim_angle;
    float      power;
    bool       power_rising;

    Wind       wind;
    GolfCamera gcam;

    Texture2D  tex_ball;
    Texture2D  tex_club;

    ScoreCard  score;

    float      state_timer;
    float      intro_timer;

    int        screen_w;
    int        screen_h;

    bool       show_trajectory;

    Vector3    shot_positions[1000];
    int        shot_pos_count;
} GolfGame;

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

static inline Color score_color(int diff) {
    if (diff <= -2) return (Color){255, 215,   0, 255};
    if (diff == -1) return (Color){ 65, 200,  65, 255};
    if (diff ==  0) return (Color){255, 255, 255, 255};
    if (diff ==  1) return (Color){255, 160,  50, 255};
    return              (Color){220,  50,  50, 255};
}

#endif