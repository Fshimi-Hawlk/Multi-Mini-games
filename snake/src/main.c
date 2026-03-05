#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "raylib.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define SIZE_BOARD 20
#define MAX_LENGTH 400

#define CELL_SIZE (WINDOW_HEIGHT / SIZE_BOARD)

#define BACKGROUND_COLOR WHITE

enum {
    GRASS, HEAD, BODY, APPLE
};

typedef struct {
    int x, y;
} iVector2_st;

typedef struct {
    float timer;
    float delay;
} speed_st;

bool isOOB(void);
bool selfCollision(int bodyLength, iVector2_st headCoord);
int initBoard(int board[SIZE_BOARD][SIZE_BOARD], int bodyLength);
void updateBody(int board[SIZE_BOARD][SIZE_BOARD], int bodyLength);
void spawnApple(int board[SIZE_BOARD][SIZE_BOARD], iVector2_st* appleCoord);
void drawBoard(int board[SIZE_BOARD][SIZE_BOARD]);
void drawSnake(float interpolation, iVector2_st direction);
void writeRecord(int highScore);
int readRecord(void);
bool mouvement(iVector2_st* direction);

typedef struct element {
    iVector2_st coord;
    Vector2 renderPos;
    struct element* suivant;
} t_element;

t_element* head;
t_element* tail;

void initQueue(void);
bool isQueueEmpty(void);
void queueAppend(iVector2_st v);
void queueRemove(iVector2_st* v);
void freeQueue(void);

int main(void) {
    srand(time(NULL));
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Snake");
    SetTargetFPS(60);
    
    int board[SIZE_BOARD][SIZE_BOARD];
    
    int nbApple = 0;
    int highScore = readRecord();
    
    iVector2_st direction = {.x = 1, .y = 0}; // Direction initiale : droite

    iVector2_st nextDirection = direction;

    initQueue();
    queueAppend((iVector2_st){.x = 5, .y = 10});
    queueAppend((iVector2_st){.x = 6, .y = 10});
    queueAppend((iVector2_st){.x = 7, .y = 10});
    int bodyLength = 3;

    iVector2_st temp;

    iVector2_st appleCoord;

    speed_st speed = {0, 0.2};

    initBoard(board, bodyLength);
    spawnApple(board, &appleCoord);
    bool move = false;

    bool gameOver = false;

    iVector2_st nextPos;
    
    while (!WindowShouldClose()) {
        if (!gameOver) {
            if (!move) {
                move = mouvement(&nextDirection);
            }
            
            nextPos = (iVector2_st){.x = tail->coord.x + direction.x, .y = tail->coord.y + direction.y};
            
            if (selfCollision(bodyLength, nextPos) || nextPos.x < 0 || nextPos.x >= SIZE_BOARD || nextPos.y < 0 || nextPos.y >= SIZE_BOARD) {
                if (nbApple > highScore) {
                    writeRecord(nbApple);
                }
                gameOver = true;
            }
            else {
                speed.timer += GetFrameTime();
                
                if (speed.timer >= speed.delay) {
                    direction = nextDirection;
                    queueAppend(nextPos);
                    updateBody(board, bodyLength);

                    if (board[tail->coord.y][tail->coord.x] == APPLE) {
                        bodyLength++;
                        spawnApple(board, &appleCoord);
                        nbApple++;
                    } 
                    else {
                        queueRemove(&temp);
                        board[temp.y][temp.x] = GRASS;
                    }

                    board[tail->coord.y][tail->coord.x] = HEAD;

                    speed.timer = 0;
                    move = false;
                }
            }
        }

        float interpolation = speed.timer / speed.delay;
        if (interpolation > 1.0f)
            interpolation = 1.0f;

        BeginDrawing();
            ClearBackground(BACKGROUND_COLOR);
            
            drawBoard(board);
            drawSnake(interpolation, direction);
            
            DrawText(TextFormat("Score : %d", nbApple), CELL_SIZE * SIZE_BOARD + 10, 50, 20, BLACK);
            DrawText(TextFormat("High Score : %d", highScore), CELL_SIZE * SIZE_BOARD + 10, 100, 20, BLACK);
        EndDrawing();
    }

    freeQueue();
    CloseWindow();
    return 0;
}

bool isOOB(void) {
    if (!tail) return true;
    return tail->coord.x < 0 || tail->coord.x >= SIZE_BOARD || tail->coord.y < 0 || tail->coord.y >= SIZE_BOARD;
}

bool selfCollision(int bodyLength, iVector2_st headCoord) {
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

void spawnApple(int board[SIZE_BOARD][SIZE_BOARD], iVector2_st* appleCoord) {
    do {
        appleCoord->x = rand() % SIZE_BOARD;
        appleCoord->y = rand() % SIZE_BOARD;
    } while ((board[appleCoord->y][appleCoord->x] == HEAD) || (board[appleCoord->y][appleCoord->x] == BODY) || (board[appleCoord->y][appleCoord->x] == APPLE));

    board[appleCoord->y][appleCoord->x] = APPLE;
}

void drawBoard(int board[SIZE_BOARD][SIZE_BOARD]) {
    for (int y = 0; y < SIZE_BOARD; y++) {
        for (int x = 0; x < SIZE_BOARD; x++) {
            int posX = x * CELL_SIZE;
            int posY = y * CELL_SIZE;
            
            if ((x + y) % 2 == 0)
                DrawRectangle(posX, posY, CELL_SIZE, CELL_SIZE, (Color){100, 200, 100, 255});
            else
                DrawRectangle(posX, posY, CELL_SIZE, CELL_SIZE, (Color){80, 170, 80, 255});
            
            if (board[y][x] == APPLE) {
                DrawRectangle(posX, posY, CELL_SIZE, CELL_SIZE, RED);
            }
        }
    }
}

void drawSnake(float interpolation, iVector2_st direction) {
    t_element* current = head;
    
    while (current != NULL) {
        float renderX, renderY;
        
        if (current->suivant) {
            // Interpoler de la position actuelle vers la suivante
            renderX = current->coord.x + (current->suivant->coord.x - current->coord.x) * interpolation;
            renderY = current->coord.y + (current->suivant->coord.y - current->coord.y) * interpolation;
        } else {
            // Pour la tête
            renderX = current->coord.x + direction.x * interpolation;
            renderY = current->coord.y + direction.y * interpolation;
        }
        
        float posX = roundf(renderX * CELL_SIZE);
        float posY = roundf(renderY * CELL_SIZE);
        
        if (current == tail) {
            DrawRectangle(posX, posY, CELL_SIZE, CELL_SIZE, DARKBLUE);
        } else {
            DrawRectangle(posX, posY, CELL_SIZE, CELL_SIZE, BLUE);
        }
        
        current = current->suivant;
    }
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

bool mouvement(iVector2_st* direction) {
    if (IsKeyDown(KEY_W) && !direction->y) { // Lettre Z
        *direction = (iVector2_st){.x = 0, .y = -1};
        return true;
    }
    if (IsKeyDown(KEY_S) && !direction->y) {
        *direction = (iVector2_st){.x = 0, .y = 1};
        return true;
    }
    if (IsKeyDown(KEY_A) && !direction->x) { // Lettre Q
        *direction = (iVector2_st){.x = -1, .y = 0};
        return true;
    }
    if (IsKeyDown(KEY_D) && !direction->x) {
        *direction = (iVector2_st){.x = 1, .y = 0};
        return true;
    }
    return false;
}

void initQueue() {
	head = NULL;
	tail = NULL;
}

bool isQueueEmpty() {
	return head == NULL;
}

void queueAppend(iVector2_st v) {
	t_element* nouv;

	nouv = malloc(sizeof(t_element));
	nouv->coord = v;
    nouv->renderPos.x = (float)v.x;
	nouv->renderPos.y = (float)v.y;
	nouv->suivant = NULL;
	if(isQueueEmpty())
		head = nouv;
	else
		tail->suivant = nouv;
	tail = nouv;
}

void queueRemove(iVector2_st* v) {
	t_element* premier;

	if(!isQueueEmpty()){
		premier = head;
		*v = premier->coord;
		head = premier->suivant;
		free(premier);
	}
}

void freeQueue(void) {
	iVector2_st unused;
	while (!isQueueEmpty()) {
		queueRemove(&unused);
	}
}
