#include "utils.h"
#include "global.h"

void printMovesMade(void) {
    for (int i = 0; i < nbMoves; i++) {
        printf("%s ", movesPlayed[i]);
    }
    putchar('\n');
}

IVec2_st GetMousePositionI(void) {
    Vector2 m = GetMousePosition();
    return (IVec2_st){(int)m.x, (int)m.y};
}