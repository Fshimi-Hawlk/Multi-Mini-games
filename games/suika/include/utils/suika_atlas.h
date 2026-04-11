/*
 * suika_atlas.h
 * Atlas de sprites pour le jeu Suika
 * Image source : assets_suika.png (864 x 486 px)
 *
 * Chaque entrée contient :
 *   name   – identifiant du fruit
 *   x, y   – coin supérieur gauche dans la texture (pixels)
 *   w, h   – largeur / hauteur du sprite (pixels)
 *   level  – niveau du fruit (1 = plus petit → 11 = plus grand)
 */

#ifndef SUIKA_ATLAS_H
#define SUIKA_ATLAS_H

#include <stddef.h>   /* size_t */

/* ------------------------------------------------------------------ */
/*  Types                                                               */
/* ------------------------------------------------------------------ */

typedef struct {
    const char *name;   /* nom du fruit            */
    int         x;      /* origine X  (px)         */
    int         y;      /* origine Y  (px)         */
    int         w;      /* largeur    (px)         */
    int         h;      /* hauteur    (px)         */
    int         level;  /* niveau dans le jeu      */
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
 * suika_get_sprite – retrouve un sprite par nom.
 * Retourne NULL si non trouvé.
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
 * suika_get_by_level – retrouve un sprite par niveau (1–11).
 * Retourne NULL si hors plage.
 */
static inline const SuikaSprite *
suika_get_by_level(int level)
{
    if (level < 1 || level > SUIKA_FRUIT_COUNT)
        return NULL;
    return &SUIKA_ATLAS[level - 1];
}

#endif /* SUIKA_ATLAS_H */
