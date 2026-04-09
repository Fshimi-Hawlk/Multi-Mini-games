/**
    @file ui/connection_screen.h
    @author Fshimi-Hawlk
    @author i-Charlys
    @date 2026-03-30
    @brief Public interface for the lobby connection screen.
*/

#ifndef UI_CONNECTION_SCREEN_H
#define UI_CONNECTION_SCREEN_H

#include "utils/userTypes.h"

/**
 * @brief Initializes connection screen widgets.
 */
void initConnectionScreen(void);

/**
 * @brief Updates connection screen logic.
 * @return true if connect button was pressed and IP is valid.
 */
bool updateConnectionScreen(void);

/**
 * @brief Draws the connection screen UI.
 */
void drawConnectionScreen(void);

/**
 * @brief Gets the IP entered by the user.
 */
const char* getEnteredIP(void);

/**
 * @brief Gets the pseudo entered by the user.
 */
const char* getEnteredPseudo(void);

/**
 * @brief Adds a discovered room to the list.
 */
void addDiscoveredRoom(const char* ip, const char* name);

/**
 * @brief Sets an error message to be displayed on the connection screen.
 */
void setConnectionError(const char* error);

#endif // UI_CONNECTION_SCREEN_H
