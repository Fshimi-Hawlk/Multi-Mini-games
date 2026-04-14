/**
    @file ui/connectionScreen.h
    @author i-Charlys (CAILLON Charles)
    @author Fshimi-Hawlk
    @date 2026-04-13
    @date 2026-04-13
    @brief Two-layer connection screen (Server List → Room List) using shared widgets.
*/
#ifndef UI_CONNECTION_SCREEN_H
#define UI_CONNECTION_SCREEN_H

#include "utils/userTypes.h"

/**
    @brief Initializes connection screen widgets.
*/
void lobby_initConnectionScreen(void);

/**
    @brief Updates connection screen logic.
    @return true if connect button was pressed and IP is valid.
*/
bool lobby_updateConnectionScreen(void);

/**
    @brief Draws the connection screen UI.
*/
void lobby_drawConnectionScreen(void);

/**
    @brief Adds a discovered server to the list.
*/
void lobby_addDiscoveredServer(const char* ip, const char* name);

/**
    @brief Sets an error message to be displayed.
*/
void lobby_setConnectionError(const char* error);

/**
    @brief Gets the IP entered by the user.
*/
const char* lobby_getEnteredIP(void);

/**
    @brief Gets the player name entered by the user.
*/
const char* lobby_getEnteredPseudo(void);

#endif // UI_CONNECTION_SCREEN_H