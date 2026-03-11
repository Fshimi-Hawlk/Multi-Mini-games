#ifndef GENERAL_API_H
#define GENERAL_API_H

typedef enum { OK = 0, ERROR_NULL_POINTER, ERROR_ALLOC, ERROR_INVALID } Error_Et;

typedef struct BaseGame_St {
    int running;
    int paused;
    long score;
    Error_Et (*freeGame)(void*);
} BaseGame_St;

#endif
