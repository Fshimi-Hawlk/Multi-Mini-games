#ifndef TETRIS_API_H

typedef struct TetrisGame_St TetrisGame_St;

TetrisGame_St* tetris_initGame(void);
void tetris_gameLoop(TetrisGame_St* const game);
void tetris_freeGame(TetrisGame_St** game);

#endif