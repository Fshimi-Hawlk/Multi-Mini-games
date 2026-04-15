/**
    @file paramsMenu.c
    @author Multi Mini-Games Team
    @date 2026-04-14
    @date 2026-04-14
    @brief paramsMenu.c implementation/header file
*/
#include "paramsMenu.h"


#include "logger.h"
#include "systemSettings.h"

// ────────────────────────────────────────────────
// Layout constants
// ────────────────────────────────────────────────

#define PM_GEAR_SIZE     32     ///< Gear button side length (pixels)
#define PM_GEAR_MARGIN   10     ///< Gear button distance from screen edge

#define PM_MENU_W        264    ///< Settings panel width
#define PM_PAD           14     ///< Inner horizontal padding
#define PM_TITLE_H       40     ///< Height of the title bar
#define PM_SECTION_H     26     ///< Height of a section label row
#define PM_ROW_H         40     ///< Height of a standard control row
#define PM_SEP           8      ///< Vertical space between sections (each side)

#define PM_SLIDER_H      8      ///< Slider track height
#define PM_HANDLE_R      7      ///< Slider handle radius
#define PM_TOGGLE_W      44     ///< Toggle switch width
#define PM_TOGGLE_H      22     ///< Toggle switch height
#define PM_ARROW_W       28     ///< Cycle arrow button width

// Total panel height (computed from sections above)
//   Title + [res] header+row + sep + [audio] header+row+row + sep + [disp] header+row + bottom
#define PM_MENU_H  (PM_TITLE_H \
                  + PM_SECTION_H + PM_ROW_H \
                  + (PM_SEP * 2 + 1) \
                  + PM_SECTION_H + PM_ROW_H + PM_ROW_H \
                  + (PM_SEP * 2 + 1) \
                  + PM_SECTION_H + PM_ROW_H \
                  + PM_SEP)

// Roundness value for DrawRectangleRounded (ratio of shortest side / 2)
#define PM_ROUND     0.10f
#define PM_ROUND_SM  0.40f   // Larger ratio for small rects (buttons, toggles)
#define PM_SEGS      8       // Segments for rounded rect curves

// ────────────────────────────────────────────────
// Colour palette
// ────────────────────────────────────────────────

static const Color PM_COL_PANEL    = { 18,  22,  40, 252 };
static const Color PM_COL_TITLEBAR = { 26,  30,  55, 255 };
static const Color PM_COL_BORDER   = { 55,  62, 110, 255 };
static const Color PM_COL_SECTION  = { 12,  15,  30, 200 };
static const Color PM_COL_ROW_HOVER= { 40,  48,  90, 220 };
static const Color PM_COL_ACCENT   = { 88, 140, 255, 255 };
static const Color PM_COL_ACCENT2  = { 60, 100, 220, 255 };
static const Color PM_COL_TEXT     = {215, 222, 245, 255 };
static const Color PM_COL_SUBTEXT  = {130, 145, 185, 255 };
static const Color PM_COL_SLIDER_T = { 40,  48,  85, 255 };
static const Color PM_COL_TOGGLE_F = { 55,  62, 100, 255 };
static const Color PM_COL_SEP      = { 40,  46,  85, 255 };
static const Color PM_COL_SHADOW   = {  0,   0,   0,  80 };
static const Color PM_COL_WHITE    = {255, 255, 255, 255 };
static const Color PM_COL_CLOSE_HV = {200,  70,  70, 230 };
static const Color PM_COL_GEAR_HV  = {255, 255, 255,  30 };

// ────────────────────────────────────────────────
// Internal helpers – geometry
// ────────────────────────────────────────────────

/** Returns the screen-space rectangle of the gear button. */
static inline Rectangle pm_gearRect(void) {
    return (Rectangle){
        .x      = (float)(GetScreenWidth() - PM_GEAR_SIZE - PM_GEAR_MARGIN),
        .y      = (float)PM_GEAR_MARGIN,
        .width  = (float)PM_GEAR_SIZE,
        .height = (float)PM_GEAR_SIZE
    };
}

/** Returns the screen-space rectangle of the settings panel. */
static inline Rectangle pm_menuRect(void) {
    Rectangle g = pm_gearRect();
    float x = g.x + g.width - PM_MENU_W;
    if (x < 5.0f) x = 5.0f;
    return (Rectangle){
        .x      = x,
        .y      = g.y + g.height + 6.0f,
        .width  = (float)PM_MENU_W,
        .height = (float)PM_MENU_H
    };
}

/** Checks if a point is inside a rectangle. */
static inline bool pm_hit(Vector2 p, Rectangle r) {
    return CheckCollisionPointRec(p, r);
}

// ────────────────────────────────────────────────
// Internal helpers – drawing
// ────────────────────────────────────────────────

/** Draws a filled rounded rectangle. */
static void pm_fillRound(Rectangle r, float roundness, Color col) {
    DrawRectangleRounded(r, roundness, PM_SEGS, col);
}

/** Draws a rounded rectangle outline. */
static void pm_lineRound(Rectangle r, float roundness, float thick, Color col) {
    DrawRectangleRoundedLinesEx(r, roundness, PM_SEGS, thick, col);
}

/** Draws a horizontal separator line. */
static void pm_separator(float x, float y, float w) {
    DrawRectangle((int)x, (int)y, (int)w, 1, PM_COL_SEP);
}

/**
 * Draws a section header label with a tinted background strip.
 * Returns the Y coordinate immediately after the header.
 */
static float pm_sectionHeader(Rectangle panel, float y, const char* label) {
    Rectangle bg = { panel.x, y, panel.width, (float)PM_SECTION_H };
    pm_fillRound(bg, 0.0f, PM_COL_SECTION);
    DrawText(label,
             (int)(panel.x + PM_PAD),
             (int)(y + (PM_SECTION_H - 14) / 2),
             14, PM_COL_SUBTEXT);
    return y + PM_SECTION_H;
}

/**
 * Draws a volume slider.
 *  - track: the full slider track area
 *  - value: current value in [0, 1]
 *  - dragging: true if the handle is being dragged
 */
static void pm_drawSlider(Rectangle track, float value, bool dragging) {
    // Clamp
    if (value < 0.0f) value = 0.0f;
    if (value > 1.0f) value = 1.0f;

    float cx = track.x + value * track.width;
    float cy = track.y + track.height * 0.5f;

    // Track background
    pm_fillRound(track, PM_ROUND_SM, PM_COL_SLIDER_T);

    // Filled portion
    if (value > 0.0f) {
        Rectangle fill = { track.x, track.y, cx - track.x, track.height };
        pm_fillRound(fill, PM_ROUND_SM, PM_COL_ACCENT2);
    }

    // Handle shadow
    DrawCircle((int)cx, (int)cy, (float)PM_HANDLE_R + 2, PM_COL_SHADOW);
    // Handle
    DrawCircle((int)cx, (int)cy, (float)PM_HANDLE_R,
               dragging ? PM_COL_WHITE : PM_COL_ACCENT);
    // Inner dot
    DrawCircle((int)cx, (int)cy, 3, dragging ? PM_COL_ACCENT : PM_COL_WHITE);
}

/**
 * Draws a toggle switch.
 *  - r: bounding rect (PM_TOGGLE_W × PM_TOGGLE_H)
 *  - on: current state
 *  - hovered: whether the mouse is over it
 */
static void pm_drawToggle(Rectangle r, bool on, bool hovered) {
    Color track = on ? PM_COL_ACCENT2 : PM_COL_TOGGLE_F;
    if (hovered) {
        track.r = (unsigned char)(track.r + 20 > 255 ? 255 : track.r + 20);
        track.g = (unsigned char)(track.g + 20 > 255 ? 255 : track.g + 20);
        track.b = (unsigned char)(track.b + 20 > 255 ? 255 : track.b + 20);
    }
    pm_fillRound(r, PM_ROUND_SM, track);

    float margin = 3.0f;
    float knobR  = r.height * 0.5f - margin;
    float knobX  = on
        ? r.x + r.width  - margin - knobR
        : r.x            + margin + knobR;
    float knobY  = r.y + r.height * 0.5f;

    DrawCircle((int)knobX, (int)knobY, knobR + 1.0f, PM_COL_SHADOW);
    DrawCircle((int)knobX, (int)knobY, knobR, PM_COL_WHITE);
}

/**
 * Draws a cycle arrow button (< or >).
 *  - r: hit area
 *  - label: "  <  " or "  >  "
 *  - hovered: mouse is over it
 */
static void pm_drawArrow(Rectangle r, const char* label, bool hovered) {
    Color bg = hovered
        ? (Color){ 60, 70, 130, 230 }
        : (Color){ 38, 44,  85, 220 };
    pm_fillRound(r, PM_ROUND_SM, bg);
    int tw = MeasureText(label, 18);
    DrawText(label,
             (int)(r.x + (r.width  - tw) * 0.5f),
             (int)(r.y + (r.height - 18) * 0.5f),
             18, PM_COL_TEXT);
}

// ────────────────────────────────────────────────
// paramsMenu_init
// ────────────────────────────────────────────────

void paramsMenu_init(ParamsMenu_St* menu) {
    if (!menu) return;

    menu->isOpen        = false;
    menu->selected      = RESOLUTION_800x600;
    menu->textureLoaded = false;
    menu->masterVolume  = 1.0f;
    menu->muted         = false;
    menu->draggingVolume= false;
    menu->showFps       = false;

    const char* path = "assets/images/parametres.png";
    if (!FileExists(path)) path = "../assets/images/parametres.png";

    if (FileExists(path)) {
        menu->gearTexture = LoadTexture(path);
        if (IsTextureValid(menu->gearTexture)) {
            menu->textureLoaded = true;
            SetTextureFilter(menu->gearTexture, TEXTURE_FILTER_BILINEAR);
        } else {
            log_warn("Failed to load params menu texture: %s", path);
        }
    } else {
        log_warn("Params menu texture not found: %s", path);
    }
}

// ────────────────────────────────────────────────
// paramsMenu_update
// ────────────────────────────────────────────────

void paramsMenu_update(ParamsMenu_St* menu) {
    if (!menu) return;

    Vector2    mouse   = GetMousePosition();
    bool       clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    bool       held    = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    bool       released= IsMouseButtonReleased(MOUSE_LEFT_BUTTON);

    Rectangle  gear    = pm_gearRect();
    Rectangle  panel   = pm_menuRect();

    // ── Gear button click ────────────────────────────────────────
    if (clicked && pm_hit(mouse, gear)) {
        menu->isOpen = !menu->isOpen;
        return;
    }

    if (!menu->isOpen) return;

    float px = panel.x;
    float pw = panel.width;
    float y  = panel.y;

    // ── Close button (×) in title bar ───────────────────────────
    {
        Rectangle closeBtn = {
            px + pw - PM_PAD - 22,
            y  + (PM_TITLE_H - 22) * 0.5f,
            22, 22
        };
        if (clicked && pm_hit(mouse, closeBtn)) {
            menu->isOpen = false;
            return;
        }
    }
    y += PM_TITLE_H;

    // ── Resolution section ───────────────────────────────────────
    y += PM_SECTION_H;   // skip section header
    {
        float rowY = y + (PM_ROW_H - PM_TOGGLE_H) * 0.5f;

        // Left arrow (<)
        Rectangle leftArrow = { px + PM_PAD, rowY, (float)PM_ARROW_W, (float)PM_TOGGLE_H };
        if (clicked && pm_hit(mouse, leftArrow)) {
            menu->selected = (menu->selected == 0)
                ? (Resolution_Et)(RESOLUTION_COUNT - 1)
                : (Resolution_Et)(menu->selected - 1);
            systemSettings.video.width = paramsMenu_getWidth(menu->selected);
            systemSettings.video.height = paramsMenu_getHeight(menu->selected);
            applySystemSettings();
            log_info("Resolution: %s (%dx%d)", paramsMenu_getLabel(menu->selected), systemSettings.video.width, systemSettings.video.height);
        }

        // Right arrow (>)
        Rectangle rightArrow = {
            px + pw - PM_PAD - PM_ARROW_W,
            rowY, (float)PM_ARROW_W, (float)PM_TOGGLE_H
        };
        if (clicked && pm_hit(mouse, rightArrow)) {
            menu->selected = (Resolution_Et)((menu->selected + 1) % RESOLUTION_COUNT);
            systemSettings.video.width = paramsMenu_getWidth(menu->selected);
            systemSettings.video.height = paramsMenu_getHeight(menu->selected);
            applySystemSettings();
            log_info("Resolution: %s (%dx%d)", paramsMenu_getLabel(menu->selected), systemSettings.video.width, systemSettings.video.height);
        }
    }
    y += PM_ROW_H;
    y += PM_SEP * 2 + 1; // separator

    // ── Audio section ────────────────────────────────────────────
    y += PM_SECTION_H;   // skip header

    // Volume slider row
    {
        float sliderY  = y + (PM_ROW_H * 0.5f);
        float trackX   = px + PM_PAD + 56;
        float trackW   = pw - PM_PAD * 2 - 56 - 36; // leave room for label+pct
        Rectangle track = {
            trackX,
            sliderY - PM_SLIDER_H * 0.5f,
            trackW,
            (float)PM_SLIDER_H
        };

        // Start drag
        if (clicked && pm_hit(mouse, (Rectangle){
                track.x - PM_HANDLE_R, track.y - PM_HANDLE_R,
                track.width + PM_HANDLE_R * 2, track.height + PM_HANDLE_R * 2})) {
            menu->draggingVolume = true;
        }

        // While dragging
        if (menu->draggingVolume && held) {
            float v = (mouse.x - track.x) / track.width;
            if (v < 0.0f) v = 0.0f;
            if (v > 1.0f) v = 1.0f;
            menu->masterVolume = v;
            systemSettings.audio.masterVolume = v;
            SetMasterVolume(menu->muted ? 0.0f : v);
        }

        // End drag
        if (released) menu->draggingVolume = false;
    }
    y += PM_ROW_H;

    // Mute toggle row
    {
        Rectangle toggleRect = {
            px + pw - PM_PAD - PM_TOGGLE_W,
            y + (PM_ROW_H - PM_TOGGLE_H) * 0.5f,
            (float)PM_TOGGLE_W, (float)PM_TOGGLE_H
        };
        if (clicked && pm_hit(mouse, toggleRect)) {
            menu->muted = !menu->muted;
            systemSettings.audio.mute = menu->muted;
            SetMasterVolume(menu->muted ? 0.0f : menu->masterVolume);
            log_info("Mute: %s", menu->muted ? "on" : "off");
        }
    }
    y += PM_ROW_H;
    y += PM_SEP * 2 + 1; // separator

    // ── Display section ──────────────────────────────────────────
    y += PM_SECTION_H;   // skip header

    // FPS toggle row
    {
        Rectangle toggleRect = {
            px + pw - PM_PAD - PM_TOGGLE_W,
            y + (PM_ROW_H - PM_TOGGLE_H) * 0.5f,
            (float)PM_TOGGLE_W, (float)PM_TOGGLE_H
        };
        if (clicked && pm_hit(mouse, toggleRect)) {
            menu->showFps = !menu->showFps;
            log_info("Show FPS: %s", menu->showFps ? "on" : "off");
        }
    }

    // ── Close when clicking outside ──────────────────────────────
    if (clicked) {
        if (!pm_hit(mouse, gear) && !pm_hit(mouse, panel)) {
            menu->isOpen = false;
        }
    }
}

// ────────────────────────────────────────────────
// paramsMenu_draw
// ────────────────────────────────────────────────

void paramsMenu_draw(ParamsMenu_St* menu) {
    if (!menu) return;

    Vector2   mouse = GetMousePosition();
    Rectangle gear  = pm_gearRect();

    // ── FPS counter (drawn before UI so it's always readable) ────
    if (menu->showFps) {
        DrawFPS(10, 10);
    }

    // ── Gear button ──────────────────────────────────────────────
    bool gearHovered = pm_hit(mouse, gear);

    // Hover glow circle
    if (gearHovered || menu->isOpen) {
        Color glowCol = menu->isOpen
            ? (Color){ 88, 140, 255, 50 }
            : PM_COL_GEAR_HV;
        DrawCircle(
            (int)(gear.x + gear.width  * 0.5f),
            (int)(gear.y + gear.height * 0.5f),
            (float)PM_GEAR_SIZE * 0.72f,
            glowCol);
    }

    // Gear icon (properly scaled via DrawTexturePro)
    if (menu->textureLoaded) {
        Rectangle src = {
            0, 0,
            (float)menu->gearTexture.width,
            (float)menu->gearTexture.height
        };
        DrawTexturePro(menu->gearTexture, src, gear, (Vector2){ 0, 0 }, 0.0f, WHITE);
    } else {
        // Fallback: filled square with ⚙ text
        pm_fillRound(gear, PM_ROUND_SM,
                     menu->isOpen ? PM_COL_ACCENT2 : PM_COL_TOGGLE_F);
        int tw = MeasureText("@", 18);
        DrawText("@",
                 (int)(gear.x + (gear.width  - tw) * 0.5f),
                 (int)(gear.y + (gear.height - 18) * 0.5f),
                 18, PM_COL_WHITE);
    }

    if (!menu->isOpen) return;

    // ── Panel ────────────────────────────────────────────────────
    Rectangle panel = pm_menuRect();
    float px = panel.x;
    float py = panel.y;
    float pw = panel.width;

    // Drop shadow
    Rectangle shadow = { px + 4, py + 4, panel.width, panel.height };
    pm_fillRound(shadow, PM_ROUND, PM_COL_SHADOW);

    // Main background
    pm_fillRound(panel, PM_ROUND, PM_COL_PANEL);
    pm_lineRound(panel, PM_ROUND, 1.5f, PM_COL_BORDER);

    float y = py;

    // ── Title bar ────────────────────────────────────────────────
    {
        Rectangle titleBar = { px, y, pw, (float)PM_TITLE_H };
        pm_fillRound(titleBar, PM_ROUND, PM_COL_TITLEBAR);
        // Only round the top corners – cover bottom with a solid strip
        DrawRectangle(
            (int)px,
            (int)(y + PM_TITLE_H * 0.5f),
            (int)pw,
            (int)(PM_TITLE_H * 0.5f),
            PM_COL_TITLEBAR);

        // Title text
        DrawText("Settings",
                 (int)(px + PM_PAD),
                 (int)(y + (PM_TITLE_H - 20) * 0.5f),
                 20, PM_COL_TEXT);

        // Close button ×
        Rectangle closeBtn = {
            px + pw - PM_PAD - 22,
            y  + (PM_TITLE_H - 22) * 0.5f,
            22, 22
        };
        bool closeHov = pm_hit(mouse, closeBtn);
        pm_fillRound(closeBtn, PM_ROUND_SM,
                     closeHov ? PM_COL_CLOSE_HV : (Color){60, 62, 100, 200});
        int xtw = MeasureText("x", 14);
        DrawText("x",
                 (int)(closeBtn.x + (closeBtn.width  - xtw) * 0.5f),
                 (int)(closeBtn.y + (closeBtn.height - 14)  * 0.5f),
                 14, PM_COL_TEXT);
    }
    y += PM_TITLE_H;

    // ── Resolution section ───────────────────────────────────────
    y = pm_sectionHeader(panel, y, "  RESOLUTION");
    {
        // Row hover
        Rectangle rowRect = { px, y, pw, (float)PM_ROW_H };
        bool rowHov = pm_hit(mouse, rowRect);
        if (rowHov)
            DrawRectangleRec(rowRect, PM_COL_ROW_HOVER);

        float rowY   = y + (PM_ROW_H - PM_TOGGLE_H) * 0.5f;
        float midX   = px + pw * 0.5f;

        // Left arrow
        Rectangle leftArrow  = { px + PM_PAD, rowY, (float)PM_ARROW_W, (float)PM_TOGGLE_H };
        // Right arrow
        Rectangle rightArrow = {
            px + pw - PM_PAD - PM_ARROW_W,
            rowY, (float)PM_ARROW_W, (float)PM_TOGGLE_H
        };

        bool lHov = pm_hit(mouse, leftArrow);
        bool rHov = pm_hit(mouse, rightArrow);

        pm_drawArrow(leftArrow,  "<", lHov);
        pm_drawArrow(rightArrow, ">", rHov);

        // Resolution label (centred)
        const char* resLabel = paramsMenu_getLabel(menu->selected);
        int tw = MeasureText(resLabel, 15);
        DrawText(resLabel,
                 (int)(midX - tw * 0.5f),
                 (int)(y + (PM_ROW_H - 15) * 0.5f),
                 15, PM_COL_TEXT);
    }
    y += PM_ROW_H;

    // Separator
    y += PM_SEP;
    pm_separator(px + PM_PAD, y, pw - PM_PAD * 2);
    y += 1 + PM_SEP;

    // ── Audio section ────────────────────────────────────────────
    y = pm_sectionHeader(panel, y, "  AUDIO");

    // Volume slider row
    {
        Rectangle rowRect = { px, y, pw, (float)PM_ROW_H };
        bool rowHov = pm_hit(mouse, rowRect);
        if (rowHov && !menu->draggingVolume)
            DrawRectangleRec(rowRect, PM_COL_ROW_HOVER);

        float sliderCY = y + PM_ROW_H * 0.5f;
        float trackX   = px + PM_PAD + 60;
        float trackW   = pw - PM_PAD * 2 - 60 - 40;
        Rectangle track = {
            trackX,
            sliderCY - PM_SLIDER_H * 0.5f,
            trackW,
            (float)PM_SLIDER_H
        };

        DrawText("Volume",
                 (int)(px + PM_PAD),
                 (int)(sliderCY - 7),
                 14, PM_COL_SUBTEXT);

        pm_drawSlider(track, menu->masterVolume, menu->draggingVolume);

        // Percentage label
        char pct[16];
        int pctVal = (int)(menu->masterVolume * 100.0f + 0.5f);
        if (pctVal > 100) pctVal = 100;
        snprintf(pct, sizeof(pct), "%d%%", pctVal);
        int ptw = MeasureText(pct, 13);
        DrawText(pct,
                 (int)(px + pw - PM_PAD - ptw),
                 (int)(sliderCY - 6),
                 13, PM_COL_ACCENT);
    }
    y += PM_ROW_H;

    // Mute toggle row
    {
        Rectangle rowRect = { px, y, pw, (float)PM_ROW_H };
        Rectangle toggleRect = {
            px + pw - PM_PAD - PM_TOGGLE_W,
            y + (PM_ROW_H - PM_TOGGLE_H) * 0.5f,
            (float)PM_TOGGLE_W, (float)PM_TOGGLE_H
        };
        bool togHov = pm_hit(mouse, toggleRect);
        if (pm_hit(mouse, rowRect))
            DrawRectangleRec(rowRect, PM_COL_ROW_HOVER);

        DrawText("Mute",
                 (int)(px + PM_PAD),
                 (int)(y + (PM_ROW_H - 14) * 0.5f),
                 14, PM_COL_SUBTEXT);

        pm_drawToggle(toggleRect, menu->muted, togHov);

        // Status label
        const char* muteLabel = menu->muted ? "On" : "Off";
        int mlw = MeasureText(muteLabel, 12);
        DrawText(muteLabel,
                 (int)(toggleRect.x - mlw - 8),
                 (int)(y + (PM_ROW_H - 12) * 0.5f),
                 12, menu->muted ? PM_COL_ACCENT : PM_COL_SUBTEXT);
    }
    y += PM_ROW_H;

    // Separator
    y += PM_SEP;
    pm_separator(px + PM_PAD, y, pw - PM_PAD * 2);
    y += 1 + PM_SEP;

    // ── Display section ──────────────────────────────────────────
    y = pm_sectionHeader(panel, y, "  DISPLAY");

    // FPS toggle row
    {
        Rectangle rowRect = { px, y, pw, (float)PM_ROW_H };
        Rectangle toggleRect = {
            px + pw - PM_PAD - PM_TOGGLE_W,
            y + (PM_ROW_H - PM_TOGGLE_H) * 0.5f,
            (float)PM_TOGGLE_W, (float)PM_TOGGLE_H
        };
        bool togHov = pm_hit(mouse, toggleRect);
        if (pm_hit(mouse, rowRect))
            DrawRectangleRec(rowRect, PM_COL_ROW_HOVER);

        DrawText("Show FPS",
                 (int)(px + PM_PAD),
                 (int)(y + (PM_ROW_H - 14) * 0.5f),
                 14, PM_COL_SUBTEXT);

        pm_drawToggle(toggleRect, menu->showFps, togHov);

        // Status label
        const char* fpsLabel = menu->showFps ? "On" : "Off";
        int flw = MeasureText(fpsLabel, 12);
        DrawText(fpsLabel,
                 (int)(toggleRect.x - flw - 8),
                 (int)(y + (PM_ROW_H - 12) * 0.5f),
                 12, menu->showFps ? PM_COL_ACCENT : PM_COL_SUBTEXT);
    }
}

// ────────────────────────────────────────────────
// paramsMenu_free
// ────────────────────────────────────────────────

void paramsMenu_free(ParamsMenu_St* menu) {
    if (!menu) return;
    if (menu->textureLoaded) {
        UnloadTexture(menu->gearTexture);
        menu->textureLoaded = false;
    }
}
