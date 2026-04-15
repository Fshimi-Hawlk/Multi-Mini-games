/**
    @file golf_course.c
    @author Maxime CHAUVEAU
    @date 2026-04-14
    @date 2026-04-14
    @brief Terrain generation and course layout for Golf 3D.
*/
#include "golf.h"
#include "rlgl.h"
#include "sharedUtils/debug.h"

/* ─── Bruit de Perlin 2D (simplifié, déterministe) ───────────────────────── */

/**
    @brief Fade function for Perlin noise (6t^5 - 15t^4 + 10t^3).
    @param[in] t Input value.
    @return Faded value.
*/
static float fade(float t) { return t*t*t*(t*(t*6.0f-15.0f)+10.0f); }

/**
    @brief Standard linear interpolation.
    @param[in] a Start value.
    @param[in] b End value.
    @param[in] t Interpolation factor [0, 1].
    @return Interpolated value.
*/
static float lerp_f(float a, float b, float t) { return a + t*(b-a); }

/**
    @brief 2D gradient function for Perlin noise.
    @param[in] hash Hash value from permutation table.
    @param[in] x    Relative X coordinate in cell.
    @param[in] z    Relative Z coordinate in cell.
    @return Gradient dot product.
*/
static float grad2(int hash, float x, float z) {
    int h = hash & 3;
    float u = (h < 2) ? x : z;
    float v = (h < 2) ? z : x;
    return ((h&1) ? -u : u) + ((h&2) ? -v : v);
}

static unsigned char g_perm[512];      ///< Permutation table for noise.
static bool          g_perm_init = false; ///< Initialization flag.

/**
    @brief Initializes the permutation table with a specific seed.
    @param[in] seed Random seed.
*/
static void perm_init(unsigned int seed) {
    unsigned char p[256];
    int i;
    for (i = 0; i < 256; i++) p[i] = (unsigned char)i;
    for (i = 255; i > 0; i--) {
        int j;
        unsigned char tmp;
        seed = seed * 1664525u + 1013904223u;
        j = (int)(seed % (unsigned int)(i + 1));
        tmp = p[i]; p[i] = p[j]; p[j] = tmp;
    }
    for (i = 0; i < 512; i++) g_perm[i] = p[i & 255];
    g_perm_init = true;
}

/**
    @brief Generates 2D Perlin noise at (x, z).
    @param[in] x X coordinate.
    @param[in] z Z coordinate.
    @return Noise value in range [-1, 1].
*/
static float perlin2d(float x, float z) {
    int X = (int)floorf(x) & 255;
    int Z = (int)floorf(z) & 255;
    x -= floorf(x);
    z -= floorf(z);
    float u = fade(x), v = fade(z);
    int a = g_perm[X]+Z, b = g_perm[X+1]+Z;
    return lerp_f(
        lerp_f(grad2(g_perm[a],   x,   z  ), grad2(g_perm[b],   x-1.0f, z  ), u),
        lerp_f(grad2(g_perm[a+1], x,   z-1.0f), grad2(g_perm[b+1], x-1.0f, z-1.0f), u),
        v);
}

/**
    @brief Generates multi-octave fractal noise.
    @param[in] x    X coordinate.
    @param[in] z    Z coordinate.
    @param[in] oct  Number of octaves.
    @param[in] pers Persistence (amplitude multiplier).
    @param[in] lac  Lacunarity (frequency multiplier).
    @return Combined noise value.
*/
static float octave_noise(float x, float z, int oct, float pers, float lac) {
    float val = 0.0f, amp = 1.0f, freq = 1.0f, maxv = 0.0f;
    int i;
    for (i = 0; i < oct; i++) {
        val  += perlin2d(x*freq, z*freq) * amp;
        maxv += amp;
        amp  *= pers;
        freq *= lac;
    }
    return val / maxv;
}

/* ─── 9 trous officiels (R&A / USGA) ─────────────────────────────────────── */
void Golf_InitCourse(GolfGame *g) {
    /* Trou 1 – Par 4 – 370 m */
    g->holes[0] = (HoleData){
        .index=1, .par=4, .distance_m=370.0f,
        .tee_pos=(Vector3){0,0,0}, .hole_pos=(Vector3){0,0,370*SCALE},
        .green_center=(Vector3){0,0,370*SCALE}, .green_radius=1.8f,
        .hazard_count=2,
        .hazards={
            {(Vector3){12,0,150*SCALE},8.0f,SURF_SAND},
            {(Vector3){-8,0,280*SCALE},6.0f,SURF_WATER},
        },
        .terrain_seed=42.0f,
        .fairway_color={80,160,60,255}, .rough_color={50,100,40,255},
    };
    /* Trou 2 – Par 3 – 165 m */
    g->holes[1] = (HoleData){
        .index=2, .par=3, .distance_m=165.0f,
        .tee_pos=(Vector3){0,0,0}, .hole_pos=(Vector3){5,0,165*SCALE},
        .green_center=(Vector3){5,0,165*SCALE}, .green_radius=1.5f,
        .hazard_count=3,
        .hazards={
            {(Vector3){-10,0,80*SCALE},20.0f,SURF_WATER},
            {(Vector3){15,0,140*SCALE},6.0f,SURF_SAND},
            {(Vector3){-5,0,160*SCALE},5.0f,SURF_SAND},
        },
        .terrain_seed=137.0f,
        .fairway_color={85,165,55,255}, .rough_color={45,95,35,255},
    };
    /* Trou 3 – Par 5 – 520 m */
    g->holes[2] = (HoleData){
        .index=3, .par=5, .distance_m=520.0f,
        .tee_pos=(Vector3){0,0,0}, .hole_pos=(Vector3){-8,0,520*SCALE},
        .green_center=(Vector3){-8,0,520*SCALE}, .green_radius=2.0f,
        .hazard_count=4,
        .hazards={
            {(Vector3){10,0,160*SCALE},10.0f,SURF_SAND},
            {(Vector3){-12,0,300*SCALE},12.0f,SURF_WATER},
            {(Vector3){8,0,420*SCALE},7.0f,SURF_SAND},
            {(Vector3){-5,0,500*SCALE},5.0f,SURF_SAND},
        },
        .terrain_seed=256.0f,
        .fairway_color={75,155,50,255}, .rough_color={48,98,38,255},
    };
    /* Trou 4 – Par 4 – 390 m */
    g->holes[3] = (HoleData){
        .index=4, .par=4, .distance_m=390.0f,
        .tee_pos=(Vector3){0,0,0}, .hole_pos=(Vector3){6,0,390*SCALE},
        .green_center=(Vector3){6,0,390*SCALE}, .green_radius=1.6f,
        .hazard_count=2,
        .hazards={
            {(Vector3){-14,0,200*SCALE},9.0f,SURF_SAND},
            {(Vector3){10,0,370*SCALE},6.0f,SURF_SAND},
        },
        .terrain_seed=88.0f,
        .fairway_color={82,162,58,255}, .rough_color={52,102,42,255},
    };
    /* Trou 5 – Par 3 – 190 m */
    g->holes[4] = (HoleData){
        .index=5, .par=3, .distance_m=190.0f,
        .tee_pos=(Vector3){0,0,0}, .hole_pos=(Vector3){-3,0,190*SCALE},
        .green_center=(Vector3){-3,0,190*SCALE}, .green_radius=1.7f,
        .hazard_count=2,
        .hazards={
            {(Vector3){8,0,150*SCALE},8.0f,SURF_WATER},
            {(Vector3){-6,0,185*SCALE},4.0f,SURF_SAND},
        },
        .terrain_seed=314.0f,
        .fairway_color={78,158,52,255}, .rough_color={48,98,40,255},
    };
    /* Trou 6 – Par 5 – 545 m */
    g->holes[5] = (HoleData){
        .index=6, .par=5, .distance_m=545.0f,
        .tee_pos=(Vector3){0,0,0}, .hole_pos=(Vector3){4,0,545*SCALE},
        .green_center=(Vector3){4,0,545*SCALE}, .green_radius=2.2f,
        .hazard_count=4,
        .hazards={
            {(Vector3){-8,0,180*SCALE},11.0f,SURF_WATER},
            {(Vector3){12,0,350*SCALE},8.0f,SURF_SAND},
            {(Vector3){-10,0,460*SCALE},9.0f,SURF_SAND},
            {(Vector3){6,0,530*SCALE},5.0f,SURF_SAND},
        },
        .terrain_seed=512.0f,
        .fairway_color={76,154,48,255}, .rough_color={46,94,36,255},
    };
    /* Trou 7 – Par 4 – 410 m */
    g->holes[6] = (HoleData){
        .index=7, .par=4, .distance_m=410.0f,
        .tee_pos=(Vector3){0,0,0}, .hole_pos=(Vector3){-7,0,410*SCALE},
        .green_center=(Vector3){-7,0,410*SCALE}, .green_radius=1.8f,
        .hazard_count=3,
        .hazards={
            {(Vector3){10,0,220*SCALE},8.0f,SURF_SAND},
            {(Vector3){-6,0,330*SCALE},10.0f,SURF_WATER},
            {(Vector3){3,0,400*SCALE},5.0f,SURF_SAND},
        },
        .terrain_seed=199.0f,
        .fairway_color={80,160,55,255}, .rough_color={50,100,42,255},
    };
    /* Trou 8 – Par 3 – 155 m */
    g->holes[7] = (HoleData){
        .index=8, .par=3, .distance_m=155.0f,
        .tee_pos=(Vector3){0,0,0}, .hole_pos=(Vector3){9,0,155*SCALE},
        .green_center=(Vector3){9,0,155*SCALE}, .green_radius=1.4f,
        .hazard_count=3,
        .hazards={
            {(Vector3){-5,0,80*SCALE},7.0f,SURF_WATER},
            {(Vector3){12,0,130*SCALE},6.0f,SURF_SAND},
            {(Vector3){6,0,150*SCALE},4.0f,SURF_SAND},
        },
        .terrain_seed=77.0f,
        .fairway_color={83,163,57,255}, .rough_color={52,103,43,255},
    };
    /* Trou 9 – Par 4 – 430 m */
    g->holes[8] = (HoleData){
        .index=9, .par=4, .distance_m=430.0f,
        .tee_pos=(Vector3){0,0,0}, .hole_pos=(Vector3){10,0,430*SCALE},
        .green_center=(Vector3){10,0,430*SCALE}, .green_radius=2.0f,
        .hazard_count=4,
        .hazards={
            {(Vector3){-8,0,150*SCALE},9.0f,SURF_SAND},
            {(Vector3){14,0,280*SCALE},13.0f,SURF_WATER},
            {(Vector3){-5,0,390*SCALE},7.0f,SURF_SAND},
            {(Vector3){12,0,420*SCALE},5.0f,SURF_SAND},
        },
        .terrain_seed=666.0f,
        .fairway_color={72,148,44,255}, .rough_color={42,88,32,255},
    };
}

/* ─── Démarrage d'un trou ────────────────────────────────────────────────── */
void Golf_StartHole(GolfGame *g, int idx) {
    HoleData *h;
    Vector3   dir;

    g->current_hole = idx;
    h = &g->holes[idx];

    perm_init((unsigned int)(h->terrain_seed * 1000.0f));

    Ball_Init(&g->ball, h->tee_pos);

    /* Choix automatique du club selon distance */
    if      (h->distance_m > 250) g->club = CLUB_DRIVER;
    else if (h->distance_m > 180) g->club = CLUB_WOOD3;
    else if (h->distance_m > 140) g->club = CLUB_IRON5;
    else                          g->club = CLUB_IRON9;

    /* Angle de visée initial vers le trou */
    dir = Vector3Subtract(h->hole_pos, h->tee_pos);
    g->aim_angle = -(atan2f(dir.x, dir.z) * RAD2DEG);

    Game_NewWind(g);
    g->shot_pos_count = 0;
}

/* ─── Hauteur du terrain ─────────────────────────────────────────────────── */
float Golf_GetTerrainHeight(GolfGame *g, float x, float z) {
    HoleData *h;
    float     n, height;
    int       i;

    if (!g_perm_init) return 0.0f;
    h = &g->holes[g->current_hole];

    /* Green plat avec légère ondulation */
    {
        float dg = Vector2Distance((Vector2){x,z},
                                   (Vector2){h->green_center.x, h->green_center.z});
        if (dg < h->green_radius * 1.5f)
            return octave_noise(x*0.3f, z*0.3f, 2, 0.4f, 2.0f) * 0.25f;
    }

    n = octave_noise(x*0.08f, z*0.08f, 4, 0.5f, 2.0f);
    height = n * 3.0f;

    for (i = 0; i < h->hazard_count; i++) {
        float d = Vector2Distance((Vector2){x,z},
                                  (Vector2){h->hazards[i].pos.x, h->hazards[i].pos.z});
        if (d < h->hazards[i].radius) {
            float t = 1.0f - d / h->hazards[i].radius;
            if (h->hazards[i].surface == SURF_WATER) height -= t * 1.5f;
            if (h->hazards[i].surface == SURF_SAND)  height -= t * 0.4f;
        }
    }
    return height;
}

/* ─── Type de surface ────────────────────────────────────────────────────── */
SurfaceType Golf_GetSurface(GolfGame *g, float x, float z) {
    HoleData *h = &g->holes[g->current_hole];
    int i;
    float fw = 12.0f;
    float max_dist;

    /* Green */
    {
        float dg = Vector2Distance((Vector2){x,z},
                                   (Vector2){h->green_center.x, h->green_center.z});
        if (dg < h->green_radius) return SURF_GREEN;
    }

    /* Obstacles */
    for (i = 0; i < h->hazard_count; i++) {
        float d = Vector2Distance((Vector2){x,z},
                                  (Vector2){h->hazards[i].pos.x, h->hazards[i].pos.z});
        if (d < h->hazards[i].radius) return h->hazards[i].surface;
    }

    /* Fairway (couloir tee→green) */
    {
        Vector2 tee  = {h->tee_pos.x,  h->tee_pos.z};
        Vector2 hole = {h->hole_pos.x, h->hole_pos.z};
        Vector2 pt   = {x, z};
        Vector2 AB   = {hole.x - tee.x, hole.y - tee.y};
        float   len2 = AB.x*AB.x + AB.y*AB.y;
        if (len2 > 0.0f) {
            float   t = ((pt.x-tee.x)*AB.x + (pt.y-tee.y)*AB.y) / len2;
            Vector2 proj;
            t = Clamp(t, 0.0f, 1.0f);
            proj = (Vector2){tee.x + t*AB.x, tee.y + t*AB.y};
            if (Vector2Distance(pt, proj) < fw) return SURF_FAIRWAY;
        }
    }

    /* OOB */
    max_dist = h->distance_m * SCALE + 30.0f;
    if (fabsf(x) > 40.0f || z < -10.0f || z > max_dist) return SURF_OOB;

    return SURF_ROUGH;
}

/* ─── Normale du terrain ─────────────────────────────────────────────────── */
Vector3 Golf_GetTerrainNormal(GolfGame *g, float x, float z) {
    float   eps = 0.5f;
    float   hC  = Golf_GetTerrainHeight(g, x,     z    );
    float   hR  = Golf_GetTerrainHeight(g, x+eps, z    );
    float   hF  = Golf_GetTerrainHeight(g, x,     z+eps);
    Vector3 n   = {-(hR-hC)/eps, 1.0f, -(hF-hC)/eps};
    return Vector3Normalize(n);
}

/* ─── Dessin du terrain avec textures ─────────────────────────────────────────── */
#define GRID_N 60

void Golf_DrawTerrain(GolfGame *g) {
    HoleData *h    = &g->holes[g->current_hole];
    float     zmax = h->distance_m * SCALE + 20.0f;
    float     xmin = -40.0f, xmax = 40.0f;
    float     step = (xmax - xmin) / GRID_N;
    float     zstep = zmax / GRID_N;
    int       iz, ix;

    for (iz = 0; iz < GRID_N; iz++) {
        for (ix = 0; ix < GRID_N; ix++) {
            float x0 = xmin + ix * step,  z0 = iz * zstep;
            float x1 = x0 + step,         z1 = z0 + zstep;
            float y00 = Golf_GetTerrainHeight(g, x0, z0);
            float y10 = Golf_GetTerrainHeight(g, x1, z0);
            float y01 = Golf_GetTerrainHeight(g, x0, z1);
            float y11 = Golf_GetTerrainHeight(g, x1, z1);
            float cx  = (x0+x1)*0.5f, cz = (z0+z1)*0.5f;
            float cy  = Golf_GetTerrainHeight(g, cx, cz);
            UNUSED(cy);
            SurfaceType surf = Golf_GetSurface(g, cx, cz);
            
            Texture2D tex;
            switch (surf) {
                case SURF_FAIRWAY: tex = g->tex_fairway; break;
                case SURF_GREEN:   tex = g->tex_green;   break;
                case SURF_SAND:    tex = g->tex_sand;    break;
                case SURF_WATER:  tex = g->tex_water;  break;
                case SURF_ROUGH: tex = g->tex_rough;  break;
                default:        tex = g->tex_rough;  break;
            }
            
            if (tex.id == 0) {
                Color col = h->rough_color;
                switch (surf) {
                    case SURF_FAIRWAY: col = h->fairway_color; break;
                    case SURF_GREEN:   col = (Color){ 60,180, 60,255}; break;
                    case SURF_SAND:    col = (Color){220,200,140,255}; break;
                    case SURF_WATER:   col = (Color){ 40,100,200,200}; break;
                    case SURF_OOB:     col = (Color){120, 80, 50,255}; break;
                    default: break;
                }
                DrawTriangle3D((Vector3){x0,y00,z0}, (Vector3){x0,y01,z1}, (Vector3){x1,y11,z1}, col);
                DrawTriangle3D((Vector3){x0,y00,z0}, (Vector3){x1,y11,z1}, (Vector3){x1,y10,z0}, col);
                continue;
            }
            
            rlSetTexture(tex.id);
            rlBegin(RL_QUADS);
                rlColor4ub(255, 255, 255, 255);
                rlTexCoord2f(0, 0); rlVertex3f(x0, y00, z0);
                rlTexCoord2f(1, 0); rlVertex3f(x0, y01, z1);
                rlTexCoord2f(1, 1); rlVertex3f(x1, y11, z1);
                rlTexCoord2f(0, 1); rlVertex3f(x1, y10, z0);
            rlEnd();
            rlSetTexture(0);
        }
    }
}

/* ─── Drapeau ────────────────────────────────────────────────────────────── */
void Golf_DrawFlag(GolfGame *g) {
    HoleData *h   = &g->holes[g->current_hole];
    Vector3   base = h->hole_pos;
    Vector3   top, tip, bot;
    base.y = Golf_GetTerrainHeight(g, base.x, base.z);

    DrawCylinder((Vector3){base.x, base.y, base.z}, 0.02f, 0.02f, FLAG_H, 6, WHITE);

    top = (Vector3){base.x,        base.y + FLAG_H,        base.z};
    tip = (Vector3){base.x + 1.5f, base.y + FLAG_H - 0.8f, base.z};
    bot = (Vector3){base.x,        base.y + FLAG_H - 1.4f, base.z};
    DrawTriangle3D(top, bot, tip, RED);
    DrawTriangle3D(top, tip, bot, RED);
}

/* ─── Trou ───────────────────────────────────────────────────────────────── */
void Golf_DrawHoleCup(GolfGame *g) {
    HoleData *h   = &g->holes[g->current_hole];
    Vector3   pos = h->hole_pos;
    pos.y = Golf_GetTerrainHeight(g, pos.x, pos.z) - 0.05f;
    DrawCylinder(pos, HOLE_R, HOLE_R, 0.15f, 16, BLACK);
    DrawCylinderWires((Vector3){pos.x, pos.y+0.15f, pos.z},
                      HOLE_R+0.02f, HOLE_R+0.02f, 0.01f, 16, WHITE);
}

/* ─── Arbres ─────────────────────────────────────────────────────────────── */

/**
    @brief Utility to darken a color by a factor.
    @param[in] c Original color.
    @param[in] f Factor [0, 1].
    @return Darkened color.
*/
static Color col_darken(Color c, float f) {
    return (Color){
        (unsigned char)Clamp(c.r*f, 0, 255),
        (unsigned char)Clamp(c.g*f, 0, 255),
        (unsigned char)Clamp(c.b*f, 0, 255),
        c.a
    };
}

/**
    @brief Utility to lighten a color by adding an amount to RGB.
    @param[in] c Original color.
    @param[in] a Amount to add.
    @return Lightened color.
*/
static Color col_lighten(Color c, float a) {
    return (Color){
        (unsigned char)Clamp(c.r + a*255, 0, 255),
        (unsigned char)Clamp(c.g + a*255, 0, 255),
        (unsigned char)Clamp(c.b + a*255, 0, 255),
        c.a
    };
}

/**
    @brief Renders a stylized tree at (x, z).
    @param[in] x      World X coordinate.
    @param[in] z      World Z coordinate.
    @param[in] height Total height of the tree.
    @param[in] leaf   Base color of the leaves.
*/
static void draw_tree(float x, float z, float height, Color leaf) {
    float th = height * 0.45f;
    float tr = height * 0.04f;
    float cr = height * 0.38f;
    DrawCylinder((Vector3){x,0,z}, tr*0.7f, tr, th, 6, (Color){90,60,35,255});
    DrawSphere((Vector3){x,         th+cr*0.55f, z         }, cr,        leaf);
    DrawSphere((Vector3){x+tr*2,    th+cr*0.25f, z-tr      }, cr*0.72f,  col_darken(leaf, 0.85f));
    DrawSphere((Vector3){x-tr*2,    th+cr*0.15f, z+tr*1.5f }, cr*0.65f,  col_lighten(leaf, 0.05f));
}

/**
    @brief Renders a group of trees around a center point.
    @param[in] cx    Center X coordinate.
    @param[in] cz    Center Z coordinate.
    @param[in] seed  Random seed for variation.
    @param[in] min_h Minimum tree height.
    @param[in] max_h Maximum tree height.
*/
static void draw_grove(float cx, float cz, float seed, float min_h, float max_h) {
    static const float offX[5] = { 0.0f,  2.2f, -1.8f,  1.0f, -2.5f};
    static const float offZ[5] = { 0.0f,  1.5f,  2.4f, -2.0f, -1.2f};
    Color greens[5] = {
        {34,120,34,255},{45,140,38,255},{28,105,28,255},
        {55,130,45,255},{38,115,30,255}
    };
    int n = 3 + (int)(seed * 7.3f) % 3;
    int i;
    for (i = 0; i < n; i++) {
        float frac = seed*(i+1)*3.7f;
        float h = min_h + (frac - floorf(frac)) * (max_h - min_h);
        draw_tree(cx + offX[i], cz + offZ[i], h, greens[i % 5]);
    }
}

/* ─── Obstacles + déco ───────────────────────────────────────────────────── */
void Golf_DrawHazards(GolfGame *g) {
    HoleData *h  = &g->holes[g->current_hole];
    float     t  = (float)GetTime();
    int       i;

    /* Nuages */
    {
        float     zc     = h->distance_m * SCALE * 0.5f;
        Color     sky    = {140,200,240,30};
        Color     sky2   = {100,160,220,60};
        int       c;
        for (c = 0; c < 4; c++) {
            float cx2 = -20.0f + c*15.0f;
            float cz2 = zc*0.3f + c*12.0f;
            float cy2 = 18.0f + (float)(c%3)*4.0f;
            DrawSphere((Vector3){cx2,         cy2,      cz2}, 4.0f+c*0.8f, sky);
            DrawSphere((Vector3){cx2+3.5f,    cy2-1.0f, cz2}, 3.0f,        sky2);
        }
    }

    /* Obstacles */
    for (i = 0; i < h->hazard_count; i++) {
        Hazard *hz = &h->hazards[i];

        if (hz->surface == SURF_WATER) {
            float wy = Golf_GetTerrainHeight(g, hz->pos.x, hz->pos.z) + 0.04f;
            Color wc = {
                40,
                (unsigned char)(100 + (int)(sinf(t*0.8f+(float)i)*15)),
                (unsigned char)(200 + (int)(cosf(t*0.5f)*10)),
                200
            };
            int r;
            DrawCylinder((Vector3){hz->pos.x, wy, hz->pos.z},
                         hz->radius, hz->radius, 0.06f, 24, wc);
            /* Roseaux */
            for (r = 0; r < 6; r++) {
                float ang = (float)r/6.0f*6.28318f;
                float rx2 = hz->pos.x + cosf(ang)*(hz->radius+0.3f);
                float rz2 = hz->pos.z + sinf(ang)*(hz->radius+0.3f);
                float ry  = Golf_GetTerrainHeight(g, rx2, rz2);
                float rh  = 0.5f + sinf(ang*3.0f)*0.2f;
                DrawCylinder((Vector3){rx2,ry,rz2}, 0.02f,0.01f,rh,4,
                             (Color){80,130,60,200});
            }
        }

        if (hz->surface == SURF_SAND) {
            float by = Golf_GetTerrainHeight(g, hz->pos.x, hz->pos.z);
            int   r;
            DrawCylinder((Vector3){hz->pos.x, by-0.05f, hz->pos.z},
                         hz->radius+0.15f, hz->radius+0.15f, 0.08f, 20,
                         (Color){200,185,130,160});
            for (r = 0; r < 12; r++) {
                float ang  = (float)r/12.0f*6.28318f;
                float bx2  = hz->pos.x + cosf(ang)*hz->radius;
                float bz2  = hz->pos.z + sinf(ang)*hz->radius;
                float lipy = Golf_GetTerrainHeight(g, bx2, bz2);
                DrawSphere((Vector3){bx2, lipy+0.04f, bz2}, 0.12f,
                           (Color){200,185,130,200});
            }
        }
    }

    /* Bosquets */
    {
        float zmax = h->distance_m * SCALE;
        float seed = h->terrain_seed;
        int   gc   = 4 + g->current_hole % 3;
        int   gi;
        for (gi = 0; gi < gc; gi++) {
            float s = seed*(gi+1)*0.137f;
            float s2;
            s -= floorf(s);
            draw_grove(-16.0f - s*6.0f, (zmax/gc)*gi + s*5.0f,
                       seed+gi, 2.5f, 5.0f);
            s2 = seed*(gi+2)*0.213f;
            s2 -= floorf(s2);
            draw_grove(16.0f + s2*6.0f, (zmax/gc)*gi + s2*4.0f,
                       seed+gi+0.5f, 2.0f, 4.5f);
        }
        draw_grove(h->green_center.x+5.0f,
                   h->green_center.z+h->green_radius+4.0f,
                   seed*2.3f, 3.5f, 6.0f);
    }

    /* Poteaux OOB */
    {
        float zmax = h->distance_m * SCALE + 5.0f;
        float ox   = 39.0f;
        int   pc   = (int)(zmax / 8.0f);
        int   p;
        for (p = 0; p <= pc; p++) {
            float pz   = (float)p * 8.0f;
            float py_l = Golf_GetTerrainHeight(g, -ox, pz);
            float py_r = Golf_GetTerrainHeight(g,  ox, pz);
            DrawCylinder((Vector3){-ox,py_l,pz}, 0.04f,0.04f,0.9f,4,WHITE);
            DrawCylinder((Vector3){ ox,py_r,pz}, 0.04f,0.04f,0.9f,4,WHITE);
        }
    }
}
