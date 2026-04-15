/**
    @file ball.c
    @author Maxime CHAUVEAU
    @date 2026-04-07
    @date 2026-04-14
    @brief Rendering logic for the golf ball and its projected trajectory.
*/
#include "../../include/core/game.h"

void Ball_Draw(GolfGame *g) {
    Ball  *b        = &g->ball;
    float  terrain_y;
    float  shadow_scale;

    if (b->state == BALL_IN_HOLE) return;

    terrain_y    = Golf_GetTerrainHeight(g, b->pos.x, b->pos.z);
    shadow_scale = Clamp(1.0f - (b->pos.y - terrain_y)*0.3f, 0.3f, 1.0f);

    DrawCylinder(
        (Vector3){b->pos.x, terrain_y+0.01f, b->pos.z},
        BALL_R*shadow_scale*1.2f, BALL_R*shadow_scale*1.2f, 0.01f, 12,
        (Color){0,0,0,(unsigned char)(80*shadow_scale)}
    );
    DrawSphere(b->pos, BALL_R, WHITE);
    DrawSphereWires(b->pos, BALL_R, 6, 6, (Color){200,200,200,80});
}

void Ball_DrawTrajectory(GolfGame *g) {
    const ClubSpec *cs;
    float  rad, loft, speed, vx, vy, vz;
    Vector3 pos;
    float   sim_dt = 0.05f;
    int     pts = 40, i;

    if (!g->show_trajectory || g->ball.state != BALL_IDLE) return;

    cs    = &CLUBS[g->club];
    rad   = g->aim_angle * DEG2RAD;
    loft  = cs->loft_deg * DEG2RAD;
    speed = cs->max_power * g->power;

    vx  = sinf(rad) * cosf(loft) * speed;
    vy  = sinf(loft) * speed * 0.5f;
    vz  = cosf(rad)  * cosf(loft) * speed;
    pos = g->ball.pos;

    for (i = 0; i < pts - 1; i++) {
        Vector3 p0 = pos;
        float   ty, t;
        Color   c;

        vx *= 0.998f; vz *= 0.998f;
        vy -= GRAVITY * sim_dt;
        pos.x += vx * sim_dt;
        pos.y += vy * sim_dt;
        pos.z += vz * sim_dt;

        ty = Golf_GetTerrainHeight(g, pos.x, pos.z);
        if (pos.y < ty + BALL_R) {
            pos.y = ty + BALL_R;
            vy = -vy * BOUNCE_FACTOR;
        }

        t = (float)i / (float)(pts - 1);
        c = (Color){
            (unsigned char)(255*(1.0f-t)),
            (unsigned char)(255*t),
            50,
            (unsigned char)(200*(1.0f - t*0.7f))
        };
        DrawLine3D(p0, pos, (Color){c.r, c.g, c.b, (unsigned char)(c.a/2)});
        if (i % 2 == 0) DrawSphere(pos, 0.035f, c);
    }
}