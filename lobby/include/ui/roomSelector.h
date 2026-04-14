/**
    @file ui/roomSelector.h
    @author i-Charlys (CAILLON Charles)
    @author Fshimi-Hawlk
    @date 2026-04-13
    @date 2026-04-13
    @brief Room Selection UI for mini-games (Layer 2 of connection screen).

    Contributors:
        - i-Charlys:
            - Original room list logic
        - Fshimi-Hawlk:
            - Full widget migration, lobby_ prefix, CSC compliance
*/

#ifndef UI_ROOM_SELECTOR_H
#define UI_ROOM_SELECTOR_H

#include "utils/userTypes.h"

/**
    @brief Initializes the room selector.
*/
void lobby_initRoomSelector(void);

/**
    @brief Opens the room selector.
    @param gameId The mini-game to list rooms for, or -1 for ALL rooms (Lobby/Layer 2 mode).
*/
void lobby_openRoomSelector(s32 gameId);

/**
    @brief Closes the room selector.
*/
void lobby_closeRoomSelector(void);

/**
    @brief Updates the room selector logic.
    @return true if an action was taken (join/create).
*/
bool lobby_updateRoomSelector(void);

/**
    @brief Draws the room selector.
*/
void lobby_drawRoomSelector(void);

/**
    @brief Handles incoming room information from the server.
    @param data Pointer to the RoomInfo_St array.
    @param count Number of rooms in the array.
*/
void lobby_handleRoomList(const void* data, s32 count);

#endif // UI_ROOM_SELECTOR_H