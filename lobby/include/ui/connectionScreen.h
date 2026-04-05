/**
    @file ui/connectionScreen.h
    @author Fshimi-Hawlk
    @author i-Charlys (CAILLON Charles)
    @date 2026-03-18
    @date 2026-04-04
    @brief Declaration of the two-layer connection screen (Server List -> Room List).
*/

#ifndef UI_CONNECTION_SCREEN_H
#define UI_CONNECTION_SCREEN_H

#include "baseTypes.h"
#include "APIs/generalAPI.h"   // for MiniGame_Et

typedef enum {
    CONNECTION_LAYER_SERVER_LIST,
    CONNECTION_LAYER_ROOM_LIST
} ConnectionLayer_Et;

/**
    @brief Allocates and positions all UI elements.
*/
void initConnectionScreen(void);

/**
    @brief Logical update. Returns true when user clicks Connect (Layer 1) or Join (Layer 2).
*/
bool updateConnectionScreen(void);

/**
    @brief Renders the current layer.
*/
void drawConnectionScreen(void);

/**
    @brief Adds a discovered server to Layer 1 list (called from receiveNetworkData).
*/
void addDiscoveredServer(const char* ip, const char* name);

/**
    @brief Adds a game instance to Layer 2 list (called when receiving INSTANCE_INFO).
*/
void addGameInstance(u32 instanceId, MiniGame_Et gameType, const char* hostName,
                     u8 playerCount, u8 maxPlayers);

/**
    @brief Returns the IP entered by the user.
*/
const char* getEnteredIP(void);

/**
    @brief Returns the player name entered in Layer 1.
*/
const char* getPlayerName(void);

/**
    @brief Switches the screen to Layer 2 (Room List) after successful server connection.
*/
void switchToRoomListLayer(void);

/**
    @brief Returns whether we are currently showing the Room List (Layer 2).
*/
bool isInRoomListLayer(void);

#endif // UI_CONNECTION_SCREEN_H