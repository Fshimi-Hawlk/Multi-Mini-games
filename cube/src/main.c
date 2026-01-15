#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// Couleurs (elles ne changent pas)
typedef enum {
    COLOR_BLUE,
    COLOR_ORANGE,
    COLOR_GREEN,
    COLOR_RED,
    COLOR_WHITE,
    COLOR_YELLOW
} ColorElmt;

typedef struct {
    ColorElmt back[3][3];
    ColorElmt left[3][3];
    ColorElmt front[3][3];
    ColorElmt right[3][3];
    ColorElmt up[3][3];
    ColorElmt down[3][3];
} Cube;

char *movesName[6][3] = {
    {"R", "R2", "R'"},
    {"L", "L2", "L'"},
    {"U", "U2", "U'"},
    {"D", "D2", "D'"},
    {"F", "F2", "F'"},
    {"B", "B2", "B'"},
};

void initCube(Cube *cube);
void printCube(Cube *cube);

void faceRotation(ColorElmt (*face)[3][3], int clockwise);

void rotateXcube(Cube *cube, int clockwise);
void rotateYcube(Cube *cube, int clockwise);
void rotateZcube(Cube *cube, int clockwise);

void rotateRcube(Cube *cube, int clockwise);
void rotateLcube(Cube *cube, int clockwise);
void rotateUcube(Cube *cube, int clockwise);
void rotateDcube(Cube *cube, int clockwise);
void rotateFcube(Cube *cube, int clockwise);
void rotateBcube(Cube *cube, int clockwise);

int keyToMove[6] = {KEY_R, KEY_L, KEY_U, KEY_D, KEY_F, KEY_B};

typedef void (*MoveCallback)(Cube*, int);

typedef struct { 
    int key; 
    MoveCallback rotate;
} KeyFunction;

const KeyFunction moveKeyToFunction[] = {
    {KEY_X, rotateXcube},
    {KEY_Y, rotateYcube},
    {KEY_W, rotateZcube}, // Z pour azerty
    {KEY_R, rotateRcube},
    {KEY_L, rotateLcube},
    {KEY_U, rotateUcube},
    {KEY_D, rotateDcube},
    {KEY_F, rotateFcube},
    {KEY_B, rotateBcube}
};

typedef struct { 
    char letterMove; 
    MoveCallback rotate;
} CharFunction;

#define MOVES_COUNT 9

const CharFunction moveCharToFunction[MOVES_COUNT] = {
    {'X', rotateXcube},
    {'Y', rotateYcube},
    {'Z', rotateZcube},
    {'R', rotateRcube},
    {'L', rotateLcube},
    {'U', rotateUcube},
    {'D', rotateDcube},
    {'F', rotateFcube},
    {'B', rotateBcube}
};

float t;

void scrambleMoves(char *moves[20]);
void scrambleMovesToString(char (*movesString)[60], char *moves[20]);

typedef struct {
    bool isAnimating;
    float t, tAnim, duration;
    int iMove;
    bool movedFinished;
} ScrambleAnimation;

ScrambleAnimation sa = {0};

void initScrambleAnimation(void);

void animateScramble(Cube *cube, char *moves[20]);

Color getRaylibColor(ColorElmt color);

void display3D(Cube *cube, Camera3D camera);

void mouvement(Cube *cube);

void UpdateCameraOrbit(Camera3D *camera, Vector3 target, float radius, float *angleX, float *angleY);

bool isCubeSolve(Cube* cube);

void writeTimer(double timer);
void readLastTimers(int timersArray[5], int* n);
int readBestTimer();

int main(void) {
    srand(time(NULL));

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Cube");

    Camera3D camera = {0};
    camera.position = (Vector3){6.0f, 6.0f, 6.0f}; // position caméra
    camera.target = (Vector3){0.0f, 0.0f, 0.0f}; // regarde le centre
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    SetTargetFPS(60);

    float angleX = 1.0f;  // angle horizontal
    float angleY = 0.5f;  // angle vertical
    float radius = 8.0f;  // distance au cube

    Cube cube;
    initCube(&cube);

    char *moves[20];
    char movesString[60] = "";

    double startTime = 0, timer = 0;
    int min, sec;

    bool wasSolved = 1;

    int timersArray[5];
    int nTimers;
    int temp;
    int bestTimer;

    bool printTimers = true;

    readLastTimers(timersArray, &nTimers);
    temp = readBestTimer();
    if (temp == -1) return 1;
    bestTimer = temp;

    while (!WindowShouldClose()) {
        UpdateCameraOrbit(&camera, (Vector3){0,0,0}, radius, &angleX, &angleY);
        
        if (IsKeyPressed(KEY_ONE)) {
            angleX = 1.0f;
            angleY = 0.5f;
            radius = 8.0f;
        }

        if (IsKeyPressed(KEY_P))
            printCube(&cube);

        if (IsKeyPressed(KEY_SEMICOLON)) { // Lettre M pour clavier azerty
            initCube(&cube);
            scrambleMoves(moves);
            scrambleMovesToString(&movesString, moves);

            initScrambleAnimation();
            startTime = 0, timer = 0;
            startTime = GetTime();
            wasSolved = false;
        }
        if (sa.isAnimating) {
            animateScramble(&cube, moves);
        }

        mouvement(&cube);
        if (!isCubeSolve(&cube)) {
            timer = GetTime() - startTime;
        }

        if (isCubeSolve(&cube) && timer > 0 && !wasSolved) {
            writeTimer(timer);
            readLastTimers(timersArray, &nTimers);
            temp = readBestTimer();
            if (temp == -1) return 1;
            bestTimer = temp;
            wasSolved = true;
        }

        if (IsKeyPressed(KEY_T)) printTimers = !printTimers;

        BeginDrawing();
            ClearBackground((Color){220, 220, 220, 255});
            DrawFPS(10, 10);
            if (startTime != 0) {
                min = timer / 60;
                sec = (int)timer % 60;
                DrawText(TextFormat("%02d:%02d", min, sec), 10, WINDOW_HEIGHT - 30, 20, BLACK);
            }
            DrawText(movesString, 150, 10, 20, BLACK);
            display3D(&cube, camera);

            if (printTimers) {
                DrawRectangle(WINDOW_WIDTH - 200, WINDOW_HEIGHT - 200, 200, 200, (Color){0, 0, 0, 128});
                for (int i = 0; i < nTimers; i++) {
                    min = timersArray[i] / 60;
                    sec = (int)timersArray[i] % 60;
                    DrawText(TextFormat("%d : %02d:%02d", i + 1, min, sec), WINDOW_WIDTH - 200, WINDOW_HEIGHT - 200 + 20 * i, 20, BLACK);
                }
                min = bestTimer / 60;
                sec = (int)bestTimer % 60;
                DrawText(TextFormat("Meilleur temps :\n%02d:%02d", min, sec), WINDOW_WIDTH - 200, WINDOW_HEIGHT - 40, 20, BLACK);
            }
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}

// Initialisation du cube
void initCube(Cube *cube) {
    // Initialisation des faces
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            cube->back[i][j] = COLOR_BLUE;
            cube->left[i][j] = COLOR_ORANGE;
            cube->front[i][j] = COLOR_GREEN;
            cube->right[i][j] = COLOR_RED;
            cube->up[i][j] = COLOR_WHITE;
            cube->down[i][j] = COLOR_YELLOW;
        }
    }
}

void printCube(Cube *cube) {
    const char COLORNAMES[] = {'B', 'O', 'G', 'R', 'W', 'Y'};
    
    // Affiche la face de haut
    for (int i = 0; i < 3; i++) {
        printf("              ");
        for (int j = 0; j < 3; j++) {
            printf("%c ", COLORNAMES[cube->up[i][j]]);
        }
        printf("\n");
    }

    // Affiche les faces (derrière, gauche, devant, droite)
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%c ", COLORNAMES[cube->back[i][j]]);
        }
        printf(" ");
        for (int j = 0; j < 3; j++) {
            printf("%c ", COLORNAMES[cube->left[i][j]]);
        }
        printf(" ");
        for (int j = 0; j < 3; j++) {
            printf("%c ", COLORNAMES[cube->front[i][j]]);
        }
        printf(" ");
        for (int j = 0; j < 3; j++) {
            printf("%c ", COLORNAMES[cube->right[i][j]]);
        }
        printf("\n");
    }

    // Affiche la face de bas
    for (int i = 0; i < 3; i++) {
        printf("              ");
        for (int j = 0; j < 3; j++) {
            printf("%c ", COLORNAMES[cube->down[i][j]]);
        }
        printf("\n");
    }
    printf("\n\n");
}

// Rotation d'une face du cube, clockwise = 1 : horaire ; 0 : contraire
void faceRotation(ColorElmt (*face)[3][3], int clockwise) {
    ColorElmt tempElmtColor;
    
    if (clockwise) {
        tempElmtColor = (*face)[0][0];
        (*face)[0][0] = (*face)[2][0];
        (*face)[2][0] = (*face)[2][2];
        (*face)[2][2] = (*face)[0][2];
        (*face)[0][2] = tempElmtColor;

        tempElmtColor = (*face)[0][1];
        (*face)[0][1] = (*face)[1][0];
        (*face)[1][0] = (*face)[2][1];
        (*face)[2][1] = (*face)[1][2];
        (*face)[1][2] = tempElmtColor;
    }
    else {
        tempElmtColor = (*face)[0][2];
        (*face)[0][2] = (*face)[2][2];
        (*face)[2][2] = (*face)[2][0];
        (*face)[2][0] = (*face)[0][0];
        (*face)[0][0] = tempElmtColor;

        tempElmtColor = (*face)[0][1];
        (*face)[0][1] = (*face)[1][2];
        (*face)[1][2] = (*face)[2][1];
        (*face)[2][1] = (*face)[1][0];
        (*face)[1][0] = tempElmtColor;
    }
}

// Rotation du cube dans l'axe x, clockwise = 1 : horaire ; 0 : contraire
void rotateXcube(Cube *cube, int clockwise) {
    ColorElmt tempFace[3][3];
    
    if (clockwise) {
        faceRotation(&cube->back, 1);
        faceRotation(&cube->back, 1);
        
        memcpy(tempFace, cube->back, sizeof(tempFace));
        memcpy(cube->back, cube->up, sizeof(tempFace));
        memcpy(cube->up, cube->front, sizeof(tempFace));
        memcpy(cube->front, cube->down, sizeof(tempFace));
        memcpy(cube->down, tempFace, sizeof(tempFace));

        faceRotation(&cube->back, 1);
        faceRotation(&cube->back, 1);

        faceRotation(&cube->left, 0);

        faceRotation(&cube->right, 1);
    }
    else {
        faceRotation(&cube->back, 1);
        faceRotation(&cube->back, 1);
        
        memcpy(tempFace, cube->back, sizeof(tempFace));
        memcpy(cube->back, cube->down, sizeof(tempFace));
        memcpy(cube->down, cube->front, sizeof(tempFace));
        memcpy(cube->front, cube->up, sizeof(tempFace));
        memcpy(cube->up, tempFace, sizeof(tempFace));
        
        faceRotation(&cube->back, 1);
        faceRotation(&cube->back, 1);
        
        faceRotation(&cube->left, 1);

        faceRotation(&cube->right, 0);
    }
}

// Rotation du cube dans l'axe y, clockwise = 1 : horaire ; 0 : contraire
void rotateYcube(Cube *cube, int clockwise) {
    ColorElmt tempFace[3][3];
    
    if (clockwise) {
        memcpy(tempFace, cube->back, sizeof(tempFace));
        memcpy(cube->back, cube->left, sizeof(tempFace));
        memcpy(cube->left, cube->front, sizeof(tempFace));
        memcpy(cube->front, cube->right, sizeof(tempFace));
        memcpy(cube->right, tempFace, sizeof(tempFace));

        faceRotation(&cube->up, 1);

        faceRotation(&cube->down, 0);
    }
    else {
        memcpy(tempFace, cube->back, sizeof(tempFace));
        memcpy(cube->back, cube->right, sizeof(tempFace));
        memcpy(cube->right, cube->front, sizeof(tempFace));
        memcpy(cube->front, cube->left, sizeof(tempFace));
        memcpy(cube->left, tempFace, sizeof(tempFace));

        faceRotation(&cube->up, 0);

        faceRotation(&cube->down, 1);
    }
}

// Rotation du cube dans l'axe z, clockwise = 1 : horaire ; 0 : contraire
void rotateZcube(Cube *cube, int clockwise) {
    ColorElmt tempFace[3][3];
    
    if (clockwise) {
        memcpy(tempFace, cube->up, sizeof(tempFace));
        memcpy(cube->up, cube->left, sizeof(tempFace));
        memcpy(cube->left, cube->down, sizeof(tempFace));
        memcpy(cube->down, cube->right, sizeof(tempFace));
        memcpy(cube->right, tempFace, sizeof(tempFace));

        faceRotation(&cube->up, 1);
        faceRotation(&cube->right, 1);
        faceRotation(&cube->down, 1);
        faceRotation(&cube->left, 1);

        faceRotation(&cube->front, 1);
        faceRotation(&cube->back, 0);
    }
    else {
        memcpy(tempFace, cube->up, sizeof(tempFace));
        memcpy(cube->up, cube->right, sizeof(tempFace));
        memcpy(cube->right, cube->down, sizeof(tempFace));
        memcpy(cube->down, cube->left, sizeof(tempFace));
        memcpy(cube->left, tempFace, sizeof(tempFace));

        faceRotation(&cube->up, 0);
        faceRotation(&cube->right, 0);
        faceRotation(&cube->down, 0);
        faceRotation(&cube->left, 0);

        faceRotation(&cube->front, 0);
        faceRotation(&cube->back, 1);
    }
}

void rotateRcube(Cube *cube, int clockwise) {
    ColorElmt tempElmtCol;

    if (clockwise) {
        faceRotation(&cube->right, 1);

        faceRotation(&cube->back, 1);
        faceRotation(&cube->back, 1);

        for (int i = 0; i < 3; i++) {
            tempElmtCol = cube->front[i][2];
            cube->front[i][2] = cube->down[i][2];
            cube->down[i][2] = cube->back[i][2];
            cube->back[i][2] = cube->up[i][2];
            cube->up[i][2] = tempElmtCol;
        }
        
        faceRotation(&cube->back, 1);
        faceRotation(&cube->back, 1);
    }
    else {
        faceRotation(&cube->right, 0);
        
        faceRotation(&cube->back, 1);
        faceRotation(&cube->back, 1);

        for (int i = 0; i < 3; i++) {
            tempElmtCol = cube->front[i][2];
            cube->front[i][2] = cube->up[i][2];
            cube->up[i][2] = cube->back[i][2];
            cube->back[i][2] = cube->down[i][2];
            cube->down[i][2] = tempElmtCol;
        }

        faceRotation(&cube->back, 1);
        faceRotation(&cube->back, 1);
    }
}

void rotateLcube(Cube *cube, int clockwise) {
    ColorElmt tempElmtCol;

    if (clockwise) {
        faceRotation(&cube->left, 1);
        
        //symetrieVertical(&cube->back);
        faceRotation(&cube->back, 1);
        faceRotation(&cube->back, 1);

        for (int i = 0; i < 3; i++) {
            tempElmtCol = cube->front[i][0];
            cube->front[i][0] = cube->up[i][0];
            cube->up[i][0] = cube->back[i][0];
            cube->back[i][0] = cube->down[i][0];
            cube->down[i][0] = tempElmtCol;
        }
        
        //symetrieVertical(&cube->back);
        faceRotation(&cube->back, 1);
        faceRotation(&cube->back, 1);
    }
    else {
        faceRotation(&cube->left, 0);
        
        //symetrieVertical(&cube->back);
        faceRotation(&cube->back, 1);
        faceRotation(&cube->back, 1);
        
        for (int i = 0; i < 3; i++) {
            tempElmtCol = cube->front[i][0];
            cube->front[i][0] = cube->down[i][0];
            cube->down[i][0] = cube->back[i][0];
            cube->back[i][0] = cube->up[i][0];
            cube->up[i][0] = tempElmtCol;
        }

        //symetrieVertical(&cube->back);
        faceRotation(&cube->back, 1);
        faceRotation(&cube->back, 1);
    }
}

void rotateUcube(Cube *cube, int clockwise) {
    ColorElmt tempRow[3];
    
    if (clockwise) {
        faceRotation(&cube->up, 1);

        memcpy(tempRow, cube->front, sizeof(tempRow));
        memcpy(cube->front, cube->right, sizeof(tempRow));
        memcpy(cube->right, cube->back, sizeof(tempRow));
        memcpy(cube->back, cube->left, sizeof(tempRow));
        memcpy(cube->left, tempRow, sizeof(tempRow));
    }
    else {
        faceRotation(&cube->up, 0);

        memcpy(tempRow, cube->front, sizeof(tempRow));
        memcpy(cube->front, cube->left, sizeof(tempRow));
        memcpy(cube->left, cube->back, sizeof(tempRow));
        memcpy(cube->back, cube->right, sizeof(tempRow));
        memcpy(cube->right, tempRow, sizeof(tempRow));
    }
}

void rotateDcube(Cube *cube, int clockwise) {
    ColorElmt tempRow[3];
    
    if (clockwise) {
        faceRotation(&cube->down, 1);

        memcpy(tempRow, cube->front[2], sizeof(tempRow));
        memcpy(cube->front[2], cube->left[2], sizeof(tempRow));
        memcpy(cube->left[2], cube->back[2], sizeof(tempRow));
        memcpy(cube->back[2], cube->right[2], sizeof(tempRow));
        memcpy(cube->right[2], tempRow, sizeof(tempRow));
    }
    else {
        faceRotation(&cube->down, 0);

        memcpy(tempRow, cube->front[2], sizeof(tempRow));
        memcpy(cube->front[2], cube->right[2], sizeof(tempRow));
        memcpy(cube->right[2], cube->back[2], sizeof(tempRow));
        memcpy(cube->back[2], cube->left[2], sizeof(tempRow));
        memcpy(cube->left[2], tempRow, sizeof(tempRow));
    }
}

void rotateFcube(Cube *cube, int clockwise) {
    ColorElmt tempElmtColor;

    if (clockwise) {
        faceRotation(&cube->front, 1);

        tempElmtColor = cube->up[2][0];
        cube->up[2][0] = cube->left[2][2];
        cube->left[2][2] = cube->down[0][2];
        cube->down[0][2] = cube->right[0][0];
        cube->right[0][0] = tempElmtColor;

        tempElmtColor = cube->up[2][2];
        cube->up[2][2] = cube->left[0][2];
        cube->left[0][2] = cube->down[0][0];
        cube->down[0][0] = cube->right[2][0];
        cube->right[2][0] = tempElmtColor;

        tempElmtColor = cube->up[2][1];
        cube->up[2][1] = cube->left[1][2];
        cube->left[1][2] = cube->down[0][1];
        cube->down[0][1] = cube->right[1][0];
        cube->right[1][0] = tempElmtColor;
    }
    else {
        faceRotation(&cube->front, 0);

        tempElmtColor = cube->up[2][0];
        cube->up[2][0] = cube->right[0][0];
        cube->right[0][0] = cube->down[0][2];
        cube->down[0][2] = cube->left[2][2];
        cube->left[2][2] = tempElmtColor;

        tempElmtColor = cube->up[2][2];
        cube->up[2][2] = cube->right[2][0];
        cube->right[2][0] = cube->down[0][0];
        cube->down[0][0] = cube->left[0][2];
        cube->left[0][2] = tempElmtColor;

        tempElmtColor = cube->up[2][1];
        cube->up[2][1] = cube->right[1][0];
        cube->right[1][0] = cube->down[0][1];
        cube->down[0][1] = cube->left[1][2];
        cube->left[1][2] = tempElmtColor;
    }
}

void rotateBcube(Cube *cube, int clockwise) {
    ColorElmt tempElmtColor;

    if (clockwise) {
        faceRotation(&cube->back, 1);

        tempElmtColor = cube->up[0][0];
        cube->up[0][0] = cube->right[0][2];
        cube->right[0][2] = cube->down[2][2];
        cube->down[2][2] = cube->left[2][0];
        cube->left[2][0] = tempElmtColor;

        tempElmtColor = cube->up[0][2];
        cube->up[0][2] = cube->right[2][2];
        cube->right[2][2] = cube->down[2][0];
        cube->down[2][0] = cube->left[0][0];
        cube->left[0][0] = tempElmtColor;

        tempElmtColor = cube->up[0][1];
        cube->up[0][1] = cube->right[1][2];
        cube->right[1][2] = cube->down[2][1];
        cube->down[2][1] = cube->left[1][0];
        cube->left[1][0] = tempElmtColor;
    }
    else {
        faceRotation(&cube->back, 0);

        tempElmtColor = cube->up[0][0];
        cube->up[0][0] = cube->left[2][0];
        cube->left[2][0] = cube->down[2][2];
        cube->down[2][2] = cube->right[0][2];
        cube->right[0][2] = tempElmtColor;

        tempElmtColor = cube->up[0][2];
        cube->up[0][2] = cube->left[0][0];
        cube->left[0][0] = cube->down[2][0];
        cube->down[2][0] = cube->right[2][2];
        cube->right[2][2] = tempElmtColor;

        tempElmtColor = cube->up[0][1];
        cube->up[0][1] = cube->left[1][0];
        cube->left[1][0] = cube->down[2][1];
        cube->down[2][1] = cube->right[1][2];
        cube->right[1][2] = tempElmtColor;
    }
}

// Couleurs Raylib correspondantes à tes ColorElmt
Color getRaylibColor(ColorElmt color) {
    Color colors[] = {BLUE, ORANGE, GREEN, RED, WHITE, YELLOW};
    return colors[color];
}

void display3D(Cube *cube, Camera3D camera) {
    BeginMode3D(camera);
    float size = 1.0f;  // taille d’un petit cube
    float offset = size * 1.1f; // petit espace pour séparer visuellement les cubes
    float space = offset * 1.5f;
    float sizeBlack = size * 1.15f;
    
    Color centerColor = BLACK;

    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            for (int z = -1; z <= 1; z++) {
                // Position du mini-cube (centre)
                Vector3 pos = { x * offset, y * offset, z * offset };

                // Dessiner le mini-cube de base (gris foncé)
                DrawCube(pos, sizeBlack, sizeBlack, sizeBlack, centerColor);
            }
        }
    }

    // On dessine face par face
    for (int i = 0; i < 3; i++) { // lignes
        for (int j = 0; j < 3; j++) { // colonnes
            // face avant (Z+)
            Vector3 posFront = {(j - 1) * offset, (1 - i) * offset, space};
            DrawCube(posFront, size, size, 0.1f, getRaylibColor(cube->front[i][j]));

            // face arrière (Z-)
            Vector3 posBack = {(1 - j) * offset, (1 - i) * offset, -space};
            DrawCube(posBack, size, size, 0.1f, getRaylibColor(cube->back[i][j]));
            
            // face gauche (X-)
            Vector3 posLeft = {-space, (1 - i) * offset, (j - 1) * offset};
            DrawCube(posLeft, 0.1f, size, size, getRaylibColor(cube->left[i][j]));
            
            // face droite (X+)
            Vector3 posRight = {space, (1 - i) * offset, (1 - j) * offset};
            DrawCube(posRight, 0.1f, size, size, getRaylibColor(cube->right[i][j]));
            
            // face haut (Y+)
            Vector3 posUp = {(j - 1) * offset, space, (i - 1) * offset};
            DrawCube(posUp, size, 0.1f, size, getRaylibColor(cube->up[i][j]));
            
            // face bas (Y-)
            Vector3 posDown = {(j - 1) * offset, -space, (1 - i) * offset};
            DrawCube(posDown, size, 0.1f, size, getRaylibColor(cube->down[i][j]));
        }
    }
    EndMode3D();
}

void mouvement(Cube *cube) {
    // Tableau associant chaque touche à sa fonction de rotation
    int n = sizeof(moveKeyToFunction) / sizeof(moveKeyToFunction[0]);
    int clockwise;

    for (int i = 0; i < n; i++) {
        if (IsKeyPressed(moveKeyToFunction[i].key)) {
            // Si shift est enfoncé → sens horaire (1), sinon antihoraire (0)
            clockwise = IsKeyDown(KEY_LEFT_SHIFT) ? 0 : 1;
            moveKeyToFunction[i].rotate(cube, clockwise);
        }
    }
}

void UpdateCameraOrbit(Camera3D *camera, Vector3 target, float radius, float *angleX, float *angleY) {
    // Sensibilité du mouvement
    float speed = 0.02f;
    
    // Flèches gauche/droite -> tourner autour de Y
    if (IsKeyDown(KEY_RIGHT)) *angleX -= speed;
    if (IsKeyDown(KEY_LEFT))  *angleX += speed;
    
    // Flèches haut/bas -> inclinaison verticale (limitée)
    if (IsKeyDown(KEY_UP))    *angleY += speed;
    if (IsKeyDown(KEY_DOWN))  *angleY -= speed;
    
    if (*angleY > PI/2.0f - 0.1f) *angleY = PI/2.0f - 0.1f; // limite vers le haut
    if (*angleY < -PI/2.0f + 0.1f) *angleY = -PI/2.0f + 0.1f; // limite vers le bas
    
    // Nouvelle position caméra en coordonnées sphériques
    camera->position.x = target.x + radius * cosf(*angleY) * cosf(*angleX);
    camera->position.y = target.y + radius * sinf(*angleY);
    camera->position.z = target.z + radius * cosf(*angleY) * sinf(*angleX);
    
    camera->target = target; // toujours regarder le cube
}

void scrambleMoves(char *moves[20]) {
    
    int previousMoveId = -1;

    int moveId;
    int moveType;

    for (int moveCount = 0; moveCount < 20; moveCount++) {
        moveType = rand() % 3;
        do {
            moveId = rand() % 6;
        } while (previousMoveId == moveId);

        moves[moveCount] = movesName[moveId][moveType];
        previousMoveId = moveId;
    }
}

void scrambleMovesToString(char (*movesString)[60], char *moves[20]) {
    size_t pos = 0;

    for (int i = 0; i < 20; i++) {
        pos += snprintf(*movesString + pos, sizeof(*movesString) - pos, "%s ", moves[i]);
        if (pos >= sizeof(*movesString)) 
            break;  // éviter overflow
    }
}

void initScrambleAnimation(void) {
    sa.movedFinished = true;
    sa.isAnimating = true;
    sa.t = 0.0f, 
    sa.tAnim = 0.0f,
    sa.duration = 0.05f;
    sa.iMove = 0;
}

void animateScramble(Cube *cube, char *moves[20]) {
    if (sa.iMove != 20) {
        if (sa.movedFinished) {
            sa.movedFinished = false;
            bool clockwise;
            for (int i = 0; i < MOVES_COUNT; i++) {
                if (moves[sa.iMove][0] == moveCharToFunction[i].letterMove) {
                    if (strlen(moves[sa.iMove]) == 1)
                        clockwise = true;
                    else if (moves[sa.iMove][1] == '2') {
                        clockwise = true;
                        moveCharToFunction[i].rotate(cube, clockwise);
                    }
                    else
                        clockwise = false;
                    
                    moveCharToFunction[i].rotate(cube, clockwise);
                }
            }
        }
        else {
            sa.t += GetFrameTime();
            sa.tAnim = fminf(sa.t / sa.duration, 1.0f);

            if (sa.tAnim >= 1) {
                sa.t = 0.0f;
                sa.tAnim = 0.0f;
                sa.movedFinished = true;
                sa.iMove++;
            }
        }
    }
    else { // animations terminées
        sa.isAnimating = false;
    }
}

bool isCubeSolve(Cube* cube) {
    ColorElmt colorBack = cube->back[1][1];
    ColorElmt colorLeft = cube->left[1][1];
    ColorElmt colorFront = cube->front[1][1];
    ColorElmt colorRight = cube->right[1][1];
    ColorElmt colorUp = cube->up[1][1];
    ColorElmt colorDown = cube->down[1][1];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (cube->back[i][j] != colorBack) return false;
            if (cube->left[i][j] != colorLeft) return false;
            if (cube->front[i][j] != colorFront) return false;
            if (cube->right[i][j] != colorRight) return false;
            if (cube->up[i][j] != colorUp) return false;
            if (cube->down[i][j] != colorDown) return false;
        }
    }
    return true;
}

void writeTimer(double timer) {
    FILE *fOld = fopen("assets/timers.txt", "r");
    if (!fOld) {
        printf("Problème ouverture fichier timers.\n");
        return;
    }

    FILE *fTemp = fopen("assets/temp.txt", "w");
    if (!fTemp) {
        printf("Problème ouverture fichier temp.\n");
        fclose(fOld);
        return;
    }

    fprintf(fTemp, "%d\n", (int)timer);

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fOld)) {
        fputs(buffer, fTemp);
    }

    fclose(fOld);
    fclose(fTemp);

    remove("assets/timers.txt");
    rename("assets/temp.txt", "assets/timers.txt");
}

void readLastTimers(int timersArray[5], int* n) {
    FILE *fd = fopen("assets/timers.txt", "r");
    if (!fd) {
        printf("Problème ouverture fichier timers.\n");
        return;
    }

    *n = 0;
    while (fscanf(fd, "%d", &timersArray[*n]) == 1 && *n < 5) {
        (*n)++;
    }

    fclose(fd);
}

int readBestTimer() {
    FILE *fd = fopen("assets/timers.txt", "r");
    if (!fd) {
        printf("Problème ouverture fichier timers.\n");
        return -1;
    }

    int bestTimer;
    int tempTimer;
    fscanf(fd, "%d", &bestTimer);

    while (fscanf(fd, "%d", &tempTimer) == 1) {
        if (tempTimer < bestTimer) bestTimer = tempTimer;
    }

    fclose(fd);

    return bestTimer;
}
