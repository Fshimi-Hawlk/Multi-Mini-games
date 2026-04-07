/**
 * @file room_selector.h
 * @brief Room Selection UI for mini-games.
 */

#ifndef ROOM_SELECTOR_H
#define ROOM_SELECTOR_H

#include "baseTypes.h"
#include "raylib.h"

#pragma pack(push, 1)
typedef struct {
    u16 id;
    u16 playerCount;
    char name[32];
    char creator[32];
} RoomInfo_St;
#pragma pack(pop)

/**
 * @brief Initializes the room selector.
 */
void initRoomSelector(void);

/**
 * @brief Opens the room selector.
 * @param gameId The mini-game to list rooms for, or -1 for ALL rooms (Lobby/Layer 2 mode).
 */
void openRoomSelector(int gameId);

/**
 * @brief Closes the room selector.
 */
void closeRoomSelector(void);

/**
 * @brief Updates the room selector logic.
 * @return true if an action was taken (join/create).
 */
bool updateRoomSelector(void);

/**
 * @brief Draws the room selector.
 */
void drawRoomSelector(void);

/**
 * @brief Handles incoming room information from the server.
 * @param data Pointer to the RoomInfo_St array.
 * @param count Number of rooms in the array.
 */
void handleRoomList(const void* data, int count);

#endif
