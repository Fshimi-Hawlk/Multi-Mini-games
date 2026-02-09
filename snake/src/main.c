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

bool isOOB();
bool selfCollision(int bodyLength);
int initBoard(int board[SIZE_BOARD][SIZE_BOARD], int bodyLength);
void updateBody(int board[SIZE_BOARD][SIZE_BOARD], int bodyLength);
void spawnApple(int board[SIZE_BOARD][SIZE_BOARD], iVector2_st* appleCoord);
void drawBoard(int board[SIZE_BOARD][SIZE_BOARD]);
void drawSnake(float interpolation, iVector2_st direction);
void writeRecord(int highScore);
int readRecord();
bool mouvement(iVector2_st* direction);

typedef struct element {
    iVector2_st coord;
    Vector2 renderPos;
    struct element* suivant;
} t_element;

t_element* tete;
t_element* queue;

void initFile(void);
bool fileVide(void);
void ajouter(iVector2_st v);
void retirer(iVector2_st* v);
void freeFile();

int main() {
    srand(time(NULL));
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Snake");
    SetTargetFPS(60);
    
    int board[SIZE_BOARD][SIZE_BOARD];
    
    int nbApple = 0;
    int highScore = readRecord();
    
    iVector2_st direction = {.x = 1, .y = 0}; // Direction initiale : droite

    iVector2_st nextDirection = direction;

    initFile();
    ajouter((iVector2_st){.x = 5, .y = 10});
    ajouter((iVector2_st){.x = 6, .y = 10});
    ajouter((iVector2_st){.x = 7, .y = 10});
    int bodyLength = 3;

    iVector2_st temp;

    iVector2_st appleCoord;

    speed_st speed = {0, 0.2};

    initBoard(board, bodyLength);
    spawnApple(board, &appleCoord);
    bool move = false;
    
    while (!WindowShouldClose()) {
        if (!move) {
            move = mouvement(&nextDirection);
        }
        if (speed.timer < 0.05) {
            direction = nextDirection;
        }

        speed.timer += GetFrameTime();

        if (speed.timer >= speed.delay) {
            ajouter((iVector2_st) {.x = queue->coord.x + direction.x, .y = queue->coord.y + direction.y});

            // Mettre à jour le board avec le corps
            updateBody(board, bodyLength);

            if (board[queue->coord.y][queue->coord.x] == APPLE) {
                bodyLength++;
                spawnApple(board, &appleCoord);
                nbApple++;
            } 
            else {
                // Si le serpent n'a pas mangé de pomme, la dernière partie du corps devient de l'herbe
                retirer(&temp);
                board[temp.y][temp.x] = GRASS;
            }

            if (isOOB() || selfCollision(bodyLength)) {
                if (nbApple > highScore) {
                    writeRecord(nbApple);
                }
                return 0;
            }

            // Met à jour le board avec la tête
            board[queue->coord.y][queue->coord.x] = HEAD;

            speed.timer = 0;
            move = false;
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
            DrawFPS(10, 10);
        EndDrawing();
    }

    freeFile();
    CloseWindow();
    return 0;
}

bool isOOB() {
    if (!queue) return true;
    return queue->coord.x < 0 || queue->coord.x >= SIZE_BOARD || queue->coord.y < 0 || queue->coord.y >= SIZE_BOARD;
}

bool selfCollision(int bodyLength) {
    t_element* temp = tete;
    for (int i = 0; i < bodyLength - 1 && temp != NULL; i++) {
        if (queue->coord.x == temp->coord.x && queue->coord.y == temp->coord.y) {
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
    
    t_element* temp = tete;
    for (int i = 0; i < bodyLength - 1 && temp != NULL; i++) {
        board[temp->coord.y][temp->coord.x] = BODY;
        temp = temp->suivant;
    }
    board[queue->coord.y][queue->coord.x] = HEAD;

    return 0;
}

void updateBody(int board[SIZE_BOARD][SIZE_BOARD], int bodyLength) {
    t_element* temp = tete;
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
    t_element* current = tete;
    
    while (current != NULL) {
        float renderX, renderY;
        
        if (current->suivant) {
            // Interpoler vers la position du suivant
            renderX = current->coord.x + (current->suivant->coord.x - current->coord.x) * interpolation;
            renderY = current->coord.y + (current->suivant->coord.y - current->coord.y) * interpolation;
        } else {
            // Pour la queue (= tête du serpent), interpoler dans la direction
            renderX = current->coord.x + direction.x * interpolation;
            renderY = current->coord.y + direction.y * interpolation;
            // printf("(%d:%d)\n", current->coord.x, current->coord.y);
        }
        

        float posX = roundf(renderX * CELL_SIZE);
        float posY = roundf(renderY * CELL_SIZE);
        
        if (current == queue) {
            DrawRectangle(posX, posY, CELL_SIZE, CELL_SIZE, DARKBLUE);  // Tête
        } else {
            DrawRectangle(posX, posY, CELL_SIZE, CELL_SIZE, BLUE);  // Corps
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

int readRecord() {
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

void initFile() {
	tete = NULL;
	queue = NULL;
}

bool fileVide() {
	return tete == NULL;
}

void ajouter(iVector2_st v) {
	t_element* nouv;

	nouv = malloc(sizeof(t_element));
	nouv->coord = v;
    nouv->renderPos.x = (float)v.x;
	nouv->renderPos.y = (float)v.y;
	nouv->suivant = NULL;
	if(fileVide())
		tete = nouv;
	else
		queue->suivant = nouv;
	queue = nouv;
}

void retirer(iVector2_st* v) {
	t_element* premier;

	if(!fileVide()){
		premier = tete;
		*v = premier->coord;
		tete = premier->suivant;
		free(premier);
	}
}

void freeFile() {
	iVector2_st unused;
	while (!fileVide()) {
		retirer(&unused);
	}
}
