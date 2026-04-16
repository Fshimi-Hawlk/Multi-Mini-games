/**
    @file chess_core.c
    @author Léandre BAUDET
    @date 2024-01-01
    @date 2026-04-14
    @brief Main entry point for the Chess game.
*/
#include "types.h"
#include "game.h"

#ifndef CHESS_STANDALONE
#define main chess_standalone_main
#endif

/**
    @brief Main entry point for the chess game.
    @param[in] argc Argument count
    @param[in] argv Argument vector
    @return int 0 on successful exit, error code otherwise
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
