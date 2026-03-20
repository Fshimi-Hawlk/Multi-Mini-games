/**
 * @file connection_screen.h
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Declaration of the connection screen scene for IP entry.
 */

#ifndef CONNECTION_SCREEN_H
#define CONNECTION_SCREEN_H

#include <stdbool.h>

/**
 * @brief Gets the IP address entered by the user.
 * @return A pointer to the entered IP string.
 */
const char* getEnteredIP(void);

/**
 * @brief Allocates and positions UI elements for the connection screen.
 */
void initConnectionScreen(void);

/**
 * @brief Logical update loop for the connection screen.
 * @return true if connection is triggered (valid IP + click), false otherwise.
 */
bool updateConnectionScreen(void);

/**
 * @brief Renders the connection screen UI.
 */
void drawConnectionScreen(void);

/**
 * @brief Adds a discovered server to the UI list.
 * @param ip IP address of the discovered server.
 * @param name Name of the discovered server.
 */
void addDiscoveredRoom(const char* ip, const char* name);

#endif // CONNECTION_SCREEN_H