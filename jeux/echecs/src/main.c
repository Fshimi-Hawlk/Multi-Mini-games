/**
 * @file main.c
 * @author Maxime CHAUVEAU
 * @brief Main entry point for the Echecs (Chess) game.
 * @version 1.0
 * @date 2024
 *
 * This file contains the main function which initializes the game
 * and starts the game loop.
 */

#include "utils/types.h"
#include "core/game.h"

/**
 * @brief Main entry point for the chess game.
 * @param argc Argument count
 * @param argv Argument vector
 * @return 0 on successful exit
 */
int main(int argc, char* argv[]) {
    int returnCode;

    Board_t board = {0};
    
    if ((returnCode = initGame(board)))
        return returnCode;

    char *predifinedMoves[] = {"h2h4", "g7g5", "h4g5", "f7f6", "g5f6", "f8g7", "f6g7", "g8f6", "g7h8+r", "f6g8"};
    int nbPredifinedMoves = argc > 1 ? (!strcmp(argv[1], "-p") ? 10 : 0) : 0;

    resetGame();

    gameLoop(board, predifinedMoves, nbPredifinedMoves);

    freeGame();

    return 0;
}
