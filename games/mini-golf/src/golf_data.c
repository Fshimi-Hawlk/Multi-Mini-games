/**
    @file golf_data.c
    @author Maxime CHAUVEAU
    @date 2026-04-07
    @date 2026-04-14
    @brief Official golf club data specifications.
*/
#include "golf.h"

/**
    @brief Official data for each golf club (R&A / USGA compliant).
*/
const ClubSpec CLUBS[CLUB_COUNT] = {
    /* name      max_power  loft_deg  accuracy  max_dist_m */
    { "Driver",  95.0f,     10.5f,    0.55f,    300.0f },
    { "Bois 3",  85.0f,     15.0f,    0.62f,    255.0f },
    { "Fer 5",   70.0f,     27.0f,    0.72f,    195.0f },
    { "Fer 9",   55.0f,     41.0f,    0.80f,    135.0f },
    { "Wedge",   45.0f,     52.0f,    0.85f,     95.0f },
    { "Putter",  18.0f,      4.0f,    0.95f,     25.0f },
};
