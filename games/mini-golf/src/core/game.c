/**
    @file game.c
    @author Maxime CHAUVEAU
    @date 2026-04-14
    @date 2026-04-14
    @brief game.c implementation/header file
*/
#include "../../include/core/game.h"

extern const ClubSpec CLUBS[CLUB_COUNT];

void Game_NewWind(GolfGame *g) {
    float rad, spd;
    g->wind.speed_kmh     = (float)GetRandomValue(0, 35);
    g->wind.direction_deg = (float)GetRandomValue(0, 359);
    rad = g->wind.direction_deg * DEG2RAD;
    spd = g->wind.speed_kmh / 3.6f;
    g->wind.vec = (Vector3){sinf(rad)*spd, 0.0f, cosf(rad)*spd};
}

static void draw_aim_arrow(GolfGame *g) {
    Ball   *b   = &g->ball;
    float   rad = g->aim_angle * DEG2RAD;
    float   len = 3.0f;
    int     i;
    Vector3 tip, max_pt;
    float   max_dist;

    if (g->state != STATE_AIMING && g->state != STATE_POWER) return;

    tip = (Vector3){
        b->pos.x + sinf(rad)*len,
        b->pos.y,
        b->pos.z + cosf(rad)*len
    };
    DrawLine3D(b->pos, tip, YELLOW);

    for (i = 0; i < 3; i++) {
        float   off = (float)i * 0.06f;
        Vector3 p   = {
            b->pos.x + sinf(rad)*(len*0.8f+off),
            b->pos.y,
            b->pos.z + cosf(rad)*(len*0.8f+off)
        };
        DrawSphere(p, 0.07f - off*0.01f, YELLOW);
    }

    max_dist = CLUBS[g->club].max_dist_m * SCALE * SCALE;
    max_pt   = (Vector3){
        b->pos.x + sinf(rad)*max_dist,
        b->pos.y,
        b->pos.z + cosf(rad)*max_dist
    };
    DrawLine3D(b->pos, max_pt, (Color){100,100,255,80});
}

static void draw_club_3d(GolfGame *g) {
    Ball    *b   = &g->ball;
    float    rad = g->aim_angle * DEG2RAD;
    Vector3  club_pos;

    if (g->tex_club.id == 0) return;
    if (g->state != STATE_AIMING && g->state != STATE_POWER) return;

    club_pos = (Vector3){
        b->pos.x - sinf(rad)*1.0f,
        b->pos.y + 0.5f,
        b->pos.z - cosf(rad)*1.0f
    };
    DrawBillboard(g->gcam.cam, g->tex_club, club_pos, 2.0f, WHITE);
}

static void draw_tee_markers(GolfGame *g) {
    HoleData *h  = &g->holes[g->current_hole];
    Vector3   tee = h->tee_pos;
    float     ty  = Golf_GetTerrainHeight(g, tee.x, tee.z);

    DrawCylinder((Vector3){tee.x-0.3f,ty,tee.z}, 0.05f,0.05f,0.4f,6,WHITE);
    DrawCylinder((Vector3){tee.x+0.3f,ty,tee.z}, 0.05f,0.05f,0.4f,6,WHITE);
    if (g->ball.strokes == 0)
        DrawCylinder((Vector3){tee.x,ty,tee.z}, 0.02f,0.03f,0.08f,6,
                     (Color){180,120,60,255});
}

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
    SetTextureFilter(g->tex_ball, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(g->tex_club, TEXTURE_FILTER_BILINEAR);

    g->show_trajectory = true;
    g->power           = 0.0f;
    g->power_rising    = true;
    g->club            = CLUB_DRIVER;
}

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
        break;

    case STATE_PAUSED:
        if (IsKeyPressed(KEY_ESCAPE)) g->state = g->prev_state;
        break;
    }

    if (g->state != STATE_PAUSED) g->prev_state = g->state;
}

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

void Game_Cleanup(GolfGame *g) {
    UnloadTexture(g->tex_ball);
    UnloadTexture(g->tex_club);
}