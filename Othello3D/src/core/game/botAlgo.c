#include "core/game/botAlgo.h"
#include "core/game/boardUtils.h"

bool isAdjacentToCorner(s64Vector2_St pos) {
    const int last = BOARD_SIZE - 1;
    if ((pos.x <= 1 && pos.y <= 1) && !(pos.x == 0 && pos.y == 0)) return true;
    if ((pos.x >= last-1 && pos.y <= 1) && !(pos.x == last && pos.y == 0)) return true;
    if ((pos.x <= 1 && pos.y >= last-1) && !(pos.x == 0 && pos.y == last)) return true;
    if ((pos.x >= last-1 && pos.y >= last-1) && !(pos.x == last && pos.y == last)) return true;
    return false;
}

u64_t computeMoveScore(const Board_t board, s64Vector2_St pos, bool playerTurn) {
    u64_t score = 0;
    int x = pos.x, y = pos.y;

    // Corners are best
    if ((x == 0 || x == BOARD_SIZE-1) && (y == 0 || y == BOARD_SIZE-1))
        score += 100;
    // Avoid cells next to corners
    else if (isAdjacentToCorner(pos))
        score -= 50;
    // Edges are good
    else if (x == 0 || x == BOARD_SIZE-1 || y == 0 || y == BOARD_SIZE-1)
        score += 50;
    // Next-to-edge cells are bad
    else if (x == 1 || x == BOARD_SIZE-2 || y == 1 || y == BOARD_SIZE-2)
        score -= 25;

    // Mobility / number of flips
    score += 10 * countTotalFlips(board, pos, playerTurn);

    return score;
}

u64_t findBestMoveIndex(const ValidMove_St moves[], u64_t count) {
    u64_t best = 0;
    for (u64_t i = 1; i < count; ++i)
        if (moves[i].score > moves[best].score)
            best = i;
    return best;
}