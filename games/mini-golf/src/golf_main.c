/**
    @file golf_main.c
    @author Maxime CHAUVEAU
    @date 2026-04-07
    @date 2026-04-14
    @brief Core game logic for Golf 3D: wind, aim arrow, club billboard,
*/
#include "golf.h"

/* ─── Vent ───────────────────────────────────────────────────────────────── */

/**
    @brief Randomizes the wind speed and direction.
    @param[in,out] g Golf game state.
*/
void Game_NewWind(GolfGame *g) {
    float spd = (float)GetRandomValue(0, 30) / 10.0f;
    float rad = (float)GetRandomValue(0, 360) * DEG2RAD;
    g->wind.speed = spd;
    g->wind.vec = (Vector3){sinf(rad)*spd, 0.0f, cosf(rad)*spd};
}

/* ─── Flèche de visée 3D ─────────────────────────────────────────────────── */

/**
    @brief Renders a 3D aiming arrow pointing in the current shot direction.
    @param[in] g Golf game state.
*/
static void draw_aim_arrow(GolfGame *g) {
    Ball   *b   = &g->ball;
    float   len = 2.0f;
    float   rad = g->aim_angle * DEG2RAD;
    Vector3 max_pt = {
        b->pos.x + sinf(rad)*len,
        b->pos.y + 0.1f,
        b->pos.z + cosf(rad)*len
    };
    DrawLine3D(b->pos, max_pt, (Color){100,100,255,80});
}

/* ─── Club billboard ─────────────────────────────────────────────────────── */

/**
    @brief Renders a 3D billboard representing the selected club.
    @param[in] g Golf game state.
*/
static void draw_club_3d(GolfGame *g) {
    Ball    *b   = &g->ball;
    float    off = 0.8f;
    float    rad = (g->aim_angle + 180.0f) * DEG2RAD;
    Vector3 club_pos = {
        b->pos.x + sinf(rad)*off,
        b->pos.y + 0.5f,
        b->pos.z + cosf(rad)*off
    };
    DrawBillboard(g->gcam.cam, g->tex_club, club_pos, 2.0f, WHITE);
}

/* ─── Marqueurs de départ ────────────────────────────────────────────────── */

/**
    @brief Renders the tee markers at the start of the hole.
    @param[in] g Golf game state.
*/
static void draw_tee_markers(GolfGame *g) {
    HoleData *h  = &g->holes[g->current_hole];
    Vector3   tee = h->tee_pos;
    float     ty  = tee.y + 0.05f;
    int       i;
    for (i = -1; i <= 1; i += 2) {
        DrawCylinder((Vector3){tee.x,ty,tee.z}, 0.02f,0.03f,0.08f,6,
                     (Color){180,120,60,255});
    }
}

/* ─── Init ───────────────────────────────────────────────────────────────── */

/**
    @brief Initializes the golf game state, textures, and cameras.
    @param[in,out] g Golf game state to initialize.
*/
void Game_Init(GolfGame *g) {
    int i;
    memset(g, 0, sizeof(GolfGame));

    g->screen_w = SCREEN_W;
    g->screen_h = SCREEN_H;
    g->state    = STATE_MENU;

    for (i = 0; i < MAX_HOLES; i++) g->score.strokes[i] = SCORE_HOLE_OUT;

    Golf_InitCourse(g);
    Ball_Init(&g->ball, (Vector3){0,0,0});
    GCam_Init(&g->gcam, (Vector3){0,0,0});

    g->tex_ball = LoadTexture(ASSET_PATH "balle.png");
    g->tex_club = LoadTexture(ASSET_PATH "club.png");
    g->tex_fairway = LoadTexture(ASSET_PATH "pelouse.jpg");
    g->tex_rough = LoadTexture(ASSET_PATH "pelouse.jpg");
    g->tex_green = LoadTexture(ASSET_PATH "pelouse.jpg");
    g->tex_sand = LoadTexture(ASSET_PATH "sable.jpg");
    g->tex_water = LoadTexture(ASSET_PATH "eau.jpg");
    SetTextureFilter(g->tex_ball, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(g->tex_club, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(g->tex_fairway, TEXTURE_FILTER_TRILINEAR);
    SetTextureFilter(g->tex_rough, TEXTURE_FILTER_TRILINEAR);
    SetTextureFilter(g->tex_green, TEXTURE_FILTER_TRILINEAR);
    SetTextureFilter(g->tex_sand, TEXTURE_FILTER_TRILINEAR);
    SetTextureFilter(g->tex_water, TEXTURE_FILTER_TRILINEAR);

    g->show_trajectory = true;
    g->power           = 0.0f;
    g->power_rising    = true;
    g->club            = CLUB_DRIVER;
}

/* ─── Update ─────────────────────────────────────────────────────────────── */

/**
    @brief Updates the game state based on elapsed time and user input.
    @param[in,out] g Golf game state.
    @param[in]     dt Elapsed time since the last frame.
*/
void Game_Update(GolfGame *g, float dt) {
    int k;

    switch (g->state) {

    case STATE_MENU:
        break;

    case STATE_HOLE_INTRO:
        g->intro_timer += dt;
        GCam_Update(&g->gcam, g, dt);
        if (g->intro_timer > 3.5f) {
            g->state = STATE_AIMING;
            GCam_SetMode(&g->gcam, CAM_ORBIT);
        }
        break;

    case STATE_AIMING:
        GCam_Update(&g->gcam, g, dt);
        if (IsKeyDown(KEY_LEFT))  g->aim_angle -= 60.0f * dt;
        if (IsKeyDown(KEY_RIGHT)) g->aim_angle += 60.0f * dt;

        if (IsKeyPressed(KEY_TAB))
            g->club = (ClubType)((g->club + 1) % CLUB_COUNT);
        for (k = KEY_ONE; k <= KEY_SIX; k++)
            if (IsKeyPressed(k)) g->club = (ClubType)(k - KEY_ONE);

        if (g->ball.surface == SURF_GREEN && g->club != CLUB_PUTTER)
            g->club = CLUB_PUTTER;

        if (IsKeyPressed(KEY_T)) g->show_trajectory = !g->show_trajectory;

        if (IsKeyPressed(KEY_SPACE)) {
            g->state        = STATE_POWER;
            g->power        = 0.0f;
            g->power_rising = true;
        }
        if (IsKeyPressed(KEY_ESCAPE)) g->state = STATE_PAUSED;
        break;

    case STATE_POWER: {
        float speed_rate = 1.2f;
        GCam_Update(&g->gcam, g, dt);
        if (g->power_rising) {
            g->power += speed_rate * dt;
            if (g->power >= 1.0f) { g->power = 1.0f; g->power_rising = false; }
        } else {
            g->power -= speed_rate * dt;
            if (g->power <= 0.0f) { g->power = 0.0f; g->power_rising = true; }
        }
        if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Ball_Shoot(g);
            g->state = STATE_BALL_MOVING;
            GCam_SetMode(&g->gcam, CAM_FOLLOW);
        }
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
            g->state = STATE_AIMING;
            g->power = 0.0f;
        }
        break;
    }

    case STATE_BALL_MOVING: {
        int strokes;
        Ball_Update(g, dt);
        GCam_Update(&g->gcam, g, dt);

        if (Ball_IsInHole(g)) {
            g->ball.state = BALL_IN_HOLE;
            strokes = g->ball.strokes + g->ball.penalty;
            g->score.strokes[g->current_hole] = strokes;
            g->score.total  += strokes;
            g->score.to_par += strokes - g->holes[g->current_hole].par;
            g->state        = STATE_HOLE_RESULT;
            g->state_timer  = 0.0f;
            GCam_SetMode(&g->gcam, CAM_ORBIT);
            break;
        }
        if (g->ball.state == BALL_IDLE) {
            g->state = STATE_AIMING;
            g->power = 0.5f;
            GCam_SetMode(&g->gcam, CAM_ORBIT);
            if (g->ball.surface == SURF_GREEN) g->club = CLUB_PUTTER;
        }
        break;
    }

    case STATE_HOLE_RESULT: {
        int next;
        g->state_timer += dt;
        GCam_Update(&g->gcam, g, dt);
        if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (g->current_hole < MAX_HOLES - 1) {
                next = g->current_hole + 1;
                Golf_StartHole(g, next);
                g->state       = STATE_HOLE_INTRO;
                g->intro_timer = 0.0f;
                GCam_SetMode(&g->gcam, CAM_HOLE);
            } else {
                g->state = STATE_SCORECARD;
            }
        }
        break;
    }

    case STATE_SCORECARD:
        /* ESC ou ESPACE sur le scorecard → fin de partie (retour lobby) */
        /* Géré dans golfAPI.c (golf_gameLoop) pour setter base.running = false */
        break;

    case STATE_PAUSED:
        if (IsKeyPressed(KEY_ESCAPE)) g->state = g->prev_state;
        break;
    }

    if (g->state != STATE_PAUSED) g->prev_state = g->state;
}

/* ─── Draw ───────────────────────────────────────────────────────────────── */

/**
    @brief Draws the 3D scene and the UI.
    @param[in] g Golf game state.
*/
void Game_Draw(GolfGame *g) {
    BeginDrawing();
    ClearBackground((Color){20,60,20,255});

    if (g->state == STATE_MENU)      { UI_DrawMenu(g);      EndDrawing(); return; }
    if (g->state == STATE_SCORECARD) { UI_DrawScorecard(g); EndDrawing(); return; }

    GCam_BeginMode3D(&g->gcam);
        DrawPlane(
            (Vector3){0, -0.5f, g->holes[g->current_hole].distance_m * SCALE / 2.0f},
            (Vector2){100, g->holes[g->current_hole].distance_m * SCALE + 40},
            (Color){30,80,30,255}
        );
        Golf_DrawTerrain(g);
        Golf_DrawHazards(g);
        Golf_DrawFlag(g);
        Golf_DrawHoleCup(g);
        draw_tee_markers(g);
        draw_aim_arrow(g);
        draw_club_3d(g);
        Ball_DrawTrajectory(g);
        Ball_Draw(g);
    GCam_EndMode3D();

    UI_DrawHUD(g);
    UI_DrawPower(g);
    if (g->state == STATE_HOLE_INTRO)  UI_DrawHoleIntro(g);
    if (g->state == STATE_HOLE_RESULT) UI_DrawHoleResult(g);
    if (g->state == STATE_PAUSED)      UI_DrawPaused(g);

    EndDrawing();
}

/* ─── Cleanup ────────────────────────────────────────────────────────────── */

/**
    @brief Frees loaded resources (textures, etc.).
    @param[in,out] g Golf game state.
*/
void Game_Cleanup(GolfGame *g) {
    UnloadTexture(g->tex_ball);
    UnloadTexture(g->tex_club);
    UnloadTexture(g->tex_fairway);
    UnloadTexture(g->tex_rough);
    UnloadTexture(g->tex_green);
    UnloadTexture(g->tex_sand);
    UnloadTexture(g->tex_water);
}
