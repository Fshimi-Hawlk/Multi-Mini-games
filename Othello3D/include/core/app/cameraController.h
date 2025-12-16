#ifndef CORE_APP_CAMERACONTROLLER_H
#define CORE_APP_CAMERACONTROLLER_H

#include "utils/types.h"

/**
 * @brief Handles all input, camera movement, look, cursor visibility,
 *        view mode toggling, and board cell selection.
 *
 * Processes:
 * - Alt key toggle for cursor visibility (only in free camera mode)
 * - Ctrl+A and button click for switching between free camera and top-down view
 * - Mouse look and WASD movement in free camera mode (when cursor hidden)
 * - Board cell raycasting when cursor is visible
 *
 * @return The board position clicked by the player, or {-1,-1} if no valid click.
 */
s64Vector2_St updateCamera(void);

#endif // CORE_APP_CAMERACONTROLLER_H