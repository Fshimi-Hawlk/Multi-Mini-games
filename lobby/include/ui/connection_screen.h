/**
<<<<<<< HEAD
 * @file connection_screen.h
 * @author i-Charlys
 * @date 2026-03-18
 * @brief Declaration of the connection screen scene for IP entry.
 */
=======
    @file ui/connection_screen.h
    @author i-Charlys (CAILLON Charles)
    @author Fshimi-Hawlk
    @date 2026-03-18
    @date 2026-03-30
    @brief Declaration of the modern connection screen for server discovery and IP entry.
*/
>>>>>>> origin/mgit-PR1-20-03

#ifndef UI_CONNECTION_SCREEN_H
#define UI_CONNECTION_SCREEN_H

#include <stdbool.h>
#include "rudp_core.h"

/**
<<<<<<< HEAD
 * @brief Gets the IP address entered by the user.
 * @return A pointer to the entered IP string.
 */
const char* getEnteredIP(void);

/**
 * @brief Gets the nickname entered by the user.
 * @return A pointer to the entered nickname string.
 */
const char* getEnteredPseudo(void);

/**
 * @brief Allocates and positions UI elements for the connection screen.
 */
=======
    @brief Allocates and positions all UI elements for the connection screen.
*/
>>>>>>> origin/mgit-PR1-20-03
void initConnectionScreen(void);

/**
    @brief Logical update for the connection screen.
    @return true if the user clicked "Connect" with a valid IP.
*/
bool updateConnectionScreen(void);

/**
    @brief Renders the connection screen.
*/
void drawConnectionScreen(void);

/**
    @brief Adds a discovered server to the list shown on screen.
    @param ip   IP address string.
    @param name Server/room name string.
*/
void addDiscoveredRoom(const char* ip, const char* name);

/**
    @brief Returns the currently entered IP address.
    @return Pointer to internal IP buffer (null-terminated).
*/
const char* getEnteredIP(void);

#endif // UI_CONNECTION_SCREEN_H