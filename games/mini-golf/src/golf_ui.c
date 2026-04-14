/**
    @file golf_ui.c
    @author Maxime CHAUVEAU
    @date 2026-04-14
    @date 2026-04-14
    @brief UI rendering for the mini-golf game.
*/
#include "golf.h"

/* ─── Helpers ────────────────────────────────────────────────────────────── */

/**
    @brief Draws a rounded rectangle panel with a background and border.
    @param[in] x      X coordinate.
    @param[in] y      Y coordinate.
    @param[in] w      Width.
    @param[in] h      Height.
    @param[in] bg     Background color.
    @param[in] border Border color.
*/
static void draw_panel(float x, float y, float w, float h,
                       Color bg, Color border) {
    DrawRectangleRounded((Rectangle){x,y,w,h}, 0.15f, 8, bg);
    DrawRectangleRoundedLines((Rectangle){x,y,w,h}, 0.15f, 8, border);
}

/**
    @brief Draws horizontally centered text with a shadow.
    @param[in] txt  Text string.
    @param[in] y    Y coordinate.
    @param[in] sz   Font size.
    @param[in] c    Text color.
*/
static void draw_text_center(const char *txt, int y, int sz, Color c) {
    int sw = GetScreenWidth();
    int tw = MeasureText(txt, sz);
    DrawText(txt, sw/2 - tw/2 + 1, y+1, sz, (Color){0,0,0,100});
    DrawText(txt, sw/2 - tw/2,     y,   sz, c);
}

/**
    @brief Draws text with a shadow at a specific position.
    @param[in] txt  Text string.
    @param[in] x    X coordinate.
    @param[in] y    Y coordinate.
    @param[in] sz   Font size.
    @param[in] c    Text color.
*/
static void draw_text_sh(const char *txt, int x, int y, int sz, Color c) {
    DrawText(txt, x+1, y+1, sz, (Color){0,0,0,120});
    DrawText(txt, x,   y,   sz, c);
}

/* ─── Indicateur vent ────────────────────────────────────────────────────── */

/**
    @brief Draws the wind indicator (compass and speed).
    @param[in] g Golf game state.
*/
void UI_DrawWindIndicator(GolfGame *g) {
    int   cx = 120, cy = 90, r = 40;
    float wr, wx, wy, spd_norm;
    Color wc;
    char  buf[24];

    draw_panel(cx-r-15, cy-r-20, (r+15)*2, (r+20)*2+30,
               (Color){0,0,0,140}, (Color){255,255,255,60});
    DrawCircle(cx, cy, (float)r, (Color){20,40,80,180});
    DrawCircleLines(cx, cy, (float)r, (Color){100,150,255,100});

    wr       = g->wind.direction * DEG2RAD;
    wx       = sinf(wr) * r * 0.8f;
    wy       = -cosf(wr) * r * 0.8f;
    spd_norm = Clamp(g->wind.speed / 40.0f, 0.0f, 1.0f);
    wc       = ColorFromHSV(120.0f*(1.0f-spd_norm), 0.8f, 1.0f);

    DrawLineEx((Vector2){(float)cx,(float)cy},
               (Vector2){(float)(cx+(int)wx),(float)(cy+(int)wy)}, 2.5f, wc);
    DrawText("N", cx-5,      cy-r-18, 12, WHITE);
    DrawText("S", cx-4,      cy+r+4,  12, WHITE);
    DrawText("E", cx+r+4,    cy-6,    12, WHITE);
    DrawText("O", cx-r-16,   cy-6,    12, WHITE);

    snprintf(buf, sizeof(buf), "%.0f km/h", g->wind.speed);
    draw_text_sh(buf, cx-25, cy+r+18, 14, wc);
}

/* ─── Jauge de puissance ─────────────────────────────────────────────────── */

/**
    @brief Draws the power meter during the shot preparation.
    @param[in] g Golf game state.
*/
void UI_DrawPower(GolfGame *g) {
    int   sw = g->screen_w, sh = g->screen_h;
    int   bw = 28, bh = 240;
    int   bx, by, fill;
    Color pc;
    char  buf[8];

    if (g->state != STATE_POWER) return;

    bx = sw - 80;
    by = sh/2 - bh/2;

    draw_panel(bx-10, by-30, bw+20, bh+60,
               (Color){0,0,0,160}, (Color){255,255,255,80});
    DrawRectangle(bx, by, bw, bh, (Color){30,30,30,200});

    fill = (int)(g->power * bh);
    pc   = ColorFromHSV(120.0f*(1.0f-g->power), 0.85f, 0.95f);
    if (g->power > 0.85f) pc = RED;
    DrawRectangle(bx, by+bh-fill, bw, fill, pc);

    DrawRectangle(bx-4, by,       bw+8, 3, (Color){255,50,50,220});
    DrawRectangle(bx-4, by+bh/4,  bw+8, 2, (Color){255,200,0,160});
    DrawRectangleLines(bx, by, bw, bh, WHITE);

    snprintf(buf, sizeof(buf), "%d%%", (int)(g->power*100));
    DrawText(buf, bx-2, by+bh+10, 16, WHITE);
    draw_text_sh("FORCE", bx-5, by-22, 14, (Color){200,200,200,255});
}

/* ─── HUD ────────────────────────────────────────────────────────────────── */

/**
    @brief Draws the main in-game HUD (strokes, par, distance, etc.).
    @param[in] g Golf game state.
*/
void UI_DrawHUD(GolfGame *g) {
    HoleData   *h;
    int         sw;
    char        buf[64];
    int         strokes, to_par;

    if (g->state == STATE_MENU || g->state == STATE_SCORECARD) return;

    h       = &g->holes[g->current_hole];
    sw      = g->screen_w;
    strokes = g->ball.strokes + g->ball.penalty;
    to_par  = g->score.to_par;

    /* Trou – haut centre */
    {
        int tw;
        snprintf(buf, sizeof(buf), "TROU %d/%d  •  PAR %d  •  %.0f m",
                 h->index, MAX_HOLES, h->par, h->distance_m);
        tw = MeasureText(buf, 20);
        draw_panel(sw/2-tw/2-12, 6, tw+24, 32,
                   (Color){0,0,0,160}, (Color){255,255,255,60});
        draw_text_sh(buf, sw/2-tw/2, 12, 20, WHITE);
    }

    /* Coups */
    snprintf(buf, sizeof(buf), "Coups : %d", strokes);
    draw_panel(sw-150, 6, 144, 32, (Color){0,0,0,160}, (Color){255,255,255,60});
    draw_text_sh(buf, sw-140, 12, 20, WHITE);

    /* Score au par */
    {
        int   diff_cur = (strokes > 0) ? strokes - h->par : 0;
        Color sc       = score_color(to_par + diff_cur);
        snprintf(buf, sizeof(buf), "%+d", to_par + diff_cur);
        draw_panel(sw-150, 44, 144, 32, (Color){0,0,0,160}, (Color){255,255,255,60});
        draw_text_sh(buf, sw-80-MeasureText(buf,20)/2, 50, 20, sc);
    }

    /* Club */
    draw_panel(sw-150, 82, 144, 28, (Color){0,0,0,160}, (Color){255,255,255,60});
    draw_text_sh(CLUBS[g->club].name, sw-140, 88, 16, (Color){255,220,100,255});

    /* Distance au trou */
    {
        float dist = Vector3Distance(g->ball.pos, h->hole_pos) / SCALE * 10.0f;
        snprintf(buf, sizeof(buf), "%.0f m", dist);
        draw_panel(sw-150, 116, 144, 28, (Color){0,0,0,160}, (Color){255,255,255,60});
        draw_text_sh(buf, sw-140, 122, 16, (Color){150,220,255,255});
    }

    /* Surface */
    {
        const char *snames[] = {"Fairway","Rough","Green","Sable","Eau","Hors-limites"};
        Color scols[] = {
            {100,200, 80,255},{80,140,50,255},{60,220,100,255},
            {220,190,120,255},{80,150,240,255},{200,80,80,255}
        };
        int si = (int)g->ball.surface;
        if (si >= 0 && si < 6) {
            draw_panel(sw-150, 150, 144, 24, (Color){0,0,0,140}, (Color){255,255,255,40});
            draw_text_sh(snames[si], sw-140, 155, 14, scols[si]);
        }
    }

    /* Vitesse + hauteur en vol */
    if (g->ball.state == BALL_FLYING) {
        float spd = Vector3Length(g->ball.vel);
        float alt = g->ball.pos.y - Golf_GetTerrainHeight(g, g->ball.pos.x, g->ball.pos.z);
        int   bw2;

        snprintf(buf, sizeof(buf), "%.0f km/h", spd*36.0f);
        bw2 = MeasureText(buf, 22);
        draw_panel(sw/2-bw2/2-10, 46, bw2+20, 30,
                   (Color){0,0,0,160}, (Color){255,200,0,100});
        draw_text_sh(buf, sw/2-bw2/2, 51, 22, (Color){255,220,80,255});

        snprintf(buf, sizeof(buf), "alt %.1f m", alt/SCALE*10.0f);
        bw2 = MeasureText(buf, 15);
        draw_panel(sw/2-bw2/2-6, 82, bw2+12, 22,
                   (Color){0,0,0,140}, (Color){200,200,255,80});
        draw_text_sh(buf, sw/2-bw2/2, 85, 15, (Color){180,200,255,255});
    }

    /* Sur le Green */
    if (g->ball.surface == SURF_GREEN && g->ball.state == BALL_IDLE) {
        float  pulse = 0.6f + sinf((float)GetTime()*3.0f)*0.4f;
        Color  gc    = (Color){60,220,100,(unsigned char)(200*pulse)};
        int    gtw;
        const char *gt = "* SUR LE GREEN *";
        gtw = MeasureText(gt, 20);
        draw_panel(sw/2-gtw/2-14, 46, gtw+28, 32,
                   (Color){0,60,20,200}, (Color){60,220,100,180});
        draw_text_sh(gt, sw/2-gtw/2, 52, 20, gc);
    }

    /* Pénalités */
    if (g->ball.penalty > 0) {
        int pw;
        snprintf(buf, sizeof(buf), "+%d penalite(s)", g->ball.penalty);
        pw = MeasureText(buf, 15);
        draw_panel(sw/2-pw/2-10, 88, pw+20, 26,
                   (Color){80,0,0,180}, (Color){255,80,80,160});
        draw_text_sh(buf, sw/2-pw/2, 93, 15, (Color){255,100,100,255});
    }

    UI_DrawWindIndicator(g);
    UI_DrawMinimap(g);
    UI_DrawClubSelector(g);

    if (g->state == STATE_AIMING)
        DrawText("< > Viser | 1-6 Club | Tab Club suivant | Espace Tirer | T Trajectoire | F1-F4 Camera | Echap Pause",
                 10, g->screen_h-22, 12, (Color){200,200,200,160});
    if (g->state == STATE_POWER)
        DrawText("Espace / Clic  ->  Frapper  |  Retour  ->  Annuler",
                 10, g->screen_h-22, 14, (Color){255,220,100,200});
}

/* ─── Minimap ────────────────────────────────────────────────────────────── */

/**
    @brief Draws the 2D minimap showing the hole layout and ball position.
    @param[in] g Golf game state.
*/
void UI_DrawMinimap(GolfGame *g) {
    HoleData *h   = &g->holes[g->current_hole];
    int       mx  = 10, my = g->screen_h-175, mw = 130, mh = 155;
    float     zmax  = h->distance_m * SCALE + 10.0f;
    float     mg    = 8.0f;
    float     mmw   = mw - mg*2;
    float     mmh   = mh - mg*2 - 18;
    float     mmx   = mx + mg;
    float     mmy   = my + mg + 16;
    int       i;

    #define MX(wx) ((mmx) + ((wx)+40.0f)/80.0f*(mmw))
    #define MZ(wz) ((mmy) + (wz)/zmax*(mmh))

    draw_panel(mx, my, mw, mh, (Color){0,0,0,180}, (Color){255,255,255,80});
    DrawText("TROU", mx+38, my+4, 13, (Color){200,200,200,200});
    DrawRectangle((int)mmx,(int)mmy,(int)mmw,(int)mmh,(Color){60,120,40,200});

    for (i = 0; i < h->hazard_count; i++) {
        Hazard *hz = &h->hazards[i];
        Color   hc = (hz->surface==SURF_WATER)  ? (Color){40,100,220,200} :
                     (hz->surface==SURF_SAND)   ? (Color){220,190,120,200} :
                                                   (Color){100,50,50,200};
        DrawCircle((int)MX(hz->pos.x),(int)MZ(hz->pos.z),
                   hz->radius*mmw/80.0f, hc);
    }
    /* Green */
    DrawCircle((int)MX(h->green_center.x),(int)MZ(h->green_center.z),
               h->green_radius*mmw/80.0f,(Color){80,200,80,220});
    /* Tee */
    DrawCircle((int)MX(h->tee_pos.x),(int)MZ(h->tee_pos.z), 4, WHITE);
    /* Drapeau */
    DrawCircle((int)MX(h->hole_pos.x),(int)MZ(h->hole_pos.z), 3, RED);
    /* Balle */
    DrawCircle((int)MX(g->ball.pos.x),(int)MZ(g->ball.pos.z), 4, YELLOW);
    /* Trace tir */
    for (i = 1; i < g->shot_pos_count; i += 3)
        DrawPixel((int)MX(g->shot_positions[i].x),
                  (int)MZ(g->shot_positions[i].z),
                  (Color){255,255,0,100});

    #undef MX
    #undef MZ
}

/* ─── Sélecteur club ─────────────────────────────────────────────────────── */

/**
    @brief Draws the club selection bar at the bottom of the screen.
    @param[in] g Golf game state.
*/
void UI_DrawClubSelector(GolfGame *g) {
    int cx, cy, cw, ch, gap, total_w, sx, i;

    if (g->state != STATE_AIMING && g->state != STATE_POWER) return;

    cx      = g->screen_w / 2;
    cy      = g->screen_h - 58;
    cw      = 90; ch = 36; gap = 4;
    total_w = CLUB_COUNT*(cw+gap) - gap;
    sx      = cx - total_w/2;

    for (i = 0; i < CLUB_COUNT; i++) {
        int   x        = sx + i*(cw+gap);
        bool  selected = (i == (int)g->club);
        Color bg       = selected ? (Color){255,200,50,220} : (Color){0,0,0,160};
        Color fg       = selected ? BLACK : WHITE;
        int   tw;
        DrawRectangleRounded((Rectangle){(float)x,(float)(cy-ch/2),(float)cw,(float)ch},
                             0.3f, 6, bg);
        if (selected)
            DrawRectangleRoundedLines(
                (Rectangle){(float)x,(float)(cy-ch/2),(float)cw,(float)ch},
                0.3f, 6, WHITE);
        tw = MeasureText(CLUBS[i].name, 13);
        DrawText(CLUBS[i].name, x+cw/2-tw/2, cy-7, 13, fg);
    }
}

/* ─── Menu principal ─────────────────────────────────────────────────────── */

/**
    @brief Draws the main menu screen.
    @param[in,out] g Golf game state.
*/
void UI_DrawMenu(GolfGame *g) {
    int   sw = g->screen_w, sh = g->screen_h;
    float t  = (float)GetTime();
    float pulse, btn_y, btn_w, btn_h;
    bool  hov;
    int   i;

    DrawRectangleGradientV(0,0,sw,sh,(Color){10,60,20,255},(Color){5,30,10,255});

    /* Titre */
    pulse = 1.0f + sinf(t*1.5f)*0.025f;
    {
        int   fsz = (int)(70*pulse);
        int   tw;
        const char *title = "GOLF 3D";
        tw = MeasureText(title, fsz);
        DrawText(title, sw/2-tw/2+2, sh/4+2,   fsz, (Color){0,0,0,80});
        DrawText(title, sw/2-tw/2,   sh/4,      fsz, (Color){255,215,0,255});
    }

    /* Sous-titre */
    draw_text_center("Parcours officiel 9 trous  R&A / USGA", sh/4+85, 17,
                     (Color){180,220,160,200});

    /* Encart infos */
    draw_panel(sw/2-220, sh/2-55, 440, 110,
               (Color){0,0,0,140}, (Color){255,215,0,100});
    {
        const char *info[] = {
            "9 trous  •  Par 35  •  3175 m",
            "Driver, Bois 3, Fer 5, Fer 9, Wedge, Putter",
            "Bunkers, lacs, rough, vent dynamique"
        };
        for (i = 0; i < 3; i++) {
            int iw = MeasureText(info[i], 16);
            DrawText(info[i], sw/2-iw/2, sh/2-40+i*30, 16, (Color){200,230,200,230});
        }
    }

    /* Bouton Jouer */
    btn_y = sh * 0.72f;
    btn_w = 220; btn_h = 56;
    hov   = CheckCollisionPointRec(GetMousePosition(),
                (Rectangle){(float)sw/2-btn_w/2, btn_y, btn_w, btn_h});
    DrawRectangleRounded((Rectangle){(float)sw/2-btn_w/2,btn_y,btn_w,btn_h},
                         0.3f,10, hov?(Color){255,200,0,255}:(Color){200,160,20,255});
    DrawRectangleRoundedLines((Rectangle){(float)sw/2-btn_w/2,btn_y,btn_w,btn_h},
                              0.3f,10,WHITE);
    {
        const char *bt = "JOUER";
        int          bw = MeasureText(bt,28);
        DrawText(bt, sw/2-bw/2, (int)btn_y+14, 28, hov?BLACK:WHITE);
    }

    DrawText("< > Viser | Espace Tirer | Tab Club | Echap Pause",
             10, sh-28, 13, (Color){150,180,150,180});

    if (hov && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        g->state       = STATE_HOLE_INTRO;
        g->intro_timer = 0.0f;
        Golf_StartHole(g, 0);
        GCam_SetMode(&g->gcam, CAM_HOLE);
    }
}

/* ─── Intro trou ─────────────────────────────────────────────────────────── */

/**
    @brief Draws the introductory screen for a new hole.
    @param[in] g Golf game state.
*/
void UI_DrawHoleIntro(GolfGame *g) {
    HoleData   *h  = &g->holes[g->current_hole];
    int         sw = g->screen_w, sh = g->screen_h;
    float       alpha;
    char        buf[32];

    alpha = Clamp(g->intro_timer * 2.0f, 0.0f, 1.0f);
    if (g->intro_timer > 2.5f)
        alpha = Clamp((3.5f - g->intro_timer) * 2.0f, 0.0f, 1.0f);

    DrawRectangle(0,0,sw,sh,(Color){0,0,0,(unsigned char)(180*alpha)});

    snprintf(buf, sizeof(buf), "TROU %d", h->index);
    draw_text_center(buf, sh/2-80, 64, (Color){255,215,0,(unsigned char)(255*alpha)});

    snprintf(buf, sizeof(buf), "PAR %d  •  %.0f metres", h->par, h->distance_m);
    draw_text_center(buf, sh/2, 24, (Color){255,255,255,(unsigned char)(255*alpha)});

    {
        const char *diffs[] = {"Facile","Court","Long","Moyen","Court",
                               "Long","Moyen","Court","Difficile"};
        draw_text_center(diffs[g->current_hole], sh/2+38, 18,
                         (Color){180,220,160,(unsigned char)(200*alpha)});
    }
}

/* ─── Résultat trou ──────────────────────────────────────────────────────── */

/**
    @brief Draws the result screen after finishing a hole.
    @param[in] g Golf game state.
*/
void UI_DrawHoleResult(GolfGame *g) {
    HoleData   *h       = &g->holes[g->current_hole];
    int         sw      = g->screen_w, sh = g->screen_h;
    int         strokes = g->ball.strokes + g->ball.penalty;
    int         diff    = strokes - h->par;
    float       alpha   = Clamp(g->state_timer * 1.5f, 0.0f, 1.0f);
    int         cx = sw/2, cy = sh/2;
    Color       sc;
    char        buf[48];

    DrawRectangle(0,0,sw,sh,(Color){0,0,0,(unsigned char)(160*alpha)});
    draw_panel(cx-180, cy-110, 360, 220,
               (Color){0,20,40,(unsigned char)(230*alpha)},
               (Color){255,215,0,(unsigned char)(200*alpha)});

    sc   = score_color(diff);
    sc.a = (unsigned char)(255*alpha);

    {
        int tw = MeasureText(score_name(diff), 40);
        DrawText(score_name(diff), cx-tw/2, cy-70, 40, sc);
    }

    snprintf(buf, sizeof(buf), "%d coups  (par %d)", strokes, h->par);
    draw_text_center(buf, cy-18, 20, (Color){255,255,255,(unsigned char)(220*alpha)});

    snprintf(buf, sizeof(buf), "%+d au par", diff);
    draw_text_center(buf, cy+14, 18, sc);

    if (g->ball.penalty > 0) {
        snprintf(buf, sizeof(buf), "%d penalite(s)", g->ball.penalty);
        draw_text_center(buf, cy+44, 14,
                         (Color){255,120,120,(unsigned char)(200*alpha)});
    }

    {
        const char *cont = (g->current_hole < MAX_HOLES-1) ?
                           "Espace - Trou suivant" : "Espace - Voir le score";
        draw_text_center(cont, cy+80, 16,
                         (Color){180,220,255,(unsigned char)(180*alpha)});
    }
}

/* ─── Scorecard ──────────────────────────────────────────────────────────── */

/**
    @brief Draws the complete scorecard for all holes.
    @param[in,out] g Golf game state.
*/
void UI_DrawScorecard(GolfGame *g) {
    int  sw = g->screen_w, sh = g->screen_h;
    int  tx = sw/2 - 400, ty = 100;
    int  i;
    int  total_strokes = 0, total_par = 0;
    char buf[48];

    DrawRectangleGradientV(0,0,sw,sh,(Color){5,20,40,255},(Color){10,40,80,255});
    draw_text_center("CARTE DE SCORE", 30, 36, (Color){255,215,0,255});

    /* En-tête */
    {
        const char *hdrs[] = {"TROU","PAR","DISTANCE","COUPS","+/-","RESULTAT"};
        int cw[] = {55,55,115,75,75,80};
        int cx   = tx;
        int j;
        draw_panel(tx-10, ty-5, 800, 28, (Color){0,50,100,200}, (Color){100,150,255,100});
        for (j = 0; j < 6; j++) {
            DrawText(hdrs[j], cx, ty, 13, (Color){200,220,255,220});
            cx += cw[j]+8;
        }
    }

    for (i = 0; i < MAX_HOLES; i++) {
        HoleData *h      = &g->holes[i];
        int       row_y  = ty + 30 + i*30;
        int       s      = g->score.strokes[i];
        bool      played = (s != SCORE_HOLE_OUT && s > 0);
        int       cx     = tx;
        int       cw[]   = {55,55,115,75,75,80};

        DrawRectangle(tx-10, row_y-4, 800, 28,
                      (i%2==0)?(Color){0,0,0,80}:(Color){255,255,255,20});

        snprintf(buf, sizeof(buf), "%d", h->index);
        DrawText(buf, cx, row_y, 15, WHITE); cx += cw[0]+8;

        snprintf(buf, sizeof(buf), "%d", h->par);
        DrawText(buf, cx, row_y, 15, (Color){180,220,180,255}); cx += cw[1]+8;

        snprintf(buf, sizeof(buf), "%.0f m", h->distance_m);
        DrawText(buf, cx, row_y, 15, (Color){180,180,220,255}); cx += cw[2]+8;

        if (played) {
            total_strokes += s; total_par += h->par;
            snprintf(buf, sizeof(buf), "%d", s);
        } else {
            snprintf(buf, sizeof(buf), "-");
        }
        DrawText(buf, cx, row_y, 15, played?WHITE:(Color){100,100,100,200});
        cx += cw[3]+8;

        if (played) {
            int d = s - h->par;
            snprintf(buf, sizeof(buf), "%+d", d);
            DrawText(buf, cx, row_y, 15, score_color(d));
        } else {
            DrawText("-", cx, row_y, 15, (Color){100,100,100,200});
        }
        cx += cw[4]+8;

        if (played) {
            int d = s - h->par;
            DrawText(score_name(d), cx, row_y, 13, score_color(d));
        }
    }

    /* Total */
    {
        int tot_y = ty + 30 + MAX_HOLES*30 + 8;
        int d     = total_strokes - total_par;
        draw_panel(tx-10, tot_y, 800, 34,
                   (Color){0,50,100,200}, (Color){255,215,0,180});
        DrawText("TOTAL", tx, tot_y+10, 17, WHITE);
        snprintf(buf, sizeof(buf), "%d / par %d", total_strokes, total_par);
        draw_text_center(buf, tot_y+9, 19, (Color){255,215,0,255});
        snprintf(buf, sizeof(buf), "%+d", d);
        {
            int bw = MeasureText(buf,21);
            DrawText(buf, tx+800-bw-20, tot_y+7, 21, score_color(d));
        }
    }

    /* Bouton rejouer */
    {
        float btn_y = (float)sh-68;
        float btn_w = 200, btn_h = 48;
        bool  hov   = CheckCollisionPointRec(GetMousePosition(),
                          (Rectangle){(float)sw/2-btn_w/2, btn_y, btn_w, btn_h});
        DrawRectangleRounded((Rectangle){(float)sw/2-btn_w/2,btn_y,btn_w,btn_h},
                             0.3f,8, hov?(Color){255,200,0,255}:(Color){50,100,200,220});
        draw_text_center("REJOUER", (int)btn_y+12, 24, hov?BLACK:WHITE);
        if (hov && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            int j;
            for (j = 0; j < MAX_HOLES; j++) g->score.strokes[j] = SCORE_HOLE_OUT;
            g->score.total  = 0;
            g->score.to_par = 0;
            g->state        = STATE_MENU;
        }
    }
}

/* ─── Pause ──────────────────────────────────────────────────────────────── */

/**
    @brief Draws the pause overlay.
    @param[in] g Golf game state.
*/
void UI_DrawPaused(GolfGame *g) {
    DrawRectangle(0,0,g->screen_w,g->screen_h,(Color){0,0,0,140});
    draw_text_center("PAUSE  —  Echap pour reprendre",
                     g->screen_h/2-14, 28, WHITE);
}
