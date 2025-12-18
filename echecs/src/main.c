#include "types.h"
#include "game.h"

int main(int argc, char* argv[]) {
    int returnCode;

    Board_t board;
    
    if ((returnCode = initGame(board)))
        return returnCode;

    char *predifinedMoves[] = {"h2h4", "g7g5", "h4g5", "f7f6", "g5f6", "f8g7", "f6g7", "g8f6", "g7h8+r", "f6g8"};
    int nbPredifinedMoves = 0;

    resetGame();

    gameLoop(board, predifinedMoves, nbPredifinedMoves);

    freeGame(board);

    return 0;
}
