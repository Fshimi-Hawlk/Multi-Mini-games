/**
    @file suika_atlas.h
    @author Maxime CHAUVEAU
    @date 2026-04-07
    @date 2026-04-14
    @brief Sprite atlas definitions for Suika fruit textures.
*/
#ifndef SUIKA_ATLAS_H
#define SUIKA_ATLAS_H

#include <stddef.h>   /* size_t */

/* ------------------------------------------------------------------ */
/*  Types                                                               */
/* ------------------------------------------------------------------ */

/**
    @brief Information about a single fruit sprite in the atlas.
*/
typedef struct {
    const char *name;   ///< Name of the fruit
    int         x;      ///< X coordinate of the top-left corner in the atlas (pixels)
    int         y;      ///< Y coordinate of the top-left corner in the atlas (pixels)
    int         w;      ///< Width of the sprite in the atlas (pixels)
    int         h;      ///< Height of the sprite in the atlas (pixels)
    int         level;  ///< Fruit level (1 = smallest -> 11 = largest)
} SuikaSprite;

/* ------------------------------------------------------------------ */
/*  Dimensions de la texture source                                    */
/* ------------------------------------------------------------------ */

#define SUIKA_ATLAS_WIDTH   864
#define SUIKA_ATLAS_HEIGHT  486
#define SUIKA_ATLAS_FILE    "assets_suika.png"

/* ------------------------------------------------------------------ */
/*  Atlas – 11 fruits, du plus petit au plus grand                     */
/* ------------------------------------------------------------------ */

#define SUIKA_FRUIT_COUNT 11

/**
    @brief Static atlas containing definitions for all fruit sprites.
*/
static const SuikaSprite SUIKA_ATLAS[SUIKA_FRUIT_COUNT] = {
    /*  name             x    y    w    h   level  */
    { "cherry",         44, 130,  45,  70,   1 },
    { "grape",         116, 136,  44,  64,   2 },
    { "strawberry",    216, 143,  40,  57,   3 },
    { "small_orange",  304, 109,  96, 113,   4 },
    { "big_orange",    451, 114, 113, 117,   5 },
    { "apple",         579,  88, 114, 134,   6 },
    { "pear",           58, 277, 102, 146,   7 },
    { "peach",         210, 305, 123, 113,   8 },
    { "pineapple",     369, 248, 106, 194,   9 },
    { "melon",         510, 288, 122, 144,  10 },
    { "watermelon",    672, 272, 168, 200,  11 },
};

/* ------------------------------------------------------------------ */
/*  Utilitaires inline                                                  */
/* ------------------------------------------------------------------ */

/**
    @brief Finds a sprite by its name identifier.

    @param[in]  name    The name of the fruit to find.
    @return             Pointer to the SuikaSprite, or NULL if not found.
*/
static inline const SuikaSprite *
suika_get_sprite(const char *name)
{
    for (size_t i = 0; i < SUIKA_FRUIT_COUNT; i++) {
        /* strcmp manuel pour éviter d'importer <string.h> si non voulu */
        const char *a = SUIKA_ATLAS[i].name;
        const char *b = name;
        while (*a && *a == *b) { a++; b++; }
        if (*a == '\0' && *b == '\0')
            return &SUIKA_ATLAS[i];
    }
    return NULL;
}

/**
    @brief Finds a sprite by its fruit level (1-11).

    @param[in]  level   The level of the fruit (1 = smallest).
    @return             Pointer to the SuikaSprite, or NULL if out of range.
*/
static inline const SuikaSprite *
suika_get_by_level(int level)
{
    if (level < 1 || level > SUIKA_FRUIT_COUNT)
        return NULL;
    return &SUIKA_ATLAS[level - 1];
}

#endif /* SUIKA_ATLAS_H */
