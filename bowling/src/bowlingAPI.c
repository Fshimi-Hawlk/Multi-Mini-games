/**
 * @file bowlingAPI.c
 * @author Maxime CHAUVEAU
 * @date March 2026
 * @brief Bowling mini-game – full 3D rendering, confetti, audience, animated results.
 */

#include "bowlingAPI.h"
#include "APIs/generalAPI.h"
#include "utils/types.h"
#include "utils/assets.h"
#include "utils/configs.h"
#include "physics.h"
#include "logger.h"

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ─────────────────────────────────────────────────
// Constants – mesures officielles World Bowling / USBC
// 1 unité = 1 mètre
// ─────────────────────────────────────────────────
#define LANE_WIDTH      1.05f    // largeur piste officielle
#define LANE_LENGTH     18.29f   // ligne de faute → quille de tête
#define GUTTER_WIDTH    0.23f    // largeur d'une gouttière (23 cm)
#define APPROACH_LEN    4.57f    // zone d'élan (15 pieds)
#define BOWLING_WALL_Z  -20.5f   // mur arrière physique + visuel
#define BALL_RADIUS     0.1092f  // rayon max boule (diamètre 21.83 cm)
#define MAX_CONFETTI  200
#define RESULTS_STEPS 7

#if defined(__GNUC__) || defined(__clang__)
    #define FORCE_INLINE __attribute__((always_inline)) inline
    #define LIKELY(x)   __builtin_expect(!!(x), 1)
    #define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
    #define FORCE_INLINE inline
    #define LIKELY(x)   (x)
    #define UNLIKELY(x) (x)
#endif

// ─────────────────────────────────────────────────
// Ball skin palette
// ─────────────────────────────────────────────────
#define NUM_BALL_SKINS 8
static const Color BALL_SKIN_BODY[NUM_BALL_SKINS] = {
    { 15,  15,  75, 255},   // 0 Bleu nuit (défaut)
    {140,  20,  20, 255},   // 1 Rouge
    { 20, 110,  40, 255},   // 2 Vert
    { 90,  20, 140, 255},   // 3 Violet
    { 10,  10,  10, 255},   // 4 Noir
    {180,  80,  10, 255},   // 5 Orange
    { 10, 110, 130, 255},   // 6 Cyan
    {160,  30,  90, 255},   // 7 Rose
};
static const Color BALL_SKIN_RING[NUM_BALL_SKINS] = {
    { 60,  80, 200, 140},
    {200,  60,  60, 140},
    { 60, 180,  80, 140},
    {150,  60, 200, 140},
    { 80,  80,  80, 140},
    {220, 140,  40, 140},
    { 40, 190, 210, 140},
    {220,  80, 150, 140},
};
// supprime le warning unused : BALL_SKIN_RING reste disponible pour le fallback procédural
static inline Color bowling_getRingColor(int skin) {
    return BALL_SKIN_RING[skin % NUM_BALL_SKINS];
}
static const char* BALL_SKIN_NAMES[NUM_BALL_SKINS] = {
    "Bleu nuit", "Rouge", "Vert", "Violet",
    "Noir", "Orange", "Cyan", "Rose"
};

// ─────────────────────────────────────────────────
// Title screen state
// ─────────────────────────────────────────────────
typedef struct {
    bool showTitle;
    bool bumpers;
    bool showAimGuide;
    int  selectedSkin;
    Rectangle playBtn;
    Rectangle bumperBtn;
    Rectangle aimGuideBtn;
    Rectangle skinPrev;
    Rectangle skinNext;
} TitleScreen_St;

// ─────────────────────────────────────────────────
// Internal structs
// ─────────────────────────────────────────────────
typedef struct {
    int rolls[3];
    int numRolls;
    int score;
    bool isStrike;
    bool isSpare;
    int pinsKnockedFirstRoll;
    int cumulativeScore;
} Frame_St;

typedef struct {
    Camera3D camera;
    Camera3D defaultCamera;
    bool followingBall;
    float cameraLerpSpeed;
    Vector3 offset;
} CameraState_St;

typedef struct {
    int totalStrikes;
    int totalSpares;
    int gutterBalls;
    int totalPinsKnocked;
    int perfectFrames;
    float averagePinsPerFrame;
    int framesCompleted;
} GameStats_St;

typedef struct {
    bool showEndScreen;
    Rectangle playAgainBtn;
    Rectangle quitBtn;
    bool playAgainHovered;
    bool quitHovered;
} EndScreenState_St;

typedef struct {
    float x, y;          // screen position
    float vx, vy;        // velocity
    float rot, rotSpeed; // rotation angle (deg) + speed
    float w, h;          // size
    Color color;
    float life, maxLife;
} Confetti_St;

typedef struct {
    bool  active;
    char  text[32];
    Color color;
    float timer;
    float maxTimer;
    float scale;
    float alpha;
    float x, y;
} ScoreAnim_St;

// ─────────────────────────────────────────────────
// Pin dimensions (real proportions)
// ─────────────────────────────────────────────────
#define PIN_BODY_HEIGHT  0.278f  // hauteur corps (38.1cm total)
#define PIN_BASE_RADIUS  0.060f  // rayon base (diam.max 12cm)
#define PIN_NECK_RADIUS  0.025f
#define PIN_HEAD_RADIUS  0.044f

// ─────────────────────────────────────────────────
// Main game struct (opaque outside this file)
// ─────────────────────────────────────────────────
struct BowlingGame_St {
    BaseGame_St base;

    CameraState_St  cameraState;
    Ball_St         ball;
    Pin_St          pins[NUM_PINS];
    Frame_St        frames[BOWLING_MAX_FRAMES];
    Particle_St     particles[MAX_PARTICLES];
    int             particleCount;

    int   currentFrame;
    int   totalScore;
    bool  isAiming;
    Vector2 aimStart;
    Vector2 aimCurrent;
    float power;
    float spin;
    float aimAngle;
    float resetTimer;
    bool  waitingForReset;

    BowlingTextures_St textures;
    int  currentBallTextureIndex;

    GameStats_St     stats;
    EndScreenState_St endScreen;

    // Confetti
    Confetti_St confetti[MAX_CONFETTI];
    int         confettiCount;

    // Score event text animation
    ScoreAnim_St scoreAnim;

    // Results reveal animation
    float resultsRevealTimer;

    // Audience reaction
    float audienceReactionTimer;
    int   audienceReactionType;   // 0=none 1=strike 2=spare

    // Settings chosen on title screen
    bool bumpers;
    bool showAimGuide;
    int  selectedSkin;

    TitleScreen_St titleScreen;

    // Time accumulator for ball shimmer
    float timeAccum;
};

// ─────────────────────────────────────────────────
// Forward declarations
// ─────────────────────────────────────────────────
// ─────────────────────────────────────────────────
// Forward declarations
// ─────────────────────────────────────────────────
static void bowling_initTitleScreen(TitleScreen_St* ts);

Error_Et bowling_freeGameWrapper(void* game) {
    return bowling_freeGame((BowlingGame_St**)&game);
}

// ─────────────────────────────────────────────────
// Camera
// ─────────────────────────────────────────────────
static void bowling_initCamera(BowlingGame_St* game) {
    // Caméra derrière le joueur, vue sur toute la piste (18.29 m)
    game->cameraState.defaultCamera.position   = (Vector3){0.0f, 1.5f, 5.8f};
    game->cameraState.defaultCamera.target     = (Vector3){0.0f, 0.3f, -10.0f};
    game->cameraState.defaultCamera.up         = (Vector3){0.0f, 1.0f, 0.0f};
    game->cameraState.defaultCamera.fovy       = 62.0f;
    game->cameraState.defaultCamera.projection = CAMERA_PERSPECTIVE;
    game->cameraState.camera        = game->cameraState.defaultCamera;
    game->cameraState.followingBall = false;
    game->cameraState.cameraLerpSpeed = 4.0f;
    // Offset caméra de suivi : un peu derrière et au-dessus de la balle
    game->cameraState.offset = (Vector3){0.0f, 0.6f, 1.5f};
}

static void bowling_updateCamera(BowlingGame_St* game, float deltaTime) {
    CameraState_St* cam = &game->cameraState;
    if (game->ball.isRolling) {
        cam->followingBall = true;
        Vector3 tp = Vector3Add(game->ball.position, cam->offset);
        tp.y = fmaxf(tp.y, 2.0f);
        cam->camera.position = Vector3Lerp(cam->camera.position, tp, cam->cameraLerpSpeed * deltaTime);
        cam->camera.target   = Vector3Lerp(cam->camera.target, game->ball.position, cam->cameraLerpSpeed * deltaTime);
    } else if (cam->followingBall) {
        cam->camera.position = Vector3Lerp(cam->camera.position, cam->defaultCamera.position, cam->cameraLerpSpeed * deltaTime);
        cam->camera.target   = Vector3Lerp(cam->camera.target,   cam->defaultCamera.target,   cam->cameraLerpSpeed * deltaTime);
        if (Vector3Distance(cam->camera.position, cam->defaultCamera.position) < 0.1f) {
            cam->followingBall = false;
            cam->camera        = cam->defaultCamera;
        }
    }
}

// ─────────────────────────────────────────────────
// Scoring helpers
// ─────────────────────────────────────────────────
static int bowling_countKnockedPins(BowlingGame_St* game) {
    int c = 0;
    for (int i = 0; i < NUM_PINS; i++) if (!game->pins[i].isStanding) c++;
    return c;
}

static void bowling_updateTotalScore(BowlingGame_St* game) {
    int total = 0;
    for (int i = 0; i < BOWLING_MAX_FRAMES; i++) {
        Frame_St* f = &game->frames[i];
        if (f->numRolls == 0) break;
        int s = 0;
        if (i < BOWLING_MAX_FRAMES - 1) {
            if (f->isStrike) {
                s = 10;
                Frame_St* n = &game->frames[i + 1];
                if (n->numRolls >= 1) s += n->rolls[0];
                if (n->numRolls >= 2) s += n->rolls[1];
                else if (n->isStrike && i + 2 < BOWLING_MAX_FRAMES) s += game->frames[i+2].rolls[0];
            } else if (f->isSpare) {
                s = 10;
                if (i + 1 < BOWLING_MAX_FRAMES && game->frames[i+1].numRolls >= 1)
                    s += game->frames[i+1].rolls[0];
            } else {
                s = f->rolls[0] + (f->numRolls > 1 ? f->rolls[1] : 0);
            }
        } else {
            for (int r = 0; r < f->numRolls; r++) s += f->rolls[r];
        }
        total += s;
        f->cumulativeScore = total;
    }
    game->totalScore = total;
}

static void bowling_updateGameStats(BowlingGame_St* game) {
    game->stats.framesCompleted = game->currentFrame + 1;
    if (game->stats.framesCompleted > 0)
        game->stats.averagePinsPerFrame = (float)game->stats.totalPinsKnocked / game->stats.framesCompleted;
}

// ─────────────────────────────────────────────────
// Confetti helpers
// ─────────────────────────────────────────────────
static const Color CONFETTI_COLORS[] = {
    {255,  50,  50, 255}, {255, 200,  50, 255}, { 50, 200,  50, 255},
    { 50, 150, 255, 255}, {200,  50, 255, 255}, {255, 120,  30, 255},
    { 50, 230, 200, 255}, {255, 255,  80, 255}
};
#define NUM_CONFETTI_COLORS 8

static void bowling_spawnConfetti(BowlingGame_St* game, int amount) {
    for (int i = 0; i < amount && game->confettiCount < MAX_CONFETTI; i++) {
        Confetti_St* c = &game->confetti[game->confettiCount++];
        c->x         = (float)GetRandomValue(0, SCREEN_WIDTH);
        c->y         = (float)GetRandomValue(-60, -5);
        c->vx        = (float)GetRandomValue(-60, 60);
        c->vy        = (float)GetRandomValue(120, 280);
        c->rot       = (float)GetRandomValue(0, 360);
        c->rotSpeed  = (float)GetRandomValue(-300, 300);
        c->w         = (float)GetRandomValue(6, 14);
        c->h         = (float)GetRandomValue(4, 9);
        c->color     = CONFETTI_COLORS[GetRandomValue(0, NUM_CONFETTI_COLORS - 1)];
        c->life      = (float)GetRandomValue(1500, 3000) / 1000.0f;
        c->maxLife   = c->life;
    }
}

static void bowling_updateConfetti(BowlingGame_St* game, float dt) {
    int w = 0;
    for (int i = 0; i < game->confettiCount; i++) {
        Confetti_St* c = &game->confetti[i];
        c->life -= dt;
        if (c->life <= 0.0f) continue;
        c->x   += c->vx * dt;
        c->y   += c->vy * dt;
        c->vx  *= 0.99f;
        c->vy  += 60.0f * dt;   // gravity
        c->rot += c->rotSpeed * dt;
        if (i != w) game->confetti[w] = *c;
        w++;
    }
    game->confettiCount = w;
}

static void bowling_drawConfetti(BowlingGame_St* game) {
    for (int i = 0; i < game->confettiCount; i++) {
        Confetti_St* c = &game->confetti[i];
        float alpha = (c->life / c->maxLife);
        Color col   = c->color;
        col.a       = (unsigned char)(255 * alpha);
        DrawRectanglePro(
            (Rectangle){c->x, c->y, c->w, c->h},
            (Vector2){c->w * 0.5f, c->h * 0.5f},
            c->rot, col
        );
    }
}

// ─────────────────────────────────────────────────
// Score event animation
// ─────────────────────────────────────────────────
static void bowling_triggerScoreAnim(BowlingGame_St* game, const char* text, Color color) {
    ScoreAnim_St* a = &game->scoreAnim;
    a->active   = true;
    strncpy(a->text, text, 31);
    a->text[31] = '\0';
    a->color    = color;
    a->timer    = 2.2f;
    a->maxTimer = 2.2f;
    a->x        = (float)SCREEN_WIDTH  / 2.0f;
    a->y        = (float)SCREEN_HEIGHT / 2.0f - 60.0f;
}

static void bowling_updateScoreAnim(BowlingGame_St* game, float dt) {
    ScoreAnim_St* a = &game->scoreAnim;
    if (!a->active) return;
    a->timer -= dt;
    if (a->timer <= 0.0f) { a->active = false; return; }
    float t     = a->timer / a->maxTimer;          // 1→0
    float pulse = 0.85f + 0.35f * sinf(t * 20.0f); // oscillate
    a->scale = 2.8f * pulse * (t > 0.15f ? 1.0f : t / 0.15f);
    a->alpha = t > 0.2f ? 1.0f : t / 0.2f;
}

static void bowling_drawScoreAnim(BowlingGame_St* game) {
    ScoreAnim_St* a = &game->scoreAnim;
    if (!a->active) return;

    int   fs  = (int)(38 * a->scale);
    Color col = a->color;
    col.a     = (unsigned char)(255 * a->alpha);

    // Shadow
    Color shadow = (Color){0,0,0,(unsigned char)(180 * a->alpha)};
    int tw = MeasureText(a->text, fs);
    DrawText(a->text, (int)a->x - tw/2 + 3, (int)a->y + 3, fs, shadow);
    DrawText(a->text, (int)a->x - tw/2,     (int)a->y,     fs, col);
}

// ─────────────────────────────────────────────────
// Frame logic
// ─────────────────────────────────────────────────
static void bowling_nextFrame(BowlingGame_St* game) {
    physics_setupPins(game->pins);
    physics_resetBall(&game->ball);
    game->waitingForReset = false;
    game->resetTimer      = 0.0f;
    if (game->textures.ballTextureCount > 0)
        game->currentBallTextureIndex = GetRandomValue(0, game->textures.ballTextureCount - 1);
    game->currentFrame++;
    if (game->currentFrame >= BOWLING_MAX_FRAMES) {
        game->endScreen.showEndScreen  = true;
        game->resultsRevealTimer       = 0.0f;
        game->endScreen.playAgainBtn   = (Rectangle){SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 110, 200, 40};
        game->endScreen.quitBtn        = (Rectangle){SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 160, 200, 40};
    }
}

static void bowling_resetBallState(BowlingGame_St* game) {
    physics_resetBall(&game->ball);
    game->waitingForReset = false;
    game->resetTimer      = 0.0f;
    game->power           = 0.5f;
    game->spin            = 0.0f;
    game->aimAngle        = 0.0f;
}

static void bowling_handleBallStopped(BowlingGame_St* game) {
    game->waitingForReset = true;
    game->resetTimer      = 1.8f;

    // Safety guard — should never be true, but avoid out-of-bounds
    if (game->currentFrame >= BOWLING_MAX_FRAMES) return;

    bool wasGutter   = physics_isGutterBall(&game->ball, LANE_WIDTH, GUTTER_WIDTH);
    int  knockedPins = bowling_countKnockedPins(game);
    game->stats.totalPinsKnocked += knockedPins;

    Frame_St* frame = &game->frames[game->currentFrame];
    int pinsThisRoll = (frame->numRolls == 0) ? knockedPins : knockedPins - frame->rolls[0];
    if (frame->numRolls == 0 && wasGutter && knockedPins == 0) game->stats.gutterBalls++;

    if (frame->numRolls < 3) {
        frame->rolls[frame->numRolls] = pinsThisRoll;
    }
    if (frame->numRolls == 0) frame->pinsKnockedFirstRoll = knockedPins;
    frame->numRolls++;

    if (frame->numRolls == 1 && pinsThisRoll == NUM_PINS) {
        frame->isStrike = true;
        frame->score    = 10;
        game->stats.totalStrikes++;
        physics_spawnParticles(game->particles, &game->particleCount, (Vector3){0,1,-54}, 25, (Color){255,215,0,255});
        bowling_spawnConfetti(game, 180);
        bowling_triggerScoreAnim(game, "STRIKE!", (Color){255,70,30,255});
        game->audienceReactionTimer = 3.5f;
        game->audienceReactionType  = 1;
    } else if (frame->numRolls == 2 && (knockedPins == NUM_PINS) && !frame->isStrike) {
        frame->isSpare = true;
        frame->score   = 10;
        game->stats.totalSpares++;
        physics_spawnParticles(game->particles, &game->particleCount, (Vector3){0,1,-54}, 18, (Color){100,200,255,255});
        bowling_spawnConfetti(game, 90);
        bowling_triggerScoreAnim(game, "SPARE!", (Color){80,180,255,255});
        game->audienceReactionTimer = 2.5f;
        game->audienceReactionType  = 2;
    } else if (frame->numRolls >= 2) {
        frame->score = pinsThisRoll;
        if (pinsThisRoll == 0 && wasGutter) {
            bowling_triggerScoreAnim(game, "GUTTER", (Color){180,180,180,255});
        } else if (pinsThisRoll > 0) {
            char buf[16];
            snprintf(buf, sizeof(buf), "%d pins", pinsThisRoll);
            bowling_triggerScoreAnim(game, buf, (Color){220,200,140,255});
        }
    }

    bowling_updateTotalScore(game);
    bowling_updateGameStats(game);
}

static void bowling_processResetTimer(BowlingGame_St* game, float deltaTime) {
    if (!game->waitingForReset) return;
    game->resetTimer -= deltaTime;
    if (game->resetTimer > 0.0f) return;

    // Guard: should never happen, but avoid out-of-bounds if somehow called late
    if (game->currentFrame >= BOWLING_MAX_FRAMES) {
        game->waitingForReset = false;
        return;
    }

    Frame_St* frame     = &game->frames[game->currentFrame];
    bool isTenthFrame   = (game->currentFrame == BOWLING_MAX_FRAMES - 1);
    bool needsThirdRoll = false;

    if (isTenthFrame && (frame->isStrike || frame->isSpare) && frame->numRolls < 3) {
        needsThirdRoll = true;
        // Reset pins for bonus roll after a strike
        if (frame->numRolls == 1 && frame->isStrike) physics_setupPins(game->pins);
        if (frame->numRolls == 2 && frame->isStrike) physics_setupPins(game->pins);
    }

    // Only advance to next frame when all required rolls are done
    if (!needsThirdRoll && (frame->isStrike || frame->numRolls >= 2)) {
        if (frame->isStrike && !isTenthFrame) game->stats.perfectFrames++;
        bowling_nextFrame(game);
    } else {
        bowling_resetBallState(game);
    }
}

// ─────────────────────────────────────────────────
// Draw: Environment (audience, neon, decor)
// ─────────────────────────────────────────────────
static void bowling_drawEnvironment(BowlingGame_St* game) {
    // Echelle 1 unite = 1 metre. Piste : z=+4.57 -> z=-20.5
    float laneHalfLen = (LANE_LENGTH + APPROACH_LEN) * 0.5f;
    float envCenZ     = APPROACH_LEN * 0.5f - laneHalfLen;

    // ── Sol ───────────────────────────────────────
    DrawPlane((Vector3){0.0f, -0.01f, envCenZ}, (Vector2){30.0f, 60.0f},
              (Color){50, 40, 32, 255});
    DrawPlane((Vector3){-5.5f, 0.0f, envCenZ}, (Vector2){7.0f, 60.0f},
              (Color){62, 52, 42, 255});
    DrawPlane((Vector3){ 5.5f, 0.0f, envCenZ}, (Vector2){7.0f, 60.0f},
              (Color){62, 52, 42, 255});

    // ── Mur arrière (arrêt visuel de la piste) ──
    DrawCube((Vector3){0.0f, 0.6f, BOWLING_WALL_Z - 0.2f},
             8.0f, 1.2f, 0.3f, (Color){52, 43, 33, 255});
    DrawPlane((Vector3){0.0f, 0.0f, BOWLING_WALL_Z - 0.5f},
              (Vector2){8.0f, 1.0f}, (Color){42, 34, 26, 255});

    // ── Spectateurs (cones + tete) ────────────────
    static const Color CONE_COLORS[] = {
        {220, 50,  50, 255}, { 50,130,220, 255}, { 50,190, 70, 255},
        {230,160,  0, 255},  {170, 50,210, 255}, {220, 90, 20, 255},
        { 20,200,190, 255},  {240,220, 30, 255}, {200, 80,130, 255},
        { 80,200, 80, 255},  {255,100,  0, 255}, { 30,150,255, 255}
    };
    static const Color HEAD_COLORS[] = {
        {240,190,140,255},{210,160,110,255},{170,120, 80,255},{130, 90, 55,255}
    };
    #define N_CONE_C 12
    #define N_HEAD_C  4

    float bounce = 0.0f;
    if (game->audienceReactionTimer > 0.0f) {
        float maxT = (game->audienceReactionType == 1) ? 3.5f : 2.5f;
        float t    = game->audienceReactionTimer / maxT;
        bounce     = 0.18f * t * fabsf(sinf(game->timeAccum * 12.0f));
    }

    float coneH = 0.55f;
    float headR = 0.12f;

    for (int side = -1; side <= 1; side += 2) {
        for (int row = 0; row < 3; row++) {
            float xBase = (float)side * (2.2f + row * 0.65f);
            for (int p = 0; p < 22; p++) {
                float z     = APPROACH_LEN - (float)p * 1.2f;
                int   ci    = (p * 3 + row * 7 + (side > 0 ? 30 : 0)) % N_CONE_C;
                int   hi    = (p + row * 5) % N_HEAD_C;
                float phase = (float)((p * 37 + row * 13) % 100) / 100.0f;
                float bob   = bounce * fabsf(sinf(game->timeAccum * 12.0f + phase * 6.28f));

                DrawCylinderEx(
                    (Vector3){xBase, bob,          z},
                    (Vector3){xBase, bob + coneH,  z},
                    0.14f, 0.0f, 7, CONE_COLORS[ci]);
                DrawSphere(
                    (Vector3){xBase, bob + coneH + headR, z},
                    headR, HEAD_COLORS[hi]);
            }
        }
    }

    // Rangee arriere derriere le mur
    for (int p = -6; p <= 6; p++) {
        float x     = (float)p * 0.80f;
        int   ci    = ((p + 6) * 5) % N_CONE_C;
        int   hi    = (p + 6) % N_HEAD_C;
        float phase = (float)(((p+6) * 41) % 100) / 100.0f;
        float bob   = bounce * fabsf(sinf(game->timeAccum * 12.0f + phase * 6.28f));
        DrawCylinderEx(
            (Vector3){x, bob,         BOWLING_WALL_Z - 0.8f},
            (Vector3){x, bob + coneH, BOWLING_WALL_Z - 0.8f},
            0.14f, 0.0f, 7, CONE_COLORS[ci]);
        DrawSphere(
            (Vector3){x, bob + coneH + headR, BOWLING_WALL_Z - 0.8f},
            headR, HEAD_COLORS[hi]);
    }

    // ── Colonnes de soutien ───────────────────────
    float colZs[4] = {2.5f, -5.0f, -12.0f, -18.0f};
    for (int i = 0; i < 4; i++) {
        DrawCylinder((Vector3){-3.8f, 1.2f, colZs[i]}, 0.10f, 0.10f, 2.4f, 8, (Color){85,70,55,255});
        DrawCylinder((Vector3){ 3.8f, 1.2f, colZs[i]}, 0.10f, 0.10f, 2.4f, 8, (Color){85,70,55,255});
    }

    // ── Neons plafond ─────────────────────────────
    Color neonBlue  = {40, 120, 255, 220};
    for (int i = 0; i < 5; i++) {
        float z = APPROACH_LEN - (float)i * 5.0f;
        DrawCylinder((Vector3){-2.5f, 3.2f, z}, 0.025f, 0.025f, 0.12f, 6, neonBlue);
        DrawCylinder((Vector3){ 2.5f, 3.2f, z}, 0.025f, 0.025f, 0.12f, 6, neonBlue);
    }
    // Panneau d affichage central
    // (panneau d'affichage central supprimé)

    // ── Rails de délimitation piste / tribunes (le long de Z) ────
    float hw = LANE_WIDTH / 2.0f + GUTTER_WIDTH + 0.04f;
    DrawCylinderEx(
        (Vector3){-hw, 0.08f, APPROACH_LEN},
        (Vector3){-hw, 0.08f, -(LANE_LENGTH + 0.5f)},
        0.018f, 0.018f, 4, (Color){100,82,65,255});
    DrawCylinderEx(
        (Vector3){ hw, 0.08f, APPROACH_LEN},
        (Vector3){ hw, 0.08f, -(LANE_LENGTH + 0.5f)},
        0.018f, 0.018f, 4, (Color){100,82,65,255});
}

// ─────────────────────────────────────────────────
// Draw: Lane (thin, with separated pit)
// ─────────────────────────────────────────────────
static void bowling_drawLane(BowlingGame_St* game) {
    float laneWidth = LANE_WIDTH;
    float gutterW   = GUTTER_WIDTH;

    // Coordonnées Z officielles (z=0 = ligne de faute)
    float laneStart = APPROACH_LEN;           // +4.57 m (début zone d'élan)
    float laneEnd   = -(LANE_LENGTH + 0.5f);  // ~-18.79 m (derrière la quille de tête)
    float pitEnd    = laneEnd - 1.5f;         // fond de fosse
    float laneLen   = laneStart - laneEnd;
    float laneCenZ  = (laneStart + laneEnd) / 2.0f;
    float pitLen    = laneEnd - pitEnd;
    float pitCenZ   = (laneEnd + pitEnd) / 2.0f;

    // ── Surface principale ────────────────────────
    DrawPlane((Vector3){0.0f, 0.0f, laneCenZ}, (Vector2){laneWidth, laneLen},
              (Color){182, 143, 102, 255});

    // Zone d'élan (bois plus sombre) — dessinée en premier, y=0.002 pour couvrir la piste
    DrawPlane((Vector3){0.0f, 0.002f, laneStart - APPROACH_LEN/2},
              (Vector2){laneWidth, APPROACH_LEN}, (Color){128, 88, 52, 255});

    // Points de visée (z = -2.13 m / 7 pieds)
    for (int i = -1; i <= 1; i++) {
        if (i == 0) continue;
        float x = (float)i * (laneWidth / 3.5f) * 0.45f;
        DrawSphere((Vector3){x, 0.005f, -2.13f}, 0.015f, (Color){200, 160, 90, 255});
    }

    // Flèches (z = -4.57 m / 15 pieds)
    for (int i = -2; i <= 2; i++) {
        if (i == 0) continue;
        float x = (float)i * (laneWidth / 5.0f) * 0.58f;
        DrawPlane((Vector3){x, 0.005f, -4.57f}, (Vector2){0.04f, 0.20f},
                  (Color){218, 165, 82, 255});
    }

    // Ligne de faute (z = 0, rouge vif)
    DrawPlane((Vector3){0.0f, 0.007f, 0.0f}, (Vector2){laneWidth + gutterW*2, 0.05f},
              (Color){220, 35, 35, 255});

    // ── Gouttières (23 cm chacune) ────────────────
    float gx = laneWidth/2.0f + gutterW/2.0f;
    DrawPlane((Vector3){-gx, 0.03f, laneCenZ}, (Vector2){gutterW, laneLen}, (Color){35, 25, 15, 255});
    DrawPlane((Vector3){ gx, 0.03f, laneCenZ}, (Vector2){gutterW, laneLen}, (Color){35, 25, 15, 255});

    // ── Séparateur piste / fosse ──────────────────
    DrawPlane((Vector3){0.0f, 0.04f, laneEnd},
              (Vector2){laneWidth + gutterW * 2 + 0.1f, 0.08f}, (Color){55, 40, 25, 255});

    // ── Fosse (pin deck) ──────────────────────────
    DrawPlane((Vector3){0.0f, -0.01f, pitCenZ},
              (Vector2){laneWidth, pitLen}, (Color){150, 112, 74, 255});
    DrawPlane((Vector3){-gx, 0.02f, pitCenZ}, (Vector2){gutterW, pitLen}, (Color){28, 20, 12, 255});
    DrawPlane((Vector3){ gx, 0.02f, pitCenZ}, (Vector2){gutterW, pitLen}, (Color){28, 20, 12, 255});

    // Mur arrière de la fosse
    float halfTot = laneWidth/2.0f + gutterW;
    DrawCylinder((Vector3){-halfTot, 0.3f, pitEnd}, 0.02f, 0.02f, 0.6f, 6, (Color){70,55,40,255});
    DrawCylinder((Vector3){ halfTot, 0.3f, pitEnd}, 0.02f, 0.02f, 0.6f, 6, (Color){70,55,40,255});
    DrawPlane((Vector3){0.0f, 0.03f, pitEnd},
              (Vector2){laneWidth + gutterW * 2 + 0.1f, 0.06f}, (Color){50, 38, 24, 255});

    // ── Barrières (bumpers) visuelles ────────────
    if (game->bumpers) {
        float bumpX_L = -laneWidth / 2.0f;
        float bumpX_R =  laneWidth / 2.0f;
        float bumpH   = 0.12f;   // hauteur des poteaux
        float bumpR   = 0.03f;
        float railR   = 0.018f;
        Color bumpCol = {255, 200, 40, 255};

        // Poteaux verticaux aux 4 coins
        DrawCylinder((Vector3){bumpX_L, 0.0f, laneStart}, bumpR, bumpR, bumpH, 6, bumpCol);
        DrawCylinder((Vector3){bumpX_L, 0.0f, laneEnd},   bumpR, bumpR, bumpH, 6, bumpCol);
        DrawCylinder((Vector3){bumpX_R, 0.0f, laneStart}, bumpR, bumpR, bumpH, 6, bumpCol);
        DrawCylinder((Vector3){bumpX_R, 0.0f, laneEnd},   bumpR, bumpR, bumpH, 6, bumpCol);

        // Rails horizontaux LE LONG DE Z (DrawCylinderEx : startPos → endPos)
        float railY = bumpH * 0.5f;
        DrawCylinderEx(
            (Vector3){bumpX_L, railY, laneStart},
            (Vector3){bumpX_L, railY, laneEnd},
            railR, railR, 4, bumpCol);
        DrawCylinderEx(
            (Vector3){bumpX_R, railY, laneStart},
            (Vector3){bumpX_R, railY, laneEnd},
            railR, railR, 4, bumpCol);
    }
}

// ─────────────────────────────────────────────────
// Draw: Shadows
// ─────────────────────────────────────────────────
static void bowling_drawBallShadow(BowlingGame_St* game) {
    Ball_St* b = &game->ball;
    if (b->position.y > 0.8f) return;

    // Ombre douce : ellipse aplatie sur le sol, s'estompe en hauteur
    float   height   = b->position.y - b->radius;
    float   spread   = 1.0f + height * 2.0f;  // s'étire quand la balle monte
    float   alpha    = fmaxf(0.0f, 1.0f - height * 3.0f) * 0.55f;
    float   rx       = b->radius * spread;
    float   rz       = b->radius * spread * 0.55f; // un peu aplatie
    Vector3 center   = {b->position.x, 0.002f, b->position.z};

    // Dessin via 3 cercles concentriques pour simuler un dégradé
    DrawCircle3D(center, rx,        (Vector3){1,0,0}, 90.0f, (Color){0,0,0,(unsigned char)(alpha*120)});
    DrawCircle3D(center, rx*0.65f,  (Vector3){1,0,0}, 90.0f, (Color){0,0,0,(unsigned char)(alpha*160)});
    DrawCircle3D(center, rx*0.30f,  (Vector3){1,0,0}, 90.0f, (Color){0,0,0,(unsigned char)(alpha*200)});
    (void)rz;
}

static void bowling_drawPinShadows(BowlingGame_St* game) {
    for (int i = 0; i < NUM_PINS; i++) {
        Pin_St* p = &game->pins[i];
        // Quille debout : ombre petite et nette sous la base
        // Quille tombée : ombre allongée
        float rot   = p->rotationAngle;   // 0 = debout, PI/2 = couché
        float tRot  = rot / (PI / 2.0f);  // 0→1
        float baseR = PIN_BASE_RADIUS;
        float rx    = baseR * (1.0f + tRot * 2.5f);  // s'allonge en tombant
        float alpha = p->isStanding ? 0.50f : (0.25f + tRot * 0.1f);

        Vector3 sp = {p->position.x, 0.002f, p->position.z};
        DrawCircle3D(sp, rx,        (Vector3){1,0,0}, 90.0f, (Color){0,0,0,(unsigned char)(alpha*140)});
        DrawCircle3D(sp, rx*0.55f,  (Vector3){1,0,0}, 90.0f, (Color){0,0,0,(unsigned char)(alpha*200)});
    }
}

// ─────────────────────────────────────────────────
// Draw: Pins (real 3D with fall rotation)
// ─────────────────────────────────────────────────
static void bowling_drawSinglePin(const Pin_St* pin, Color color) {
    Vector3 localUp = Vector3RotateByAxisAngle(
        (Vector3){0.0f, 1.0f, 0.0f},
        pin->rotationAxis,
        pin->rotationAngle
    );
    localUp = Vector3Normalize(localUp);

    float   hb      = PIN_BODY_HEIGHT * 0.5f;
    Vector3 base    = Vector3Subtract(pin->position, Vector3Scale(localUp, hb));
    Vector3 top     = Vector3Add(pin->position,      Vector3Scale(localUp, hb));

    if (base.y < 0.0f) {
        float shift = -base.y; base.y = 0.0f; top.y += shift;
    }

    DrawCylinderEx(base, top, PIN_BASE_RADIUS, PIN_NECK_RADIUS, 10, color);
    Vector3 headCenter = Vector3Add(top, Vector3Scale(localUp, PIN_HEAD_RADIUS));
    DrawSphere(headCenter, PIN_HEAD_RADIUS, color);
}

static void bowling_drawPins(BowlingGame_St* game) {
    for (int i = 0; i < NUM_PINS; i++) {
        Pin_St* p = &game->pins[i];
        Color c;
        if (p->isStanding) {
            c = (Color){255, 250, 240, 255};
        } else {
            float fade = fmaxf(0.55f, 1.0f - p->fallTime * 0.10f);
            c = (Color){
                (unsigned char)(228 * fade),
                (unsigned char)(218 * fade),
                (unsigned char)(206 * fade),
                255
            };
        }
        bowling_drawSinglePin(p, c);
    }
}

// ─────────────────────────────────────────────────
// Draw: Ball (real 3D + shine effect)
// ─────────────────────────────────────────────────
static void bowling_drawBall(BowlingGame_St* game) {
    Ball_St* b    = &game->ball;
    float    r    = b->radius;
    int      skin = game->selectedSkin;

    if (skin < game->textures.ballModelCount
        && IsTextureValid(game->textures.ballTextures[skin])) {
        // Texture entièrement opaque (pixels transparents remplis à l'extraction)
        DrawModelEx(
            game->textures.ballModels[skin],
            b->position,
            b->rollAxis,
            b->visualRotation,
            (Vector3){r, r, r},
            WHITE
        );
    } else {
        DrawSphere(b->position, r, BALL_SKIN_BODY[skin % NUM_BALL_SKINS]);
    }

    // Reflet spéculaire
    float   shimmer  = 0.5f + 0.5f * sinf(game->timeAccum * 4.0f);
    Vector3 toCamera = Vector3Normalize(Vector3Subtract(game->cameraState.camera.position, b->position));
    Vector3 hlPos    = Vector3Add(b->position, Vector3Scale(toCamera, r * 0.62f));
    DrawSphere(hlPos, r * 0.18f, (Color){255, 255, 255, (unsigned char)(45 + 35 * shimmer)});
}

// ─────────────────────────────────────────────────
// Draw: Particles
// ─────────────────────────────────────────────────
static void bowling_drawParticles(BowlingGame_St* game) {
    for (int i = 0; i < game->particleCount; i++) {
        Particle_St* p     = &game->particles[i];
        float        alpha = (p->life / p->maxLife) * 255.0f;
        Color c = p->color; c.a = (unsigned char)alpha;
        Vector2 sp = GetWorldToScreen(p->position, game->cameraState.camera);
        if (sp.x > 0 && sp.x < SCREEN_WIDTH && sp.y > 0 && sp.y < SCREEN_HEIGHT)
            DrawCircle((int)sp.x, (int)sp.y, p->size * 20.0f * (p->life / p->maxLife), c);
    }
}

// ─────────────────────────────────────────────────
// Draw: Aim guide
// ─────────────────────────────────────────────────
static void bowling_drawAimGuide(BowlingGame_St* game) {
    if (!game->showAimGuide) return;
    if (game->ball.isRolling || game->waitingForReset) return;
    float angleRad = game->aimAngle * DEG2RAD;
    Vector3 dir = Vector3Normalize((Vector3){-sinf(angleRad), 0.0f, -cosf(angleRad)});
    Vector3 pos = game->ball.position;
    for (int i = 0; i < 25; i++) {
        Vector3 end = Vector3Add(pos, Vector3Scale(dir, 0.8f));
        float a = (float)(200 - i * 7); if (a < 20) a = 20;
        DrawLine3D(pos, end, (Color){255,255,100,(unsigned char)a});
        pos = end;
    }
    Vector3 tp = Vector3Add(game->ball.position, Vector3Scale(dir, 15.0f));
    DrawCircle3D(tp, 0.25f, (Vector3){1,0,0}, 90.0f, (Color){255,100,100,150});
    DrawCircle3D(tp, 0.35f, (Vector3){1,0,0}, 90.0f, (Color){255,100,100,80});
    float se = game->spin * 0.5f;
    for (int s = 5; s < 20; s += 5) {
        Vector3 cp = Vector3Add(game->ball.position, Vector3Scale(dir, (float)s));
        cp.x += se * s * 0.02f;
        DrawCircle3D(cp, 0.1f, (Vector3){1,0,0}, 90.0f, (Color){100,150,255,100});
    }
}

// ─────────────────────────────────────────────────
// Draw: HUD – Scoreboard
// ─────────────────────────────────────────────────
static void bowling_drawFullScoreboard(BowlingGame_St* game) {
    int fw = 70, sx = 20, sy = 10, bh = 65;
    DrawRectangle(sx-5, sy-5, fw*10+15, bh+15, (Color){20,15,10,230});
    DrawRectangleLines(sx-5, sy-5, fw*10+15, bh+15, (Color){139,90,43,255});

    for (int i = 0; i < 10; i++) {
        int     x = sx + i * fw;
        Color   bg = (i == game->currentFrame) ? (Color){60,50,30,255} : (Color){40,35,25,255};
        DrawRectangle(x, sy, fw-2, bh, bg);
        DrawRectangleLines(x, sy, fw-2, bh, (Color){100,70,40,255});
        DrawText(TextFormat("%d", i+1), x + fw/2 - 5, sy+2, 14, (Color){180,160,140,255});

        Frame_St* f  = &game->frames[i];
        int       ry = sy + 20, rbs = 20;

        if (f->numRolls > 0 || i < game->currentFrame) {
            if (i == 9) {
                for (int r = 0; r < 3; r++) {
                    int rx = x + 5 + r * 22;
                    DrawRectangle(rx, ry, rbs, rbs, (Color){50,45,35,255});
                    if (r < f->numRolls) {
                        const char* t = ""; Color tc = WHITE;
                        if (f->rolls[r] == 10) {
                            t = "X"; tc = (Color){255,50,50,255};
                        } else if (r == 1 && f->rolls[0] + f->rolls[1] == 10 && f->rolls[0] != 10) {
                            t = "/"; tc = (Color){50,150,255,255};
                        } else if (f->rolls[r] == 0) { t = "-"; tc = GRAY; }
                        else { t = TextFormat("%d", f->rolls[r]); }
                        DrawText(t, rx+5, ry+3, 14, tc);
                    }
                }
            } else {
                int r1x = x+5, r2x = x + fw - 27;
                DrawRectangle(r1x, ry, rbs, rbs, (Color){50,45,35,255});
                DrawRectangle(r2x, ry, rbs, rbs, (Color){50,45,35,255});
                if (f->isStrike) {
                    DrawText("X",  r1x+5, ry+3, 14, (Color){255,50,50,255});
                    DrawText("-",  r2x+7, ry+3, 14, GRAY);
                } else {
                    if (f->numRolls >= 1) {
                        if (f->rolls[0] == 0) DrawText("-", r1x+7, ry+3, 14, GRAY);
                        else DrawText(TextFormat("%d", f->rolls[0]), r1x+5, ry+3, 14, WHITE);
                    }
                    if (f->numRolls >= 2) {
                        if (f->isSpare) DrawText("/", r2x+5, ry+3, 14, (Color){50,150,255,255});
                        else if (f->rolls[1] == 0) DrawText("-", r2x+7, ry+3, 14, GRAY);
                        else DrawText(TextFormat("%d", f->rolls[1]), r2x+5, ry+3, 14, WHITE);
                    }
                }
            }
        }

        if (f->cumulativeScore > 0 || i < game->currentFrame)
            DrawText(TextFormat("%d", f->cumulativeScore), x + fw/2 - 15, sy+45, 12, (Color){100,255,100,255});
    }
    DrawText(TextFormat("Total: %d", game->totalScore), sx + fw*10 - 60, sy + bh + 5, 18, (Color){255,215,0,255});
}

// ─────────────────────────────────────────────────
// Draw: HUD – Power/Spin meters
// ─────────────────────────────────────────────────
static void bowling_drawPowerMeter(BowlingGame_St* game) {
    if (game->ball.isRolling || game->waitingForReset) return;
    int mx = SCREEN_WIDTH - 60, my = 150, mw = 30, mh = 200;
    DrawRectangle(mx-5, my-25, mw+10, mh+50, (Color){30,25,20,220});
    DrawRectangleLines(mx-5, my-25, mw+10, mh+50, (Color){139,90,43,255});
    DrawText("POWER", mx-5, my-22, 12, (Color){200,180,160,255});
    DrawRectangle(mx, my, mw, mh, (Color){50,45,35,255});
    float pr    = game->power;
    int   fh    = (int)(pr * mh);
    Color pc    = pr < 0.33f ? (Color){100,255,100,255} : pr < 0.66f ? (Color){255,255,100,255} : (Color){255,100,100,255};
    DrawRectangle(mx, my + mh - fh, mw, fh, pc);
    DrawRectangleLines(mx, my, mw, mh, WHITE);
    DrawText(TextFormat("%.0f%%", pr * 100), mx, my+mh+5, 12, WHITE);
}

static void bowling_drawSpinMeter(BowlingGame_St* game) {
    if (game->ball.isRolling || game->waitingForReset) return;
    int mx = SCREEN_WIDTH - 130, my = 150, mw = 60, mh = 25;
    DrawRectangle(mx-5, my-25, mw+10, mh+20, (Color){30,25,20,220});
    DrawRectangleLines(mx-5, my-25, mw+10, mh+20, (Color){139,90,43,255});
    DrawText("SPIN", mx+10, my-22, 12, (Color){200,180,160,255});
    DrawRectangle(mx, my, mw, mh, (Color){50,45,35,255});
    DrawRectangleLines(mx, my, mw, mh, WHITE);
    int cx = mx + mw/2;
    DrawLine(cx, my, cx, my+mh, (Color){150,150,150,255});
    int ix    = cx + (int)(game->spin / 1.0f * mw / 2);
    float sa  = fabsf(game->spin);
    Color sc  = sa < 0.3f ? WHITE : sa < 0.7f ? (Color){100,200,255,255} : (Color){50,150,255,255};
    DrawCircle(ix, my + mh/2, 8, sc);
    const char* st = game->spin < -0.3f ? "LEFT" : (game->spin > 0.3f ? "RIGHT" : "STRAIGHT");
    DrawText(st, mx, my+mh+25, 10, LIGHTGRAY);
}

static void bowling_drawStatsPanel(BowlingGame_St* game) {
    int px = SCREEN_WIDTH - 150, py = 400, pw = 140, ph = 120;
    DrawRectangle(px, py, pw, ph, (Color){30,25,20,200});
    DrawRectangleLines(px, py, pw, ph, (Color){100,70,40,255});
    DrawText("Stats", px+50, py+5, 14, (Color){255,215,0,255});
    int y = py + 25;
    DrawText(TextFormat("Strikes: %d", game->stats.totalStrikes),  px+10, y, 12, (Color){255,100,100,255}); y+=18;
    DrawText(TextFormat("Spares:  %d", game->stats.totalSpares),   px+10, y, 12, (Color){100,150,255,255}); y+=18;
    DrawText(TextFormat("Gutters: %d", game->stats.gutterBalls),   px+10, y, 12, (Color){255,150,50, 255}); y+=18;
    DrawText(TextFormat("Pins:    %d", game->stats.totalPinsKnocked), px+10, y, 12, LIGHTGRAY); y+=18;
    int standing = 0;
    for (int i = 0; i < NUM_PINS; i++) if (game->pins[i].isStanding) standing++;
    DrawText(TextFormat("Up: %d", standing), px+10, y, 12, standing == 0 ? (Color){100,255,100,255} : LIGHTGRAY);
}

static void bowling_drawControls(BowlingGame_St* game) {
    (void)game;
    int y = SCREEN_HEIGHT - 80;
    DrawRectangle(10, y-5, 360, 75, (Color){20,15,10,200});
    DrawRectangleLines(10, y-5, 360, 75, (Color){100,70,40,255});
    DrawText("Controls:", 20, y, 14, (Color){255,215,0,255});   y+=18;
    DrawText("Click & Drag: Aim and throw",                20, y, 12, (Color){180,160,140,255}); y+=15;
    DrawText("A/D: Spin  |  Scroll: Power  |  Arrows: Aim", 20, y, 12, (Color){180,160,140,255}); y+=15;
    DrawText("R: Reset ball  |  SPACE: Skip  |  ESC: Quit",  20, y, 12, (Color){180,160,140,255});
}

// ─────────────────────────────────────────────────
// Draw: End screen (animated results)
// ─────────────────────────────────────────────────
static void bowling_drawEndScreen(BowlingGame_St* game) {
    if (!game->endScreen.showEndScreen) return;

    int cx = SCREEN_WIDTH / 2, cy = SCREEN_HEIGHT / 2 - 90;
    float rt = game->resultsRevealTimer;

    // Overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0,0,0,185});

    // Panel slide-in
    float panelAlpha = fminf(1.0f, rt / 0.4f);
    int panelW = 420, panelH = 380;
    int panelX = cx - panelW/2, panelY = cy - 140;

    if (panelAlpha > 0.0f) {
        Color panelBg = (Color){38,28,18,(unsigned char)(250 * panelAlpha)};
        Color border  = (Color){255,215,0,(unsigned char)(255 * panelAlpha)};
        DrawRectangle(panelX, panelY, panelW, panelH, panelBg);
        DrawRectangleLines(panelX, panelY, panelW, panelH, border);
    }

    if (rt > 0.4f) {
        // Title
        float titleAlpha = fminf(1.0f, (rt - 0.4f) / 0.3f);
        Color titleColor = (Color){255,215,0,(unsigned char)(255*titleAlpha)};
        DrawText("GAME OVER", cx - MeasureText("GAME OVER",38)/2, cy-130, 38, titleColor);
    }

    if (rt > 0.85f) {
        float a = fminf(1.0f, (rt-0.85f)/0.25f);
        Color sc = (Color){255,255,255,(unsigned char)(255*a)};
        DrawText(TextFormat("Final Score: %d", game->totalScore),
            cx - MeasureText(TextFormat("Final Score: %d", game->totalScore),26)/2,
            cy - 80, 26, sc);
    }

    // Rating
    if (rt > 1.1f) {
        float a = fminf(1.0f, (rt-1.1f)/0.25f);
        const char* rating;
        Color rc;
        int s = game->totalScore;
        if      (s == 300) { rating = "PERFECT GAME!"; rc = (Color){255,215,0,255}; }
        else if (s >= 200) { rating = "EXCELLENT!";    rc = (Color){255,150,50,255}; }
        else if (s >= 150) { rating = "GREAT!";        rc = (Color){100,220,100,255}; }
        else if (s >= 100) { rating = "GOOD";          rc = (Color){100,180,255,255}; }
        else               { rating = "KEEP TRYING";   rc = (Color){180,180,180,255}; }
        rc.a = (unsigned char)(255*a);
        DrawText(rating, cx - MeasureText(rating,22)/2, cy - 50, 22, rc);
    }

    // Stats appearing one-by-one
    static const float STAT_DELAYS[RESULTS_STEPS] = {1.4f, 1.7f, 2.0f, 2.3f, 2.6f, 2.9f, 3.2f};
    const char* statLabels[RESULTS_STEPS] = {
        TextFormat("Strikes:      %d",  game->stats.totalStrikes),
        TextFormat("Spares:       %d",  game->stats.totalSpares),
        TextFormat("Gutter Balls: %d",  game->stats.gutterBalls),
        TextFormat("Pins Knocked: %d / 100", game->stats.totalPinsKnocked),
        TextFormat("Avg / Frame:  %.1f", game->stats.averagePinsPerFrame),
        TextFormat("Perfect Frames: %d", game->stats.perfectFrames),
        game->stats.totalStrikes == 12 ? "--- PERFECT GAME! ---" : ""
    };
    Color statColors[RESULTS_STEPS] = {
        {255,100,100,255},{100,150,255,255},{255,150,50,255},
        {220,200,180,255},{180,220,180,255},{255,215,0,255},
        {255,215,0,255}
    };

    for (int i = 0; i < RESULTS_STEPS; i++) {
        if (rt < STAT_DELAYS[i]) continue;
        if (statLabels[i][0] == '\0') continue;
        float a = fminf(1.0f, (rt - STAT_DELAYS[i]) / 0.2f);
        // Slide in from left
        int slide = (int)((1.0f - a) * 30);
        Color c   = statColors[i]; c.a = (unsigned char)(255*a);
        DrawText(statLabels[i], panelX + 20 - slide, cy + 5 + i*28, 16, c);
    }

    // Buttons
    if (rt > 3.5f) {
        Rectangle pa = game->endScreen.playAgainBtn;
        Rectangle qu = game->endScreen.quitBtn;
        Vector2   mp = GetMousePosition();
        game->endScreen.playAgainHovered = CheckCollisionPointRec(mp, pa);
        game->endScreen.quitHovered      = CheckCollisionPointRec(mp, qu);

        Color pac = game->endScreen.playAgainHovered ? (Color){80,160,80,255} : (Color){55,120,55,255};
        Color quc = game->endScreen.quitHovered      ? (Color){160,70,70,255} : (Color){120,50,50,255};

        DrawRectangleRec(pa, pac); DrawRectangleLinesEx(pa, 2, WHITE);
        DrawText("Play Again", pa.x+50, pa.y+11, 18, WHITE);
        DrawRectangleRec(qu, quc); DrawRectangleLinesEx(qu, 2, WHITE);
        DrawText("Quit",       qu.x+73, qu.y+11, 18, WHITE);
    }
}

// ─────────────────────────────────────────────────
// Input
// ─────────────────────────────────────────────────
static void bowling_handleInput(BowlingGame_St* game) {
    if (game->ball.isRolling || game->waitingForReset) return;
    float wm = GetMouseWheelMove();
    if (wm != 0) game->power = fmaxf(0.1f, fminf(1.0f, game->power + wm * 0.1f));
    if (IsKeyDown(KEY_A))     game->spin     = fmaxf(-1.0f, game->spin - 0.05f);
    if (IsKeyDown(KEY_D))     game->spin     = fminf( 1.0f, game->spin + 0.05f);
    if (IsKeyDown(KEY_LEFT))  game->aimAngle = fmaxf(-30.0f, game->aimAngle - 1.0f);
    if (IsKeyDown(KEY_RIGHT)) game->aimAngle = fminf( 30.0f, game->aimAngle + 1.0f);
    if (IsKeyPressed(KEY_R))  bowling_resetBallState(game);
}

static void bowling_handleEndScreenInput(BowlingGame_St* game) {
    if (!game->endScreen.showEndScreen) return;
    if (game->resultsRevealTimer < 3.5f) return; // don't allow clicks until buttons shown
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;
    Vector2 mp = GetMousePosition();

    if (CheckCollisionPointRec(mp, game->endScreen.playAgainBtn)) {
        game->endScreen.showEndScreen  = false;
        game->currentFrame             = 0;
        game->totalScore               = 0;
        game->stats                    = (GameStats_St){0};
        game->resultsRevealTimer       = 0.0f;
        game->confettiCount            = 0;
        for (int i = 0; i < BOWLING_MAX_FRAMES; i++) {
            game->frames[i] = (Frame_St){0};
        }
        physics_setupPins(game->pins);
        bowling_resetBallState(game);
        bowling_initCamera(game);
        bowling_initTitleScreen(&game->titleScreen);  // re-show title screen
        game->base.running = true;
    }

    if (CheckCollisionPointRec(mp, game->endScreen.quitBtn)) {
        game->base.running = false;
    }
}

// ─────────────────────────────────────────────────
// Init / Free
// ─────────────────────────────────────────────────
// ─────────────────────────────────────────────────
// Title screen
// ─────────────────────────────────────────────────
static void bowling_initTitleScreen(TitleScreen_St* ts) {
    ts->showTitle     = true;
    ts->bumpers       = false;
    ts->showAimGuide  = true;
    ts->selectedSkin  = 0;

    int cx = SCREEN_WIDTH  / 2;
    int cy = SCREEN_HEIGHT / 2;

    ts->playBtn      = (Rectangle){ cx - 120, cy + 170, 240, 52 };
    ts->bumperBtn    = (Rectangle){ cx -  90, cy +  20, 180, 44 };
    ts->aimGuideBtn  = (Rectangle){ cx -  90, cy +  75, 180, 44 };
    ts->skinPrev     = (Rectangle){ cx - 170, cy -  62, 44,  44 };
    ts->skinNext     = (Rectangle){ cx + 126, cy -  62, 44,  44 };
}

static void bowling_drawTitleScreen(BowlingGame_St* game) {
    TitleScreen_St* ts = &game->titleScreen;
    int cx = SCREEN_WIDTH  / 2;
    int cy = SCREEN_HEIGHT / 2;
    float t = game->timeAccum;

    // Background
    ClearBackground((Color){25, 18, 12, 255});

    // Title glow pulsation
    float pulse = 0.7f + 0.3f * sinf(t * 2.2f);
    Color titleGlow = {(unsigned char)(255 * pulse), (unsigned char)(180 * pulse), 0, 255};
    int tw = MeasureText("BOWLING", 72);
    DrawText("BOWLING", cx - tw/2 + 4, cy - 210 + 4, 72, (Color){0, 0, 0, 120});
    DrawText("BOWLING", cx - tw/2,     cy - 210,     72, titleGlow);

    // Subtitle
    int sw = MeasureText("Mini-Games", 22);
    DrawText("Mini-Games", cx - sw/2, cy - 130, 22, (Color){160, 140, 100, 255});

    // ── Ball skin selector ─────────────────────────
    DrawText("Skin de la balle", cx - MeasureText("Skin de la balle", 18)/2, cy - 100, 18, (Color){200, 180, 140, 255});

    // Preview boule : texture dans un cercle découpé, sans fond coloré
    int bx = cx, by = cy - 44, br = 36;
    DrawEllipse(bx, by + br + 4, br - 4, 5, (Color){0, 0, 0, 60}); // ombre

    int skin = ts->selectedSkin;
    if (skin < game->textures.ballTextureCount && IsTextureValid(game->textures.ballTextures[skin])) {
        // Masque circulaire : dessiner la texture uniquement dans le cercle
        // (DrawTexturePro sur un carré puis DrawCircle par-dessus avec la couleur de fond
        //  pour donner l'illusion d'un clip — la bonne façon avec raylib sans shader)
        Texture2D tex  = game->textures.ballTextures[skin];
        Rectangle src  = {0, 0, (float)tex.width, (float)tex.height};
        Rectangle dest = {(float)(bx - br), (float)(by - br), (float)(br*2), (float)(br*2)};
        DrawTexturePro(tex, src, dest, (Vector2){0,0}, 0.0f, WHITE);
        // Masque circulaire inversé (coins) pour simuler le clip — fond transparent
        // DrawCircle serait opaque, on dessine des triangles coin → trop complexe
        // On laisse la texture carrée mais elle est propre grâce au remplissage des pixels
    } else {
        Color bodyCol = BALL_SKIN_BODY[skin % NUM_BALL_SKINS];
        DrawCircle(bx, by, br, bodyCol);
        float angle = t * 60.0f * DEG2RAD;
        for (int h = 0; h < 3; h++) {
            float ha = angle + h * (2.0f * PI / 3.0f);
            int hx = bx + (int)(cosf(ha) * 15);
            int hy = by + (int)(sinf(ha) * 15);
            DrawCircle(hx, hy, 5, (Color){
                (unsigned char)(bodyCol.r * 0.25f),
                (unsigned char)(bodyCol.g * 0.25f),
                (unsigned char)(bodyCol.b * 0.25f), 255
            });
        }
    }
    DrawCircle(bx - 11, by - 11, 8, (Color){255, 255, 255, 40});

    // Skin name or index
    const char* skinName = (skin < game->textures.ballTextureCount)
        ? TextFormat("Balle %d / %d", skin + 1, game->textures.ballTextureCount)
        : BALL_SKIN_NAMES[skin % NUM_BALL_SKINS];
    DrawText(skinName, cx - MeasureText(skinName, 16)/2, by + br + 12, 16, (Color){220, 200, 160, 255});

    // Prev / Next buttons
    Vector2 mp = GetMousePosition();
    Color prevCol = CheckCollisionPointRec(mp, ts->skinPrev) ? (Color){180,150,80,255} : (Color){100,80,40,255};
    Color nextCol = CheckCollisionPointRec(mp, ts->skinNext) ? (Color){180,150,80,255} : (Color){100,80,40,255};
    DrawRectangleRec(ts->skinPrev, prevCol);
    DrawRectangleRec(ts->skinNext, nextCol);
    DrawRectangleLinesEx(ts->skinPrev, 1.5f, (Color){220,180,80,255});
    DrawRectangleLinesEx(ts->skinNext, 1.5f, (Color){220,180,80,255});
    DrawText("<", (int)ts->skinPrev.x + 13, (int)ts->skinPrev.y + 12, 22, WHITE);
    DrawText(">", (int)ts->skinNext.x + 12, (int)ts->skinNext.y + 12, 22, WHITE);

    // ── Bumpers toggle ─────────────────────────────
    bool bumHover = CheckCollisionPointRec(mp, ts->bumperBtn);
    Color bumBg   = ts->bumpers ? (Color){40,140,60,255} : (Color){100,40,40,255};
    if (bumHover) { bumBg.r = (unsigned char)fminf(255,bumBg.r+30); bumBg.g = (unsigned char)fminf(255,bumBg.g+30); bumBg.b = (unsigned char)fminf(255,bumBg.b+30); }
    DrawRectangleRec(ts->bumperBtn, bumBg);
    DrawRectangleLinesEx(ts->bumperBtn, 1.5f, (Color){220,180,80,255});
    const char* bumLabel = ts->bumpers ? "Barrières : ON" : "Barrières : OFF";
    DrawText(bumLabel,
        (int)ts->bumperBtn.x + (int)(ts->bumperBtn.width/2) - MeasureText(bumLabel,16)/2,
        (int)ts->bumperBtn.y + 14, 16, WHITE);

    // ── Aim guide toggle ───────────────────────────
    bool aimHover = CheckCollisionPointRec(mp, ts->aimGuideBtn);
    Color aimBg   = ts->showAimGuide ? (Color){40,140,60,255} : (Color){100,40,40,255};
    if (aimHover) { aimBg.r = (unsigned char)fminf(255,aimBg.r+30); aimBg.g = (unsigned char)fminf(255,aimBg.g+30); aimBg.b = (unsigned char)fminf(255,aimBg.b+30); }
    DrawRectangleRec(ts->aimGuideBtn, aimBg);
    DrawRectangleLinesEx(ts->aimGuideBtn, 1.5f, (Color){220,180,80,255});
    const char* aimLabel = ts->showAimGuide ? "Visée : ON" : "Visée : OFF";
    DrawText(aimLabel,
        (int)ts->aimGuideBtn.x + (int)(ts->aimGuideBtn.width/2) - MeasureText(aimLabel,16)/2,
        (int)ts->aimGuideBtn.y + 14, 16, WHITE);

    // ── Play button ────────────────────────────────
    bool playHover = CheckCollisionPointRec(mp, ts->playBtn);
    Color playBg   = playHover ? (Color){80, 160, 80, 255} : (Color){55, 120, 55, 255};
    DrawRectangleRec(ts->playBtn, playBg);
    DrawRectangleLinesEx(ts->playBtn, 2.0f, (Color){220, 180, 80, 255});
    int ptw = MeasureText("JOUER", 28);
    DrawText("JOUER",
        (int)ts->playBtn.x + (int)(ts->playBtn.width/2) - ptw/2,
        (int)ts->playBtn.y + 12, 28, WHITE);

    // Controls hint
    DrawText("ESC: Quitter", 14, SCREEN_HEIGHT - 24, 14, (Color){100, 90, 70, 255});
}

static bool bowling_handleTitleScreenInput(BowlingGame_St* game) {
    TitleScreen_St* ts = &game->titleScreen;
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return false;
    Vector2 mp = GetMousePosition();

    if (CheckCollisionPointRec(mp, ts->skinPrev)) {
        int maxSkin = (game->textures.ballTextureCount > 0) ? game->textures.ballTextureCount : NUM_BALL_SKINS;
        ts->selectedSkin = (ts->selectedSkin - 1 + maxSkin) % maxSkin;
        return false;
    }
    if (CheckCollisionPointRec(mp, ts->skinNext)) {
        int maxSkin = (game->textures.ballTextureCount > 0) ? game->textures.ballTextureCount : NUM_BALL_SKINS;
        ts->selectedSkin = (ts->selectedSkin + 1) % maxSkin;
        return false;
    }
    if (CheckCollisionPointRec(mp, ts->bumperBtn)) {
        ts->bumpers = !ts->bumpers;
        return false;
    }
    if (CheckCollisionPointRec(mp, ts->aimGuideBtn)) {
        ts->showAimGuide = !ts->showAimGuide;
        return false;
    }
    if (CheckCollisionPointRec(mp, ts->playBtn)) {
        game->bumpers       = ts->bumpers;
        game->showAimGuide  = ts->showAimGuide;
        game->selectedSkin  = ts->selectedSkin;
        ts->showTitle       = false;
        // Remplir les textures des boules avec leurs couleurs après l'écran d'accueil
        bowling_fillBallTextures(&game->textures);
        return true;
    }
    return false;
}

// ─────────────────────────────────────────────────
Error_Et bowling_initGame__full(BowlingGame_St** game, BowlingConfigs_St configs) {
    (void)configs;
    *game = malloc(sizeof(BowlingGame_St));
    if (!*game) { log_error("Failed to allocate BowlingGame_St"); return ERROR_ALLOC; }

    BowlingGame_St* g = *game;
    memset(g, 0, sizeof(*g));

    g->base.freeGame  = bowling_freeGameWrapper;
    g->base.running   = true;

    bowling_initCamera(g);

    g->ball.position       = (Vector3){0.0f, BALL_RADIUS, APPROACH_LEN - 0.5f};
    g->ball.velocity       = (Vector3){0,0,0};
    g->ball.spin           = (Vector3){0,0,0};
    g->ball.spinAmount     = 0;
    g->ball.radius         = BALL_RADIUS;  // 10.92 cm (diam. max 21.83 cm)
    g->ball.mass           = 6.35f;        // 14 lbs ≈ 6.35 kg (typique)
    g->ball.isRolling     = false;
    g->ball.visualRotation = 0;
    g->ball.rollAxis      = (Vector3){1,0,0};

    physics_setupPins(g->pins);
    physics_initParticles(g->particles, &g->particleCount);

    g->currentFrame  = 0;
    g->totalScore    = 0;
    g->isAiming      = false;
    g->power         = 0.5f;
    g->spin          = 0;
    g->aimAngle      = 0;
    g->resetTimer    = 0;
    g->waitingForReset = false;
    g->confettiCount = 0;
    g->timeAccum     = 0;
    g->resultsRevealTimer     = 0;
    g->audienceReactionTimer  = 0;
    g->audienceReactionType   = 0;
    g->scoreAnim.active       = false;

    Error_Et tex = bowling_loadTextures(&g->textures);
    if (tex != OK) {
        log_error("Failed to load textures – proceeding with procedural rendering");
        // not fatal – we render without textures
    }
    if (g->textures.ballTextureCount > 0)
        g->currentBallTextureIndex = GetRandomValue(0, g->textures.ballTextureCount - 1);

    g->bumpers      = configs.bumpers;
    g->selectedSkin = (configs.ballSkin >= 0 && configs.ballSkin < NUM_BALL_SKINS) ? configs.ballSkin : 0;
    bowling_initTitleScreen(&g->titleScreen);

    log_debug("Bowling initialized successfully");
    return OK;
}

// ─────────────────────────────────────────────────
// Game loop
// ─────────────────────────────────────────────────
Error_Et bowling_gameLoop(BowlingGame_St* const game) {
    if (!game) { log_error("NULL game in gameLoop"); return ERROR_NULL_POINTER; }
    if (!game->base.running) return OK;

    float dt = GetFrameTime();
    game->timeAccum += dt;

    // ── Title screen ──────────────────────────────
    if (game->titleScreen.showTitle) {
        if (IsKeyPressed(KEY_ESCAPE)) { game->base.running = false; return OK; }
        bowling_handleTitleScreenInput(game);
        BeginDrawing();
            bowling_drawTitleScreen(game);
        EndDrawing();
        return OK;
    }

    // Update audience reaction
    if (game->audienceReactionTimer > 0.0f)
        game->audienceReactionTimer = fmaxf(0.0f, game->audienceReactionTimer - dt);
    else
        game->audienceReactionType = 0;

    bowling_handleEndScreenInput(game);

    if (game->endScreen.showEndScreen) {
        game->resultsRevealTimer += dt;
        bowling_updateConfetti(game, dt);

        BeginDrawing();
            ClearBackground((Color){40,30,20,255});
            bowling_drawEndScreen(game);
            bowling_drawConfetti(game);
        EndDrawing();
        return OK;
    }

    bowling_handleInput(game);

    // Aiming and launch
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !game->ball.isRolling && !game->waitingForReset) {
        game->isAiming  = true;
        game->aimStart  = GetMousePosition();
    }
    if (game->isAiming) {
        game->aimCurrent = GetMousePosition();
        Vector2 diff     = Vector2Subtract(game->aimCurrent, game->aimStart);
        game->power      = fminf(1.0f, 0.3f + Vector2Length(diff) / 300.0f);
        game->aimAngle   = fmaxf(-30.0f, fminf(30.0f, -diff.x / 10.0f));
        game->spin       = fmaxf(-1.0f, fminf(1.0f, -diff.x / 200.0f));

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            game->isAiming = false;
            float angle    = game->aimAngle * DEG2RAD;
            Vector3 dir    = Vector3Normalize((Vector3){-sinf(angle), 0, -cosf(angle)});
            // Vitesse officielle : amateur 20-25 km/h, pro 25-35 km/h
            // = 5.5 à 9.7 m/s. On offre 4 à 14 m/s pour couvrir tous niveaux.
            float launchPower = game->power * 10.0f + 4.0f;
            physics_launchBall(&game->ball, dir, launchPower, game->spin);
        }
    }

    // Physics
    if (game->ball.isRolling) {
        physics_updateBall(&game->ball, dt, LANE_WIDTH, GUTTER_WIDTH, game->bumpers);
        physics_checkCollisions(&game->ball, game->pins, game->particles, &game->particleCount);
        physics_updatePins(game->pins, NUM_PINS, dt, LANE_WIDTH, LANE_LENGTH);
        physics_updateParticles(game->particles, &game->particleCount, dt);

        float speed = Vector3Length(game->ball.velocity);

        // Back wall: balle arrêtée → fin du lancer immédiate
        if (game->ball.position.z <= BOWLING_WALL_Z + game->ball.radius) {
            game->ball.position.z = BOWLING_WALL_Z + game->ball.radius;
            game->ball.velocity   = (Vector3){0, 0, 0};
            game->ball.isRolling  = false;
            bowling_handleBallStopped(game);
        } else if (speed < 0.1f) {
            game->ball.isRolling = false;
            bowling_handleBallStopped(game);
        } else {
            float edge = LANE_WIDTH * 2.0f;
            if (game->ball.position.x < -edge - 0.5f || game->ball.position.x > edge + 0.5f) {
                game->ball.isRolling = false;
                bowling_handleBallStopped(game);
            }
        }
    }

    bowling_updateCamera(game, dt);
    bowling_processResetTimer(game, dt);
    bowling_updateScoreAnim(game, dt);
    bowling_updateConfetti(game, dt);

    if (IsKeyPressed(KEY_SPACE)) {
        Frame_St* f   = &game->frames[game->currentFrame];
        bool is10th   = (game->currentFrame == BOWLING_MAX_FRAMES - 1);
        bool needs3rd = is10th && (f->isStrike || f->isSpare) && f->numRolls < 3;
        if (f->isStrike || (f->numRolls >= 2 && !needs3rd)) bowling_nextFrame(game);
        else bowling_resetBallState(game);
    }
    if (IsKeyPressed(KEY_ESCAPE)) game->base.running = false;

    // ── Render ────────────────────────────────────
    BeginDrawing();
        ClearBackground((Color){40,30,20,255});

        BeginMode3D(game->cameraState.camera);
            bowling_drawEnvironment(game);
            bowling_drawLane(game);
            bowling_drawBallShadow(game);
            bowling_drawPinShadows(game);
            bowling_drawPins(game);
            bowling_drawBall(game);
            bowling_drawAimGuide(game);
            bowling_drawParticles(game);
        EndMode3D();

        bowling_drawFullScoreboard(game);
        bowling_drawPowerMeter(game);
        bowling_drawSpinMeter(game);
        bowling_drawStatsPanel(game);
        bowling_drawControls(game);
        bowling_drawScoreAnim(game);
        bowling_drawConfetti(game);
        bowling_drawEndScreen(game);

        if (game->waitingForReset && !game->endScreen.showEndScreen)
            DrawText(TextFormat("Reset in %.1f...", game->resetTimer),
                     SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2, 20, (Color){255,200,50,255});
    EndDrawing();

    return OK;
}

// ─────────────────────────────────────────────────
// Free
// ─────────────────────────────────────────────────
Error_Et bowling_freeGame(BowlingGame_St** game) {
    if (!game || !*game) return ERROR_NULL_POINTER;
    bowling_unloadTextures(&(*game)->textures);
    free(*game);
    *game = NULL;
    return OK;
}
