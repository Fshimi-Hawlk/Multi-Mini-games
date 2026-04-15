/**
    @file globals.c
    @author Maxime CHAUVEAU
    @date 2026-04-07
    @date 2026-04-14
    @brief Global variable definitions including tetramino shapes, colors, and input repeat settings.
*/
#include "utils/globals.h"
#include "core/shape.h"

InputRepeat_St inputRepeat = {0, 0, 0, 0.2f, 0.05f};

/* Using numeric values instead of macros to be C99 pedantic-compliant */
Color tetraminosColors[] = {
    {0, 255, 255, 255},     /* CYAN */
    {253, 249, 0, 255},     /* YELLOW */
    {255, 0, 255, 255},     /* MAGENTA */
    {0, 228, 48, 255},      /* GREEN */
    {230, 41, 55, 255},     /* RED */
    {0, 121, 241, 255},     /* BLUE */
    {255, 161, 0, 255}      /* ORANGE */
};

Tetromino_t tetraminosShapes[] = {I_SHAPE, O_SHAPE, T_SHAPE, S_SHAPE, Z_SHAPE, J_SHAPE, L_SHAPE};