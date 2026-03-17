#include "core/game.h"

bool isOOB(iVector2 coord) {
    return coord.x < 0 || coord.x >= SIZE_BOARD || coord.y < 0 || coord.y >= SIZE_BOARD;
}

bool selfCollision(const Snake_St* const snake, iVector2 nextHeadPos) {
    SnakeBodyPart_St* temp = snake->head;
    for (int i = 0; i < snake->bodyLength - 1 && temp != NULL; i++) {
        if (nextHeadPos.x == temp->coord.x && nextHeadPos.y == temp->coord.y) {
            return true;
        }
        temp = temp->suivant;
    }
    return false;
}

int initBoard(Board_t board, const Snake_St* const snake) {
    for (int i = 0; i < SIZE_BOARD; i++) {
        for (int j = 0; j < SIZE_BOARD; j++) {
            board[i][j] = GAME_TILE_GRASS;
        }
    }

    SnakeBodyPart_St* temp = snake->head;
    for (int i = 0; i < snake->bodyLength - 1 && temp != NULL; i++) {
        board[temp->coord.y][temp->coord.x] = GAME_TILE_BODY;
        temp = temp->suivant;
    }
    
    board[snake->tail->coord.y][snake->tail->coord.x] = GAME_TILE_HEAD;

    return 0;
}

void updateBoard(Board_t board, const Snake_St* const snake) {
    SnakeBodyPart_St* temp = snake->head;
    for (int i = 0; i < snake->bodyLength && temp != NULL; i++) {
        board[temp->coord.y][temp->coord.x] = GAME_TILE_BODY;
        temp = temp->suivant;
    }
}

void spawnApple(Board_t board) {
    iVector2 appleCoord = {0};

    do {
        appleCoord.x = rand() % SIZE_BOARD;
        appleCoord.y = rand() % SIZE_BOARD;
    } while (0
     || (board[appleCoord.y][appleCoord.x] == GAME_TILE_HEAD) 
     || (board[appleCoord.y][appleCoord.x] == GAME_TILE_BODY) 
     || (board[appleCoord.y][appleCoord.x] == GAME_TILE_APPLE)
    );

    board[appleCoord.y][appleCoord.x] = GAME_TILE_APPLE;
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
        *direction = (iVector2) {.x = 0, .y = -1};
        return true;
    }
    if (IsKeyDown(KEY_S) && !direction->y) {
        *direction = (iVector2) {.x = 0, .y = 1};
        return true;
    }
    if (IsKeyDown(KEY_A) && !direction->x) { // Lettre Q
        *direction = (iVector2) {.x = -1, .y = 0};
        return true;
    }
    if (IsKeyDown(KEY_D) && !direction->x) {
        *direction = (iVector2) {.x = 1, .y = 0};
        return true;
    }
    return false;
}

void initSnake(Snake_St* snake) {
    memset(snake, 0, sizeof(*snake));
}

bool isSnakeEmpty(Snake_St* snake) {
    return snake->head == NULL && snake->bodyLength == 0;
}

void snakeAppend(Snake_St* snake, iVector2 pos) {
    SnakeBodyPart_St* newPart;

    newPart = malloc(sizeof(SnakeBodyPart_St));
    newPart->coord = pos;
    newPart->renderPos.x = (float)pos.x;
    newPart->renderPos.y = (float)pos.y;
    newPart->suivant = NULL;

    if(isSnakeEmpty(snake)) {
        snake->head = newPart;
    } else {
        snake->tail->suivant = newPart;
    }

    snake->tail = newPart;

    snake->bodyLength++;
}

void snakeRemove(Snake_St* snake, iVector2* pos) {
    SnakeBodyPart_St* head;

    if (!isSnakeEmpty(snake)){
        head = snake->head;

        if (pos != NULL) {
            *pos = head->coord;
        }

        snake->head = head->suivant;
        free(head);

        snake->bodyLength--;
    }
}

void freeSnake(Snake_St* snake) {
    while (!isSnakeEmpty(snake)) {
        snakeRemove(snake, NULL);
    }
}
