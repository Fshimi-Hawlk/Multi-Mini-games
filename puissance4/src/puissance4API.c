/**
 * @file puissance4API.c
 * @brief Implementation of the Puissance4 (Connect Four) game API
 * @author Maxime CHAUVEAU
 * @date March 2026
 * @version 1.0.0
 * 
 * @section description Description
 * 
 * This file contains the complete implementation of the Puissance4 game,
 * including game logic, rendering, and user input handling.
 * 
 * @section game_mechanics Game Mechanics
 * 
 * The game uses a 6x7 grid where players drop colored discs.
 * The implementation includes:
 * - Board initialization and management
 * - Piece dropping with gravity simulation
 * - Win detection in all directions (horizontal, vertical, diagonal)
 * - Draw detection when board is full
 * - Player turn management
 * 
 * @section rendering Rendering
 * 
 * The game uses RayLib for graphics rendering:
 * - Blue game board background
 * - White empty cells
 * - Red discs for Player 1
 * - Yellow discs for Player 2
 * - Column selection highlighting
 * - Current player and game state UI
 */

#include "puissance4API.h"
#include "core/game.h"
#include "audio.h"
#include "utils/globals.h"
#include "utils/utils.h"
#include "../../thirdparty/raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =============================================================================
// PRIVATE DEFINES
// =============================================================================

#define PUISSANCE4_WINDOW_WIDTH 1000
#define PUISSANCE4_WINDOW_HEIGHT 800
#define PUISSANCE4_BOARD_ROWS 6
#define PUISSANCE4_BOARD_COLS 7
#define PUISSANCE4_CELL_SIZE 80
#define PUISSANCE4_PIECE_RADIUS 30

// =============================================================================
// PRIVATE TYPES
// =============================================================================

typedef enum {
    PUISSANCE4_PLAYER_NONE = 0,
    PUISSANCE4_PLAYER_1 = 1,
    PUISSANCE4_PLAYER_2 = 2
} Puissance4Player_Et;

typedef enum {
    PUISSANCE4_STATE_MENU,
    PUISSANCE4_STATE_PLAYING,
    PUISSANCE4_STATE_GAME_OVER
} Puissance4State_Et;

struct Puissance4Game_St {
    GameConfig_St config;
    Puissance4State_Et state;
    int board[PUISSANCE4_BOARD_ROWS][PUISSANCE4_BOARD_COLS];
    Puissance4Player_Et currentPlayer;
    Puissance4Player_Et winner;
    int selectedColumn;
    bool isRunning;
};

// =============================================================================
// PRIVATE FUNCTIONS
// =============================================================================

/**
 * @brief Initialize the game board
 * 
 * Sets all board cells to empty (PUISSANCE4_PLAYER_NONE).
 * Called during game initialization and reset.
 * 
 * @param game Game instance with board to initialize
 * 
 * @post All board cells are set to PUISSANCE4_PLAYER_NONE
 */
static void initBoard(Puissance4Game_St* game) {
    for (int row = 0; row < PUISSANCE4_BOARD_ROWS; row++) {
        for (int col = 0; col < PUISSANCE4_BOARD_COLS; col++) {
            game->board[row][col] = PUISSANCE4_PLAYER_NONE;
        }
    }
}

/**
 * @brief Check if a column is valid for placing a piece
 * 
 * A column is valid if it is within bounds and has at least
 * one empty cell (the top row is not filled).
 * 
 * @param game Game instance
 * @param col Column index to check (0-6)
 * @return true if column is valid for dropping, false otherwise
 */
static bool isValidColumn(const Puissance4Game_St* game, int col) {
    if (col < 0 || col >= PUISSANCE4_BOARD_COLS) {
        return false;
    }
    return game->board[0][col] == PUISSANCE4_PLAYER_NONE;
}

/**
 * @brief Drop a piece in the specified column
 * 
 * Finds the lowest empty position in the column and places
 * the current player's piece there.
 * 
 * @param game Game instance
 * @param col Column index where to drop the piece (0-6)
 * @return The row where the piece landed, or -1 if invalid
 * 
 * @post The piece is placed at the lowest available position
 * @post If placed successfully, game state is updated
 */
static int dropPiece(Puissance4Game_St* game, int col) {
    if (!isValidColumn(game, col)) {
        return -1;
    }
    
    for (int row = PUISSANCE4_BOARD_ROWS - 1; row >= 0; row--) {
        if (game->board[row][col] == PUISSANCE4_PLAYER_NONE) {
            game->board[row][col] = game->currentPlayer;
            return row;
        }
    }
    return -1;
}

/**
 * @brief Check for a win condition
 * 
 * Checks if the piece just placed at (row, col) creates a
 * winning line of 4 or more consecutive pieces of the same
 * player in any direction:
 * - Horizontal (left to right)
 * - Vertical (top to bottom)
 * - Diagonal (top-left to bottom-right)
 * - Anti-diagonal (top-right to bottom-left)
 * 
 * @param game Game instance
 * @param row Row of the last placed piece
 * @param col Column of the last placed piece
 * @return true if the current player has won, false otherwise
 */
static bool checkWin(const Puissance4Game_St* game, int row, int col) {
    int player = game->board[row][col];
    if (player == PUISSANCE4_PLAYER_NONE) {
        return false;
    }
    
    // Check horizontal
    int count = 0;
    for (int c = 0; c < PUISSANCE4_BOARD_COLS; c++) {
        if (game->board[row][c] == player) {
            count++;
            if (count >= 4) return true;
        } else {
            count = 0;
        }
    }
    
    // Check vertical
    count = 0;
    for (int r = 0; r < PUISSANCE4_BOARD_ROWS; r++) {
        if (game->board[r][col] == player) {
            count++;
            if (count >= 4) return true;
        } else {
            count = 0;
        }
    }
    
    // Check diagonal (top-left to bottom-right)
    int startRow = row - col;
    int startCol = 0;
    if (startRow > 0) {
        startCol = -startRow;
        startRow = 0;
    }
    count = 0;
    for (int i = 0; i < PUISSANCE4_BOARD_ROWS && startRow + i < PUISSANCE4_BOARD_ROWS && startCol + i < PUISSANCE4_BOARD_COLS; i++) {
        if (game->board[startRow + i][startCol + i] == player) {
            count++;
            if (count >= 4) return true;
        } else {
            count = 0;
        }
    }
    
    // Check anti-diagonal (top-right to bottom-left)
    startRow = row + col;
    startCol = PUISSANCE4_BOARD_COLS - 1;
    if (startRow >= PUISSANCE4_BOARD_ROWS) {
        startCol = startRow - (PUISSANCE4_BOARD_ROWS - 1);
        startRow = PUISSANCE4_BOARD_ROWS - 1;
    }
    count = 0;
    for (int i = 0; i < PUISSANCE4_BOARD_ROWS && startRow - i >= 0 && startCol + i < PUISSANCE4_BOARD_COLS; i++) {
        if (game->board[startRow - i][startCol + i] == player) {
            count++;
            if (count >= 4) return true;
        } else {
            count = 0;
        }
    }
    
    return false;
}

/**
 * @brief Check if the board is full (draw)
 * 
 * Checks if all cells in the top row are filled, which indicates
 * the board is completely full (since pieces stack from bottom).
 * 
 * @param game Game instance
 * @return true if board is full, false otherwise
 */
static bool isBoardFull(const Puissance4Game_St* game) {
    for (int col = 0; col < PUISSANCE4_BOARD_COLS; col++) {
        if (game->board[0][col] == PUISSANCE4_PLAYER_NONE) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Switch to the next player
 * 
 * Toggles the current player between Player 1 and Player 2.
 * Called after a successful piece drop.
 * 
 * @param game Game instance
 * 
 * @post currentPlayer is toggled to the other player
 */
static void switchPlayer(Puissance4Game_St* game) {
    game->currentPlayer = (game->currentPlayer == PUISSANCE4_PLAYER_1) ? 
                         PUISSANCE4_PLAYER_2 : PUISSANCE4_PLAYER_1;
}

/**
 * @brief Draw the game board
 * 
 * Renders the 6x7 game board with all placed pieces.
 * Uses RayLib for graphics rendering.
 * 
 * @param game Game instance containing board state
 * 
 * @note Board is drawn with blue background
 * @note Empty cells are shown as white circles
 * @note Player 1 pieces are red
 * @note Player 2 pieces are yellow
 * @note Selected column is highlighted at the top
 */
static void drawBoard(const Puissance4Game_St* game) {
    // Draw board background
    int boardWidth = PUISSANCE4_BOARD_COLS * PUISSANCE4_CELL_SIZE;
    int boardHeight = PUISSANCE4_BOARD_ROWS * PUISSANCE4_CELL_SIZE;
    int boardX = (PUISSANCE4_WINDOW_WIDTH - boardWidth) / 2;
    int boardY = (PUISSANCE4_WINDOW_HEIGHT - boardHeight) / 2;
    
    // Draw blue board
    DrawRectangle(boardX, boardY, boardWidth, boardHeight, (Color){0, 0, 180, 255});
    
    // Draw cells
    for (int row = 0; row < PUISSANCE4_BOARD_ROWS; row++) {
        for (int col = 0; col < PUISSANCE4_BOARD_COLS; col++) {
            int cellX = boardX + col * PUISSANCE4_CELL_SIZE + PUISSANCE4_CELL_SIZE / 2;
            int cellY = boardY + row * PUISSANCE4_CELL_SIZE + PUISSANCE4_CELL_SIZE / 2;
            
            // Draw empty cell (white circle)
            Color pieceColor = WHITE;
            if (game->board[row][col] == PUISSANCE4_PLAYER_1) {
                pieceColor = (Color){255, 0, 0, 255}; // Red
            } else if (game->board[row][col] == PUISSANCE4_PLAYER_2) {
                pieceColor = (Color){255, 255, 0, 255}; // Yellow
            }
            
            DrawCircle(cellX, cellY, PUISSANCE4_PIECE_RADIUS, pieceColor);
        }
    }
    
    // Draw column highlight
    if (game->selectedColumn >= 0 && game->selectedColumn < PUISSANCE4_BOARD_COLS) {
        int highlightX = boardX + game->selectedColumn * PUISSANCE4_CELL_SIZE + PUISSANCE4_CELL_SIZE / 2;
        DrawRectangle(highlightX - PUISSANCE4_CELL_SIZE / 2, boardY - 10, PUISSANCE4_CELL_SIZE, 10, (Color){100, 100, 100, 150});
    }
}

/**
 * @brief Draw game UI
 * 
 * Renders the user interface elements:
 * - Current player indicator
 * - Game over message with winner
 * - Restart instructions
 * - Control instructions
 * 
 * @param game Game instance containing current state
 */
static void drawUI(const Puissance4Game_St* game) {
    // Draw current player indicator
    const char* playerText;
    Color playerColor;
    
    if (game->currentPlayer == PUISSANCE4_PLAYER_1) {
        playerText = "Player 1 (Red)";
        playerColor = (Color){255, 0, 0, 255};
    } else {
        playerText = "Player 2 (Yellow)";
        playerColor = (Color){255, 255, 0, 255};
    }
    
    DrawText(playerText, 20, 20, 24, playerColor);
    
    // Draw game over message
    if (game->state == PUISSANCE4_STATE_GAME_OVER) {
        const char* winnerText;
        if (game->winner == PUISSANCE4_PLAYER_1) {
            winnerText = "Player 1 Wins!";
        } else if (game->winner == PUISSANCE4_PLAYER_2) {
            winnerText = "Player 2 Wins!";
        } else {
            winnerText = "It's a Draw!";
        }
        
        int textWidth = MeasureText(winnerText, 48);
        DrawText(winnerText, (PUISSANCE4_WINDOW_WIDTH - textWidth) / 2, PUISSANCE4_WINDOW_HEIGHT / 2 - 24, 48, WHITE);
        
        const char* restartText = "Press SPACE to restart";
        int restartWidth = MeasureText(restartText, 24);
        DrawText(restartText, (PUISSANCE4_WINDOW_WIDTH - restartWidth) / 2, PUISSANCE4_WINDOW_HEIGHT / 2 + 40, 24, LIGHTGRAY);
    }
    
    // Draw instructions
    const char* instructions = "Click on column to drop piece | ESC to quit";
    int instrWidth = MeasureText(instructions, 16);
    DrawText(instructions, (PUISSANCE4_WINDOW_WIDTH - instrWidth) / 2, PUISSANCE4_WINDOW_HEIGHT - 30, 16, GRAY);
}

/**
 * @brief Handle mouse input
 * 
 * Processes mouse input for piece selection and dropping:
 * - Mouse X position determines selected column
 * - Left click drops a piece in the selected column
 * 
 * @param game Game instance to update based on input
 * 
 * @note Input is only processed when game state is PLAYING
 * @note Column is determined by mouse X position relative to board
 */
static void handleInput(Puissance4Game_St* game) {
    if (game->state != PUISSANCE4_STATE_PLAYING) {
        return;
    }
    
    int mouseX = GetMouseX();
    int boardWidth = PUISSANCE4_BOARD_COLS * PUISSANCE4_CELL_SIZE;
    int boardX = (PUISSANCE4_WINDOW_WIDTH - boardWidth) / 2;
    
    // Calculate selected column
    if (mouseX >= boardX && mouseX < boardX + boardWidth) {
        game->selectedColumn = (mouseX - boardX) / PUISSANCE4_CELL_SIZE;
    } else {
        game->selectedColumn = -1;
    }
    
    // Handle click
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && game->selectedColumn >= 0) {
        int row = dropPiece(game, game->selectedColumn);
        if (row >= 0) {
            // Check for win
            if (checkWin(game, row, game->selectedColumn)) {
                game->winner = game->currentPlayer;
                game->state = PUISSANCE4_STATE_GAME_OVER;
            } else if (isBoardFull(game)) {
                game->winner = PUISSANCE4_PLAYER_NONE;
                game->state = PUISSANCE4_STATE_GAME_OVER;
            } else {
                switchPlayer(game);
            }
        }
    }
}

/**
 * @brief Reset the game
 * 
 * Resets all game state to start a new game:
 * - Clears the board
 * - Sets current player to Player 1
 * - Clears winner
 * - Sets state to PLAYING
 * - Clears column selection
 * 
 * @param game Game instance to reset
 * 
 * @post Game is ready for a new round
 */
static void resetGame(Puissance4Game_St* game) {
    initBoard(game);
    game->currentPlayer = PUISSANCE4_PLAYER_1;
    game->winner = PUISSANCE4_PLAYER_NONE;
    game->state = PUISSANCE4_STATE_PLAYING;
    game->selectedColumn = -1;
}

// =============================================================================
// PUBLIC API IMPLEMENTATION
// =============================================================================

/**
 * @brief Initialize a new Puissance4 game
 * 
 * Allocates memory for a new game instance and initializes all
 * game state to default values.
 * 
 * @param config Game configuration (can be NULL for default)
 * @return Pointer to the game instance, or NULL on allocation failure
 * 
 * @post Game board is empty
 * @post Current player is Player 1 (Red)
 * @post Game state is PLAYING
 * @post Running flag is true
 * 
 * @note If config is NULL, default audio/video settings are used
 */
Puissance4Game_St* puissance4_initGame(const GameConfig_St* config) {
    Puissance4Game_St* game = (Puissance4Game_St*)malloc(sizeof(Puissance4Game_St));
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
    
    game->state = PUISSANCE4_STATE_PLAYING;
    game->isRunning = true;
    
    // Initialize board
    initBoard(game);
    game->currentPlayer = PUISSANCE4_PLAYER_1;
    game->winner = PUISSANCE4_PLAYER_NONE;
    game->selectedColumn = -1;
    
    return game;
}

/**
 * @brief Run one frame of the game loop
 * 
 * Executes one iteration of the game loop, handling:
 * - Input processing (mouse for piece dropping, keyboard for controls)
 * - Game state updates (win detection, player switching)
 * - Rendering (board, pieces, UI)
 * 
 * This function should be called once per frame, typically at 60 FPS.
 * 
 * @param game Game instance
 * 
 * @note Handles ESC key to quit the game
 * @note Handles SPACE key to restart when game is over
 * @note Uses RayLib for all rendering
 */
void puissance4_gameLoop(Puissance4Game_St* const game) {
    if (!game || !game->isRunning) {
        return;
    }
    
    // Handle input
    handleInput(game);
    
    // Handle restart
    if (game->state == PUISSANCE4_STATE_GAME_OVER && IsKeyPressed(KEY_SPACE)) {
        resetGame(game);
    }
    
    // Handle escape to quit
    if (IsKeyPressed(KEY_ESCAPE)) {
        game->isRunning = false;
    }
    
    // Drawing
    BeginDrawing();
    ClearBackground((Color){40, 40, 60, 255});
    
    drawBoard(game);
    drawUI(game);
    
    EndDrawing();
}

/**
 * @brief Free the game resources
 * 
 * Deallocates all memory associated with the game instance.
 * After calling this function, the game pointer is invalid.
 * 
 * @param game Game instance to free (can be NULL)
 * 
 * @post All game memory is freed
 * 
 * @note Safe to call with NULL pointer (no operation)
 */
void puissance4_freeGame(Puissance4Game_St* game) {
    if (game) {
        free(game);
    }
}

/**
 * @brief Check if the game is still running
 * 
 * Returns the current running state of the game.
 * The game is running when:
 * - No player has won yet
 * - Board is not full (no draw)
 * - User has not pressed ESC to quit
 * 
 * @param game Game instance
 * @return true if game is running, false if game ended or game is NULL
 * 
 * @pre game must be a valid pointer returned by puissance4_initGame
 */
bool puissance4_isRunning(const Puissance4Game_St* game) {
    return game && game->isRunning;
}
