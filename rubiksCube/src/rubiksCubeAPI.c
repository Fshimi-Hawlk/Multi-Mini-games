/**
 * @file rubiksCubeAPI.c
 * @brief Implementation of the Rubik's Cube game API
 * @author Maxime CHAUVEAU
 * @date March 2026
 * 
 * @section implementation Implementation Details
 * This file contains the complete implementation of the Rubik's Cube game,
 * including:
 * - Cube state management
 * - Face rotation algorithms
 * - Input handling
 * - Rendering and display
 * 
 * @section cube_representation Cube Representation
 * The cube is represented as 6 faces, each containing a 3x3 grid of colors.
 * Each face is stored as a 2D array of RubiksCubeColor_Et values.
 * Standard cube notation is used for face identification:
 * - Front (green), Back (blue), Left (orange), Right (red)
 * - Top (white), Bottom (yellow)
 */

#include "rubiksCubeAPI.h"
#include "core/game.h"
#include "audio.h"
#include "utils/globals.h"
#include "utils/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// =============================================================================
// PRIVATE DEFINES
// =============================================================================

#define RUBIKS_CUBE_WINDOW_WIDTH 1000
#define RUBIKS_CUBE_WINDOW_HEIGHT 800
#define RUBIKS_CUBE_SIZE 3
#define RUBIKS_CUBE_FACE_SIZE 200
#define RUBIKS_CUBE_CELL_SIZE (RUBIKS_CUBE_FACE_SIZE / RUBIKS_CUBE_SIZE)

// Face colors
#define COLOR_WHITE  {255, 255, 255, 255}
#define COLOR_YELLOW {255, 255, 0, 255}
#define COLOR_RED    {255, 0, 0, 255}
#define COLOR_ORANGE {255, 165, 0, 255}
#define COLOR_BLUE   {0, 0, 255, 255}
#define COLOR_GREEN  {0, 255, 0, 255}

// =============================================================================
// PRIVATE TYPES
// =============================================================================

typedef enum {
    RUBIKS_STATE_MENU,
    RUBIKS_STATE_PLAYING,
    RUBIKS_STATE_SOLVED
} RubiksCubeState_Et;

typedef enum {
    RUBIKS_FACE_FRONT,
    RUBIKS_FACE_BACK,
    RUBIKS_FACE_LEFT,
    RUBIKS_FACE_RIGHT,
    RUBIKS_FACE_TOP,
    RUBIKS_FACE_BOTTOM
} RubiksCubeFace_Et;

typedef enum {
    RUBIKS_COLOR_WHITE,
    RUBIKS_COLOR_YELLOW,
    RUBIKS_COLOR_RED,
    RUBIKS_COLOR_ORANGE,
    RUBIKS_COLOR_BLUE,
    RUBIKS_COLOR_GREEN
} RubiksCubeColor_Et;

struct RubiksCubeGame_St {
    GameConfig_St config;
    RubiksCubeState_Et state;
    RubiksCubeColor_Et faces[6][3][3];  // 6 faces, 3x3 grid
    float rotationX;
    float rotationY;
    bool isRunning;
    int moveCount;
};

// =============================================================================
// PRIVATE FUNCTIONS
// =============================================================================

/**
 * @brief Initialize the cube to solved state
 * 
 * Sets each face of the cube to its standard solved configuration:
 * - Front face: Green
 * - Back face: Blue
 * - Left face: Orange
 * - Right face: Red
 * - Top face: White
 * - Bottom face: Yellow
 * 
 * @param game Game instance to initialize
 */
static void initCube(RubiksCubeGame_St* game) {
    // Front = Green
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            game->faces[RUBIKS_FACE_FRONT][i][j] = RUBIKS_COLOR_GREEN;
    
    // Back = Blue
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            game->faces[RUBIKS_FACE_BACK][i][j] = RUBIKS_COLOR_BLUE;
    
    // Left = Orange
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            game->faces[RUBIKS_FACE_LEFT][i][j] = RUBIKS_COLOR_ORANGE;
    
    // Right = Red
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            game->faces[RUBIKS_FACE_RIGHT][i][j] = RUBIKS_COLOR_RED;
    
    // Top = White
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            game->faces[RUBIKS_FACE_TOP][i][j] = RUBIKS_COLOR_WHITE;
    
    // Bottom = Yellow
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            game->faces[RUBIKS_FACE_BOTTOM][i][j] = RUBIKS_COLOR_YELLOW;
}

/**
 * @brief Get raylib Color from cube color enum
 * 
 * Converts the internal RubiksCubeColor_Et representation to
 * the raylib Color structure for rendering.
 * 
 * @param color Internal color enumeration value
 * @return Raylib Color structure for the given color
 */
static Color getColor(RubiksCubeColor_Et color) {
    switch (color) {
        case RUBIKS_COLOR_WHITE:  return (Color)COLOR_WHITE;
        case RUBIKS_COLOR_YELLOW: return (Color)COLOR_YELLOW;
        case RUBIKS_COLOR_RED:    return (Color)COLOR_RED;
        case RUBIKS_COLOR_ORANGE: return (Color)COLOR_ORANGE;
        case RUBIKS_COLOR_BLUE:   return (Color)COLOR_BLUE;
        case RUBIKS_COLOR_GREEN:  return (Color)COLOR_GREEN;
        default: return WHITE;
    }
}

/**
 * @brief Rotate a face 90 degrees clockwise
 * 
 * Performs an in-place rotation of a 3x3 face matrix.
 * The rotation is performed clockwise when viewed from outside the cube.
 * 
 * @param face 3x3 array representing the face to rotate
 */
static void rotateFaceClockwise(RubiksCubeColor_Et face[3][3]) {
    RubiksCubeColor_Et temp[3][3];
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            temp[i][j] = face[i][j];
    
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            face[i][j] = temp[2-j][i];
}

/**
 * @brief Rotate a face 90 degrees counter-clockwise
 * 
 * Performs an in-place rotation of a 3x3 face matrix.
 * The rotation is performed counter-clockwise when viewed from outside the cube.
 * Implemented by calling rotateFaceClockwise three times.
 * 
 * @param face 3x3 array representing the face to rotate
 */
static void rotateFaceCounterClockwise(RubiksCubeColor_Et face[3][3]) {
    for (int i = 0; i < 3; i++)
        rotateFaceClockwise(face);
    rotateFaceClockwise(face);
}

/**
 * @brief Perform a move on the cube
 * 
 * Executes a face rotation move using standard Rubik's Cube notation.
 * The function handles both the face rotation and the adjacent edge rotations
 * that are required to maintain cube integrity.
 * 
 * @param game Game instance
 * @param move Move character (F, B, R, L, U, or D)
 * 
 * @note Move counter is incremented after each successful move
 */
static void performMove(RubiksCubeGame_St* game, char move) {
    RubiksCubeColor_Et temp[3];
    
    switch (move) {
        // F - Front clockwise
        case 'F':
            rotateFaceClockwise(game->faces[RUBIKS_FACE_FRONT]);
            // Rotate edges
            for (int i = 0; i < 3; i++) temp[i] = game->faces[RUBIKS_FACE_TOP][2][i];
            for (int i = 0; i < 3; i++) game->faces[RUBIKS_FACE_TOP][2][i] = game->faces[RUBIKS_FACE_LEFT][2-i][2];
            for (int i = 0; i < 3; i++) game->faces[RUBIKS_FACE_LEFT][i][2] = game->faces[RUBIKS_FACE_BOTTOM][0][i];
            for (int i = 0; i < 3; i++) game->faces[RUBIKS_FACE_BOTTOM][0][i] = game->faces[RUBIKS_FACE_RIGHT][2-i][0];
            for (int i = 0; i < 3; i++) game->faces[RUBIKS_FACE_RIGHT][i][0] = temp[i];
            break;
            
        // B - Back clockwise
        case 'B':
            rotateFaceClockwise(game->faces[RUBIKS_FACE_BACK]);
            for (int i = 0; i < 3; i++) temp[i] = game->faces[RUBIKS_FACE_TOP][0][i];
            for (int i = 0; i < 3; i++) game->faces[RUBIKS_FACE_TOP][0][i] = game->faces[RUBIKS_FACE_RIGHT][i][2];
            for (int i = 0; i < 3; i++) game->faces[RUBIKS_FACE_RIGHT][i][2] = game->faces[RUBIKS_FACE_BOTTOM][2][2-i];
            for (int i = 0; i < 3; i++) game->faces[RUBIKS_FACE_BOTTOM][2][i] = game->faces[RUBIKS_FACE_LEFT][i][0];
            for (int i = 0; i < 3; i++) game->faces[RUBIKS_FACE_LEFT][i][0] = temp[2-i];
            break;
            
        // R - Right clockwise
        case 'R':
            rotateFaceClockwise(game->faces[RUBIKS_FACE_RIGHT]);
            for (int i = 0; i < 3; i++) temp[i] = game->faces[RUBIKS_FACE_TOP][i][2];
            for (int i = 0; i < 3; i++) game->faces[RUBIKS_FACE_TOP][i][2] = game->faces[RUBIKS_FACE_FRONT][i][2];
            for (int i = 0; i < 3; i++) game->faces[RUBIKS_FACE_FRONT][i][2] = game->faces[RUBIKS_FACE_BOTTOM][i][2];
            for (int i = 0; i < 3; i++) game->faces[RUBIKS_FACE_BOTTOM][i][2] = game->faces[RUBIKS_FACE_BACK][2-i][0];
            for (int i = 0; i < 3; i++) game->faces[RUBIKS_FACE_BACK][i][0] = temp[2-i];
            break;
            
        // L - Left clockwise
        case 'L':
            rotateFaceClockwise(game->faces[RUBIKS_FACE_LEFT]);
            for (int i = 0; i < 3; i++) temp[i] = game->faces[RUBIKS_FACE_TOP][i][0];
            for (int i = 0; i < 3; i++) game->faces[RUBIKS_FACE_TOP][i][0] = game->faces[RUBIKS_FACE_BACK][2-i][2];
            for (int i = 0; i < 3; i++) game->faces[RUBIKS_FACE_BACK][i][2] = game->faces[RUBIKS_FACE_BOTTOM][2-i][0];
            for (int i = 0; i < 3; i++) game->faces[RUBIKS_FACE_BOTTOM][i][0] = game->faces[RUBIKS_FACE_FRONT][i][0];
            for (int i = 0; i < 3; i++) game->faces[RUBIKS_FACE_FRONT][i][0] = temp[i];
            break;
            
        // U - Up clockwise
        case 'U':
            rotateFaceClockwise(game->faces[RUBIKS_FACE_TOP]);
            for (int i = 0; i < 3; i++) temp[i] = game->faces[RUBIKS_FACE_FRONT][0][i];
            for (int i = 0; i < 3; i++) game->faces[RUBIKS_FACE_FRONT][0][i] = game->faces[RUBIKS_FACE_RIGHT][0][i];
            for (int i = 0; i < 3; i++) game->faces[RUBIKS_FACE_RIGHT][0][i] = game->faces[RUBIKS_FACE_BACK][0][i];
            for (int i = 0; i < 3; i++) game->faces[RUBIKS_FACE_BACK][0][i] = game->faces[RUBIKS_FACE_LEFT][0][i];
            for (int i = 0; i < 3; i++) game->faces[RUBIKS_FACE_LEFT][0][i] = temp[i];
            break;
            
        // D - Down clockwise
        case 'D':
            rotateFaceClockwise(game->faces[RUBIKS_FACE_BOTTOM]);
            for (int i = 0; i < 3; i++) temp[i] = game->faces[RUBIKS_FACE_FRONT][2][i];
            for (int i = 0; i < 3; i++) game->faces[RUBIKS_FACE_FRONT][2][i] = game->faces[RUBIKS_FACE_LEFT][2][i];
            for (int i = 0; i < 3; i++) game->faces[RUBIKS_FACE_LEFT][2][i] = game->faces[RUBIKS_FACE_BACK][2][i];
            for (int i = 0; i < 3; i++) game->faces[RUBIKS_FACE_BACK][2][i] = game->faces[RUBIKS_FACE_RIGHT][2][i];
            for (int i = 0; i < 3; i++) game->faces[RUBIKS_FACE_RIGHT][2][i] = temp[i];
            break;
            
        default:
            break;
    }
    
    game->moveCount++;
}

/**
 * @brief Scramble the cube with random moves
 * 
 * Applies 20 random face rotations to scramble the cube.
 * Uses standard Rubik's Cube notation moves (F, B, R, L, U, D).
 * 
 * @param game Game instance to scramble
 * 
 * @note This function is called automatically during game initialization
 */
static void scrambleCube(RubiksCubeGame_St* game) {
    const char moves[] = {'F', 'B', 'R', 'L', 'U', 'D'};
    for (int i = 0; i < 20; i++) {
        char move = moves[rand() % 6];
        performMove(game, move);
    }
}

/**
 * @brief Draw a single cube face
 * 
 * Renders a 3x3 face of the cube at the specified screen position.
 * Each cell is drawn with its corresponding color and outlined with black.
 * 
 * @param face 3x3 array representing the face colors
 * @param x X position on screen (top-left corner)
 * @param y Y position on screen (top-left corner)
 * @param size Total size of the face in pixels
 */
static void drawFace(RubiksCubeColor_Et face[3][3], int x, int y, int size) {
    int cellSize = size / 3;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            Color color = getColor(face[i][j]);
            DrawRectangle(x + j * cellSize, y + i * cellSize, cellSize, cellSize, color);
            DrawRectangleLines(x + j * cellSize, y + i * cellSize, cellSize, cellSize, BLACK);
        }
    }
}

/**
 * @brief Draw the 3D cube representation
 * 
 * Renders the cube with pseudo-3D perspective based on the current
 * rotation angles. Draws the front, top, and right faces.
 * 
 * @param game Game instance containing cube state and rotation
 * 
 * @note Rotation angles are modified by arrow key input
 */
static void drawCube(const RubiksCubeGame_St* game) {
    int centerX = RUBIKS_CUBE_WINDOW_WIDTH / 2;
    int centerY = RUBIKS_CUBE_WINDOW_HEIGHT / 2;
    int faceSize = 150;
    
    // Calculate perspective
    float angleX = game->rotationX * PI / 180.0f;
    float angleY = game->rotationY * PI / 180.0f;
    
    // Draw front face (green)
    int frontX = centerX - faceSize / 2;
    int frontY = centerY - faceSize / 2;
    drawFace(game->faces[RUBIKS_FACE_FRONT], frontX, frontY, faceSize);
    
    // Draw top face (white) - positioned above
    drawFace(game->faces[RUBIKS_FACE_TOP], frontX, frontY - faceSize, faceSize);
    
    // Draw right face (red) - positioned to the right
    drawFace(game->faces[RUBIKS_FACE_RIGHT], frontX + faceSize, frontY, faceSize);
}

/**
 * @brief Draw the game user interface
 * 
 * Renders the UI elements including:
 * - Game title
 * - Move counter
 * - Control instructions
 * - Solved message (when applicable)
 * 
 * @param game Game instance
 */
static void drawUI(const RubiksCubeGame_St* game) {
    // Draw title
    const char* title = "Rubik's Cube";
    int titleWidth = MeasureText(title, 32);
    DrawText(title, (RUBIKS_CUBE_WINDOW_WIDTH - titleWidth) / 2, 20, 32, WHITE);
    
    // Draw move count
    char movesText[32];
    sprintf(movesText, "Moves: %d", game->moveCount);
    DrawText(movesText, 20, 20, 24, LIGHTGRAY);
    
    // Draw instructions
    const char* instructions = 
        "Controls:\n"
        "Arrow Keys - Rotate view\n"
        "F, B, R, L, U, D - Rotate faces\n"
        "Shift + key - Counter-clockwise\n"
        "SPACE - Scramble\n"
        "ESC - Quit";
    DrawText(instructions, 20, RUBIKS_CUBE_WINDOW_HEIGHT - 150, 16, GRAY);
    
    // Draw solved message
    if (game->state == RUBIKS_STATE_SOLVED) {
        const char* solvedText = "Cube Solved!";
        int textWidth = MeasureText(solvedText, 48);
        DrawText(solvedText, (RUBIKS_CUBE_WINDOW_WIDTH - textWidth) / 2, RUBIKS_CUBE_WINDOW_HEIGHT / 2 - 24, 48, GREEN);
    }
}

/**
 * @brief Handle user input
 * 
 * Processes all keyboard input for:
 * - Arrow keys: View rotation
 * - F, B, R, L, U, D: Face rotations (with Shift for counter-clockwise)
 * - SPACE: Scramble cube
 * - ESC: Quit game
 * 
 * @param game Game instance
 * 
 * @note Only processes input when game is in playing state
 */
static void handleInput(RubiksCubeGame_St* game) {
    if (game->state != RUBIKS_STATE_PLAYING) {
        return;
    }
    
    // View rotation with arrow keys
    if (IsKeyDown(KEY_LEFT)) game->rotationY -= 2.0f;
    if (IsKeyDown(KEY_RIGHT)) game->rotationY += 2.0f;
    if (IsKeyDown(KEY_UP)) game->rotationX -= 2.0f;
    if (IsKeyDown(KEY_DOWN)) game->rotationX += 2.0f;
    
    // Face rotations
    bool shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    
    if (IsKeyPressed(KEY_F)) {
        if (shift) {
            performMove(game, 'F');
            performMove(game, 'F');
            performMove(game, 'F');
        } else {
            performMove(game, 'F');
        }
    }
    if (IsKeyPressed(KEY_B)) {
        if (shift) {
            performMove(game, 'B');
            performMove(game, 'B');
            performMove(game, 'B');
        } else {
            performMove(game, 'B');
        }
    }
    if (IsKeyPressed(KEY_R)) {
        if (shift) {
            performMove(game, 'R');
            performMove(game, 'R');
            performMove(game, 'R');
        } else {
            performMove(game, 'R');
        }
    }
    if (IsKeyPressed(KEY_L)) {
        if (shift) {
            performMove(game, 'L');
            performMove(game, 'L');
            performMove(game, 'L');
        } else {
            performMove(game, 'L');
        }
    }
    if (IsKeyPressed(KEY_U)) {
        if (shift) {
            performMove(game, 'U');
            performMove(game, 'U');
            performMove(game, 'U');
        } else {
            performMove(game, 'U');
        }
    }
    if (IsKeyPressed(KEY_D)) {
        if (shift) {
            performMove(game, 'D');
            performMove(game, 'D');
            performMove(game, 'D');
        } else {
            performMove(game, 'D');
        }
    }
    
    // Scramble
    if (IsKeyPressed(KEY_SPACE)) {
        scrambleCube(game);
    }
    
    // Quit
    if (IsKeyPressed(KEY_ESCAPE)) {
        game->isRunning = false;
    }
}

// =============================================================================
// PUBLIC API IMPLEMENTATION
// =============================================================================

RubiksCubeGame_St* rubiksCube_initGame(const GameConfig_St* config) {
    RubiksCubeGame_St* game = (RubiksCubeGame_St*)malloc(sizeof(RubiksCubeGame_St));
    if (!game) {
        return NULL;
    }
    
    // Initialize with default config
    if (config) {
        memcpy(&game->config, config, sizeof(GameConfig_St));
    } else {
        game->config.audio = NULL;
        game->config.video = NULL;
    }
    
    game->state = RUBIKS_STATE_PLAYING;
    game->isRunning = true;
    game->rotationX = 0.0f;
    game->rotationY = 0.0f;
    game->moveCount = 0;
    
    // Initialize cube
    initCube(game);
    scrambleCube(game);
    
    return game;
}

void rubiksCube_gameLoop(RubiksCubeGame_St* const game) {
    if (!game || !game->isRunning) {
        return;
    }
    
    // Handle input
    handleInput(game);
    
    // Drawing
    BeginDrawing();
    ClearBackground((Color){40, 40, 60, 255});
    
    drawCube(game);
    drawUI(game);
    
    EndDrawing();
}

void rubiksCube_freeGame(RubiksCubeGame_St* game) {
    if (game) {
        free(game);
    }
}

bool rubiksCube_isRunning(const RubiksCubeGame_St* game) {
    return game && game->isRunning;
}
