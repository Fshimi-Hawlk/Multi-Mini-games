#include "core/game.h"

bool isOOB(iVector2 coord) {
    return coord.x < 0 || coord.x >= SIZE_BOARD || coord.y < 0 || coord.y >= SIZE_BOARD;
}

bool selfCollision(int bodyLength, iVector2 headCoord) {
    t_element* temp = head;
    for (int i = 0; i < bodyLength - 1 && temp != NULL; i++) {
        if (headCoord.x == temp->coord.x && headCoord.y == temp->coord.y) {
            return true;
        }
        temp = temp->suivant;
    }
    return false;
}

int initBoard(int board[SIZE_BOARD][SIZE_BOARD], int bodyLength) {
    for (int i = 0; i < SIZE_BOARD; i++) {
        for (int j = 0; j < SIZE_BOARD; j++) {
            board[i][j] = GRASS;
        }
    }

    t_element* temp = head;
    for (int i = 0; i < bodyLength - 1 && temp != NULL; i++) {
        board[temp->coord.y][temp->coord.x] = BODY;
        temp = temp->suivant;
    }
    board[tail->coord.y][tail->coord.x] = HEAD;

    return 0;
}

void updateBody(int board[SIZE_BOARD][SIZE_BOARD], int bodyLength) {
    t_element* temp = head;
    for (int i = 0; i < bodyLength && temp != NULL; i++) {
        board[temp->coord.y][temp->coord.x] = BODY;
        temp = temp->suivant;
    }
}

void spawnApple(int board[SIZE_BOARD][SIZE_BOARD], iVector2* appleCoord) {
    do {
        appleCoord->x = rand() % SIZE_BOARD;
        appleCoord->y = rand() % SIZE_BOARD;
    } while ((board[appleCoord->y][appleCoord->x] == HEAD) || (board[appleCoord->y][appleCoord->x] == BODY) || (board[appleCoord->y][appleCoord->x] == APPLE));

    board[appleCoord->y][appleCoord->x] = APPLE;
}

void writeRecord(int highScore) {
    FILE *fd = fopen("assets/highScore.txt", "w");
    if (!fd) {
        return;
    }

    fprintf(fd, "%d", highScore);
    fclose(fd);
}

int readRecord(void) {
    int highScore = 0;

    FILE *fd = fopen("assets/highScore.txt", "r");
    if (!fd) {
        return highScore;
    }

    fscanf(fd, "%d", &highScore);
    fclose(fd);

    return highScore;
}

bool mouvement(iVector2* direction) {
    if (IsKeyDown(KEY_W) && !direction->y) { // Lettre Z
        *direction = (iVector2){.x = 0, .y = -1};
        return true;
    }
    if (IsKeyDown(KEY_S) && !direction->y) {
        *direction = (iVector2){.x = 0, .y = 1};
        return true;
    }
    if (IsKeyDown(KEY_A) && !direction->x) { // Lettre Q
        *direction = (iVector2){.x = -1, .y = 0};
        return true;
    }
    if (IsKeyDown(KEY_D) && !direction->x) {
        *direction = (iVector2){.x = 1, .y = 0};
        return true;
    }
    return false;
}

void initQueue(Queue_St* q) {
    q->head = NULL;
    q->tail = NULL;
}

bool isQueueEmpty(Queue_St* q) {
    return q->head == NULL;
}

void queueAppend(Queue_St* q, iVector2 v) {
    t_element* nouv;

    nouv = malloc(sizeof(t_element));
    nouv->coord = v;
    nouv->renderPos.x = (float)v.x;
    nouv->renderPos.y = (float)v.y;
    nouv->suivant = NULL;
    if(isQueueEmpty(q))
        q->head = nouv;
    else
        q->tail->suivant = nouv;
    q->tail = nouv;
}

void queueRemove(Queue_St* q, iVector2* v) {
    t_element* premier;

    if (!isQueueEmpty(q)){
        premier = q->head;
        *v = premier->coord;
        q->head = premier->suivant;
        free(premier);
    }
}

void freeQueue(Queue_St* q) {
    iVector2 unused;
    while (!isQueueEmpty(q)) {
        queueRemove(q, &unused);
    }
}
