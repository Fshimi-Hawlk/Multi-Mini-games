/**
    @file ui/waitingRoom.h
    @author i-Charlys (CAILLON Charles)
    @author Fshimi-Hawlk
    @date 2026-04-13
    @date 2026-04-13
    @brief Overlay UI for waiting rooms (modal panel + status text).

    Contributors:
        - i-Charlys: Original waiting room logic
        - Fshimi-Hawlk: Full widget migration, lobby_ prefix, CSC compliance
*/

#ifndef UI_WAITING_ROOM_H
#define UI_WAITING_ROOM_H

#include "utils/userTypes.h"

/**
    @brief Initializes the waiting room UI.
*/
void lobby_initWaitingRoom(void);

/**
    @brief Shows the waiting room for a specific room.
*/
void lobby_showWaitingRoom(s32 gameId, s32 roomId, bool isHost);

/**
    @brief Updates the waiting room information from game sync.
*/
void lobby_updateWaitingRoomInfo(s32 players, s32 max, bool host);

/**
    @brief Sets the visibility of the waiting room panel.
*/
void lobby_setWaitingRoomPanelVisible(bool visible);

/**
    @brief Updates the waiting room logic.
*/
void lobby_updateWaitingRoom(void);

/**
    @brief Draws the waiting room UI (panel or status text).
*/
void lobby_drawWaitingRoom(void);

#endif // UI_WAITING_ROOM_H