/**
    @file globals.c
    @author Léandre BAUDET
    @date 2026-01-12
    @date 2026-04-14
    @brief Definitions of program-wide global variables.
*/
#include "utils/globals.h"

// ────────────────────────────────────────────────
// General Globals
// ────────────────────────────────────────────────

LobbyGame_St lobby_game = {0};

Font lobby_fonts[__fontSizeCount] = {0};

f32 gameTime = 0.0f;

PlayerProgress_St g_progress = {0};
bool g_shouldExit = false;

// ────────────────────────────────────────────────
// Platforms / Terrains
// ────────────────────────────────────────────────

TerrainVec_St terrains = {0};

Texture2D terrainTextures[__terrainKindCount] = {0};

GameInteractionZone_St gameZones[__miniGameIdCount] = {
    [MINI_GAME_ID_BINGO] = {
        .hitbox = {
            .x      = -1475.00,
            .y      = 425.00,
            .width  = 75,
            .height = 75
        },
        .name = "Bingo"
    },

    [MINI_GAME_ID_CHESS] = {
        .hitbox = {
            .x      = 682.50,
            .y      = -775.00,   // platform P13 top=-700; zone.y = -700 - 75
            .width  = 75,
            .height = 75
        },
        .name = "Chess"
    },

    [MINI_GAME_ID_DROP_FOUR]  = {
        .hitbox = {
            .x      = -587.50,
            .y      = -755.00,   // platform P10 top=-680; zone.y = -680 - 75
            .width  = 75,
            .height = 75
        },
        .name = "Drop Four"
    },

    [MINI_GAME_ID_TETROMINO_FALL] = {
        .hitbox = {
            .x      = -87.50,
            .y      = -275.00,   // platform P6 top=-200; zone.y = -200 - 75
            .width  = 75,
            .height = 75
        },
        .name = "Tetromino Fall",
        .color = {0, 120, 220, 200},
    },

    [MINI_GAME_ID_EDITOR] = {
        .hitbox = {
            .x      = -157.50,
            .y      = -715.00,   // platform P12 top=-640; zone.y = -640 - 75
            .width  = 75,
            .height = 75
        },
        .name = "Level Editor",
        .color = {0, 160, 80,  200},
    },

    [MINI_GAME_ID_SUIKA] = {
        .hitbox = {
            .x      = 402.50,
            .y      = -1175.00,  // platform P17 top=-1100; zone.y = -1100 - 75
            .width  = 75,
            .height = 75
        },
        .name = "Suika",
        .color = {220, 80, 0,  200},
    },

    [MINI_GAME_ID_BOWLING] = {
        .hitbox = {
            .x      = -427.50,
            .y      = -925.00,   // platform P14 top=-850; zone.y = -850 - 75
            .width  = 75,
            .height = 75
        },
        .name = "Bowling",
        .color = {140, 0, 200, 200},
    },

    [MINI_GAME_ID_MINI_GOLF] = {
        .hitbox = {
            .x      = -537.50,
            .y      = -225.00,   // platform P5 top=-150; zone.y = -150 - 75
            .width  = 75,
            .height = 75
        },
        .name = "3D Mini-Golf",
        .color = { 20, 160,  50, 200},
    },

    [MINI_GAME_ID_SNAKE] = {
        .hitbox = {
            .x      = -37.50,
            .y      = -535.00,   // platform P9 top=-460; zone.y = -460 - 75
            .width  = 75,
            .height = 75
        },
        .name = "Snake",
        .color = {  0, 200,  80, 200},
    },

    [MINI_GAME_ID_POLY_BLAST] = {
        .hitbox = {
            .x      = 312.50,
            .y      = -625.00,   // platform P11 top=-550; zone.y = -550 - 75
            .width  = 75,
            .height = 75
        },
        .name = "Poly Blast!",
        .color = { 60,  60, 200, 200},
    },

    [MINI_GAME_ID_DISC_REVERSAL]    = {
        .hitbox = {
            .x      = -37.50,
            .y      = -1055.00,  // platform P16 top=-980; zone.y = -980 - 75
            .width  = 75,
            .height = 75
        },
        .name = "Disc Reversal"
    },

    [MINI_GAME_ID_KING_FOR_FOUR] = {
        .hitbox = {
            .x      =  62.50,
            .y      = -75.00,    // platform P4 top=0; zone.y = 0 - 75
            .width  = 75,
            .height = 75
        },
        .name = "King For Four"
    },

    [MINI_GAME_ID_TWIST_CUBE] = {
        .hitbox = {
            .x      = -857.50,
            .y      = -515.00,   // platform P8 top=-440; zone.y = -440 - 75
            .width  = 75,
            .height = 75
        },
        .name = "Twist Cube"
    },

    [MINI_GAME_ID_SOLO_CARDS] = {
        .hitbox = {
            .x      =  242.50,
            .y      = -855.00,   // platform P15 top=-780; zone.y = -780 - 75
            .width  = 75,
            .height = 75
        },
        .name = "Solo Cards"
    },
};

// ────────────────────────────────────────────────
// Skin stuff
// ────────────────────────────────────────────────

Rectangle skinButtonRect = { WINDOW_WIDTH - 70, WINDOW_HEIGHT / 2.0f - 25, 50, 50 };

Texture2D logoSkinButton = {0};

// ────────────────────────────────────────────────
// Parameters Menu
// ────────────────────────────────────────────────

ParamsMenu_St paramsMenu;

// ────────────────────────────────────────────────
// Textures / Ambiant
// ────────────────────────────────────────────────

Texture2D treeTexture;
Texture2D backgroundTexture;
Texture2D leafTexture;

GrassBlade_St grassBlades[MAX_GRASS_BLADES];
int grassCount = 0;

const Vector2 moonLightDir = {-0.6f, -0.8f};

// ────────────────────────────────────────────────
// Sounds
// ────────────────────────────────────────────────

Sound sound_jump;
Sound sound_doubleJump;
Sound sound_gameLaunch;

Sound sound_doubleJumpMeme;