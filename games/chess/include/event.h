/**
    @file event.h
    @author Léandre BAUDET
    @date 2024-01-01
    @date 2026-04-14
    @brief Event handling for chess.
*/
#ifndef EVENT_H
#define EVENT_H

#include "types.h"

/**
    @brief Handle all input events for the current frame.
    @param[in,out] board  The game board
*/
void handleEvents(Board_t board);

#endif
