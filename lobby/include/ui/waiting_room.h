/**
 * @file waiting_room.h
 * @brief Overlay UI for waiting rooms.
 */

#ifndef WAITING_ROOM_H
#define WAITING_ROOM_H

#include "baseTypes.h"
#include "raylib.h"

/**
 * @brief Initializes the waiting room UI.
 */
void initWaitingRoom(void);

/**
 * @brief Shows the waiting room for a specific room.
 */
void showWaitingRoom(int gameId, int roomId, bool isHost);

/**
 * @brief Updates the waiting room information from game sync.
 */
void updateWaitingRoomInfo(int players, int max, bool host);

/**
 * @brief Sets the visibility of the waiting room panel.
 */
void setWaitingRoomPanelVisible(bool visible);

/**
 * @brief Updates the waiting room logic.
 */
void updateWaitingRoom(void);

/**
 * @brief Draws the waiting room UI (panel or status text).
 */
void drawWaitingRoom(void);

#endif
