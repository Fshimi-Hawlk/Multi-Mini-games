/**
 * @file event.h
 * @author Maxime CHAUVEAU
 * @brief Event handling for Echecs.
 * @version 1.0
 * @date 2024
 *
 * This file contains functions for handling user input events
 * such as mouse clicks and window events.
 */

#ifndef EVENT_H
#define EVENT_H

#include "types.h"

/**
 * @brief Handle all input events for the current frame.
 * @param board The game board
 */
void handleEvents(Board_t board);

#endif