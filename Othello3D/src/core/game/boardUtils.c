#include "core/game/boardUtils.h"
#include "utils/config.h"
#include "utils/globals.h"
#include "utils/logger/logger.h"
#include "utils/types.h"

bool isBoardFull(const Board_t board) {
    for (u64_t y = 0; y < BOARD_SIZE; ++y)
        for (u64_t x = 0; x < BOARD_SIZE; ++x)
            if (board[y][x] == EMPTY)
                return false;
    return true;
}

s64Vector2_St mouseToBoardPos(Vector2 mousePos) {
    if (mousePos.x < boardOffset.x || mousePos.x >= boardOffset.x + BOARD_PX_SIZE||
        mousePos.y < boardOffset.y || mousePos.y >= boardOffset.y + BOARD_PX_SIZE)
        return (s64Vector2_St){-1, -1};

    return (s64Vector2_St){
        .x = (s64_t)((mousePos.x - boardOffset.x) / CELL_PX_SIZE),
        .y = (s64_t)((mousePos.y - boardOffset.y) / CELL_PX_SIZE)
    };
}

bool isOutOfBounds(s64Vector2_St pos) {
    return pos.x < 0 || pos.x >= BOARD_SIZE || pos.y < 0 || pos.y >= BOARD_SIZE;
}

bool isDirectionValid(const Board_t board, s64Vector2_St pos, s64Vector2_St dir, bool playerTurn) {
    CellState_Et own      = playerTurn ? PLAYER : COMPUTER;
    CellState_Et opponent = playerTurn ? COMPUTER : PLAYER;

    s64_t x = pos.x + dir.x;
    s64_t y = pos.y + dir.y;

    bool seenOpponent = false;

    while (!isOutOfBounds((s64Vector2_St){x, y})) {
        CellState_Et cell = board[y][x];

        if (cell == opponent)
            seenOpponent = true;
        else if (cell == own)
            return seenOpponent;          // valid bracket
        else
            return false;                 // empty or invalid

        x += dir.x;
        y += dir.y;
    }
    return false;                         // hit edge without own piece
}

bool isValidMove(const Board_t board, s64Vector2_St pos, bool playerTurn) {
    if (isOutOfBounds(pos) || board[pos.y][pos.x] != EMPTY)
        return false;

    for (s64_t dy = -1; dy <= 1; ++dy)
        for (s64_t dx = -1; dx <= 1; ++dx)
            if (!(dx == 0 && dy == 0))
                if (isDirectionValid(board, pos, (s64Vector2_St){dx, dy}, playerTurn))
                    return true;
    return false;
}

bool hasValidMoves(const Board_t board, bool playerTurn) {
    for (u64_t y = 0; y < BOARD_SIZE; ++y)
        for (u64_t x = 0; x < BOARD_SIZE; ++x)
            if (isValidMove(board, (s64Vector2_St){x, y}, playerTurn))
                return true;
    return false;
}

void countPieces(const Board_t board, u64_t* player, u64_t* computer) {
    *player = *computer = 0;
    for (u64_t y = 0; y < BOARD_SIZE; ++y)
        for (u64_t x = 0; x < BOARD_SIZE; ++x) {
            if (board[y][x] == PLAYER) (*player)++;
            if (board[y][x] == COMPUTER) (*computer)++;
        }
}

u64_t countFlipsInDirection(const Board_t board, s64Vector2_St pos, s64Vector2_St dir, bool playerTurn) {
    CellState_Et opponent = playerTurn ? COMPUTER : PLAYER;

    u64_t count = 0;
    s64_t x = pos.x + dir.x;
    s64_t y = pos.y + dir.y;

    while (!isOutOfBounds((s64Vector2_St){x, y}) && board[y][x] == opponent) {
        count++;
        x += dir.x;
        y += dir.y;
    }
    return count;
}

u64_t countTotalFlips(const Board_t board, s64Vector2_St pos, bool playerTurn) {
    u64_t total = 0;
    for (s64_t dy = -1; dy <= 1; ++dy)
        for (s64_t dx = -1; dx <= 1; ++dx)
            if (!(dx == 0 && dy == 0))
                total += countFlipsInDirection(board, pos, (s64Vector2_St){dx, dy}, playerTurn);
    return total;
}

void collectFlippedPieces(const Board_t board, s64Vector2_St pos, bool playerTurn,
                         s64Vector2_St outList[], u64_t* outCount) {
    *outCount = 0;
    for (s64_t dy = -1; dy <= 1; ++dy)
        for (s64_t dx = -1; dx <= 1; ++dx)
            if (!(dx == 0 && dy == 0)) {
                s64Vector2_St dir = {dx, dy};
                if (isDirectionValid(board, pos, dir, playerTurn)) {
                    s64_t x = pos.x + dx;
                    s64_t y = pos.y + dy;
                    CellState_Et opponent = playerTurn ? COMPUTER : PLAYER;
                    while (!isOutOfBounds((s64Vector2_St){x, y}) && board[y][x] == opponent) {
                        outList[(*outCount)++] = (s64Vector2_St){x, y};
                        x += dx;
                        y += dy;
                    }
                }
            }
}

s64Vector2_St getBoardCellFromRay(Vector2 mousePos, Camera3D cam) {
    Ray ray = GetMouseRay(mousePos, cam);

    Vector3 quadP1 = {0.0f, 0.0f, 0.0f};
    Vector3 quadP2 = {(float) BOARD_PX_SIZE, 0.0f, 0.0f};
    Vector3 quadP3 = {(float) BOARD_PX_SIZE, 0.0f, (float) BOARD_PX_SIZE};
    Vector3 quadP4 = {0.0f, 0.0f, (float) BOARD_PX_SIZE};

    RayCollision col = GetRayCollisionQuad(ray, quadP1, quadP2, quadP3, quadP4);
    if (!col.hit) return (s64Vector2_St){-1,-1};

    s64_t x = (s64_t) ((col.point.x) / CELL_PX_SIZE);
    s64_t z = (s64_t) ((col.point.z) / CELL_PX_SIZE);

    if (x >= 0 && x < BOARD_SIZE && z >= 0 && z < BOARD_SIZE)
        return (s64Vector2_St){x, z};
    return (s64Vector2_St){-1,-1};
}