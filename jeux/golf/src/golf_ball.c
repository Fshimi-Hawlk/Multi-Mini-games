#include "golf.h"

/* ─── Init ───────────────────────────────────────────────────────────────── */
void Ball_Init(Ball *b, Vector3 pos) {
    b->pos       = pos;
    b->pos.y    += BALL_R;
    b->vel       = Vector3Zero();
    b->state     = BALL_IDLE;
    b->surface   = SURF_FAIRWAY;
    b->spin      = 0.0f;
    b->rot_angle = 0.0f;
    b->strokes   = 0;
    b->penalty   = 0;
    b->last_valid = pos;
}

/* ─── Tir ────────────────────────────────────────────────────────────────── */
void Ball_Shoot(GolfGame *g) {
    const ClubSpec *cs = &CLUBS[g->club];
    Ball           *b  = &g->ball;
    float           rad, loft, speed, disp, final_angle;

    b->strokes++;
    b->last_valid = b->pos;

    rad   = g->aim_angle * DEG2RAD;
    loft  = cs->loft_deg * DEG2RAD;
    speed = cs->max_power * g->power;

    disp        = (1.0f - cs->accuracy) * ((float)GetRandomValue(-100,100)/100.0f) * 8.0f;
    final_angle = rad + disp * DEG2RAD;

    b->vel.x = sinf(final_angle) * cosf(loft) * speed;
    b->vel.y = sinf(loft)        * speed * 0.5f;
    b->vel.z = cosf(final_angle) * cosf(loft) * speed;

    /* Effet vent initial */
    b->vel.x += g->wind.vec.x * 0.15f;
    b->vel.z += g->wind.vec.z * 0.15f;

    b->state = BALL_FLYING;
    b->spin  = speed * 0.8f;

    g->shot_pos_count = 0;
}

/* ─── Vent en vol ────────────────────────────────────────────────────────── */
void Ball_ApplyWindEffect(GolfGame *g, float dt) {
    float wind_ms = g->wind.speed_kmh / 3.6f;
    g->ball.vel.x += g->wind.vec.x * wind_ms * 0.002f * dt;
    g->ball.vel.z += g->wind.vec.z * wind_ms * 0.002f * dt;
}

/* ─── Physique ───────────────────────────────────────────────────────────── */
void Ball_Update(GolfGame *g, float dt) {
    Ball  *b = &g->ball;
    float  terrain_y, ground_y;

    if (b->state == BALL_IDLE || b->state == BALL_IN_HOLE) return;

    terrain_y = Golf_GetTerrainHeight(g, b->pos.x, b->pos.z);
    ground_y  = terrain_y + BALL_R;
    b->surface = Golf_GetSurface(g, b->pos.x, b->pos.z);

    /* OOB */
    if (b->surface == SURF_OOB) {
        b->penalty++;
        b->pos   = b->last_valid;
        b->pos.y = Golf_GetTerrainHeight(g, b->pos.x, b->pos.z) + BALL_R;
        b->vel   = Vector3Zero();
        b->state = BALL_IDLE;
        return;
    }

    /* Eau */
    if (b->surface == SURF_WATER && b->pos.y <= ground_y + 0.1f) {
        b->penalty++;
        b->pos   = b->last_valid;
        b->pos.y = Golf_GetTerrainHeight(g, b->pos.x, b->pos.z) + BALL_R;
        b->vel   = Vector3Zero();
        b->state = BALL_IDLE;
        return;
    }

    /* ── Vol ── */
    if (b->state == BALL_FLYING) {
        float air_drag = 0.998f;
        b->vel.y     -= GRAVITY * dt;
        b->vel.x     *= air_drag;
        b->vel.z     *= air_drag;
        Ball_ApplyWindEffect(g, dt);
        b->rot_angle += b->spin * dt * 15.0f;

        b->pos.x += b->vel.x * dt;
        b->pos.y += b->vel.y * dt;
        b->pos.z += b->vel.z * dt;

        if (g->shot_pos_count < 999)
            g->shot_positions[g->shot_pos_count++] = b->pos;

        if (b->pos.y <= ground_y) {
            float vy_abs = fabsf(b->vel.y);
            float bounce = BOUNCE_FACTOR;
            if (b->surface == SURF_SAND)  bounce = 0.05f;
            if (b->surface == SURF_GREEN) bounce = 0.20f;
            if (b->surface == SURF_ROUGH) bounce = 0.25f;

            b->pos.y = ground_y;
            if (vy_abs > 1.0f) {
                b->vel.y  = -b->vel.y * bounce;
                b->spin  *= 0.5f;
                if (b->surface == SURF_SAND) {
                    b->vel.x *= 0.4f;
                    b->vel.z *= 0.4f;
                }
            } else {
                b->vel.y = 0.0f;
                b->state = BALL_ROLLING;
            }
        }
    }

    /* ── Roulement ── */
    if (b->state == BALL_ROLLING) {
        float   friction;
        Vector3 normal;
        float   speed;
        float   ty;

        switch (b->surface) {
            case SURF_GREEN:   friction = FRICTION_GREEN;   break;
            case SURF_FAIRWAY: friction = FRICTION_FAIRWAY; break;
            case SURF_SAND:    friction = FRICTION_SAND;    break;
            default:           friction = FRICTION_ROUGH;   break;
        }

        normal    = Golf_GetTerrainNormal(g, b->pos.x, b->pos.z);
        b->vel.x += normal.x * SLOPE_INFLUENCE * dt * 9.81f;
        b->vel.z += normal.z * SLOPE_INFLUENCE * dt * 9.81f;
        b->vel.x *= friction;
        b->vel.z *= friction;
        b->vel.y  = 0.0f;

        speed        = Vector3Length(b->vel);
        b->rot_angle += speed * dt * 20.0f;

        b->pos.x += b->vel.x * dt;
        b->pos.z += b->vel.z * dt;
        ty = Golf_GetTerrainHeight(g, b->pos.x, b->pos.z);
        b->pos.y = ty + BALL_R;

        if (g->shot_pos_count < 999)
            g->shot_positions[g->shot_pos_count++] = b->pos;

        if (speed < STOP_THRESHOLD) {
            b->vel   = Vector3Zero();
            b->state = BALL_IDLE;
        }
    }

    if (b->surface != SURF_WATER && b->surface != SURF_OOB)
        b->last_valid = b->pos;
}

/* ─── Dans le trou ? ─────────────────────────────────────────────────────── */
bool Ball_IsInHole(GolfGame *g) {
    HoleData *h     = &g->holes[g->current_hole];
    float     d     = Vector3Distance(g->ball.pos, h->hole_pos);
    float     speed = Vector3Length(g->ball.vel);
    return (d < HOLE_R + BALL_R) && (speed < 3.0f) && (g->ball.state != BALL_FLYING);
}

/* ─── Dessin balle ───────────────────────────────────────────────────────── */
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

/* ─── Trajectoire prédictive ─────────────────────────────────────────────── */
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
