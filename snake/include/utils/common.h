#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "raylib.h"
#include "baseTypes.h"

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
    float timer;
    float delay;
} speed_st;



typedef struct element {
    iVector2 coord;
    Vector2 renderPos;
    struct element* suivant;
} t_element;

typedef struct {
    t_element* head;
    t_element* tail;
} Queue_St;

void initQueue(Queue_St* q);
bool isQueueEmpty(Queue_St* q);
void queueAppend(Queue_St* q, iVector2 v);
void queueRemove(Queue_St* q, iVector2* v);
void freeQueue(Queue_St* q);