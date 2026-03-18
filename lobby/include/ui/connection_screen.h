/**
 * @file connection_screen.h
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Declaration of the connection screen scene for IP entry.
 */

#ifndef CONNECTION_SCREEN_H
#define CONNECTION_SCREEN_H

#include <stdbool.h>
#include "rudp_core.h"

/**
 * @brief Gets the IP address entered by the user.
 * @return A pointer to the entered IP string.
 */
const char* GetEnteredIP(void);

/**
 * @brief Allocates and positions UI elements for the connection screen.
 */
void InitConnectionScreen(void);

/**
 * @brief Logical update loop for the connection screen.
 * @return true if connection is triggered (valid IP + click), false otherwise.
 */
bool UpdateConnectionScreen(void);

/**
 * @brief Renders the connection screen UI.
 */
void DrawConnectionScreen(void);

/**
 * @brief Adds a discovered server to the UI list.
 * @param ip IP address of the discovered server.
 * @param name Name of the discovered server.
 */
void AddDiscoveredRoom(const char* ip, const char* name);

#endif // CONNECTION_SCREEN_H