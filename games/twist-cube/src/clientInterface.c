/**
    @file clientInterface.c
    @author i-Charlys
    @date 2026-04-02
    @date 2026-04-14
    @brief Client-side implementation of the Twist Cube game interface managing local game state and server communication.
*/

#include "raylib.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "APIs/generalAPI.h"
#include "networkInterface.h"

/**
    @brief Definition of enum RubikActionCodes_e
*/
enum RubikActionCodes_e {
    ACTION_CODE_RUBIK_SCRAMBLE = firstAvailableActionCode + 0x20,
    ACTION_CODE_RUBIK_PROGRESS,
    ACTION_CODE_RUBIK_ELIMINATE
};

/**
    @brief Definition of typedef struct
*/
typedef struct {
    BaseGame_St base;
} RubikGame_St;

// We'll include the original main.c content by refactoring it later or just using local copies
// For now, let's define the necessary structures and functions based on the original main.c

typedef enum {
    COLOR_BLUE, COLOR_ORANGE, COLOR_GREEN, COLOR_RED, COLOR_WHITE, COLOR_YELLOW
} ColorElmt;

typedef struct {
    ColorElmt back[3][3];
    ColorElmt left[3][3];
    ColorElmt front[3][3];
    ColorElmt right[3][3];
    ColorElmt up[3][3];
    ColorElmt down[3][3];
} Cube;

extern s32 networkSocket;
extern RUDPConnection_St serverConnection;

static Cube my_cube;
static int myIdInternal = -1;
static bool eliminated = false;
static float solve_progress = 0.0f;
static bool game_started = false;
static double solve_start_time = 0;
static double current_solve_time = 0;
static bool is_solved = false;

// Extern functions from main.c (need to be exposed or moved)
extern void initCube(Cube *cube);
extern void mouvement(Cube *cube);
extern void display3D(Cube *cube, Camera3D camera);
extern bool isCubeSolve(Cube* cube);
extern void scrambleMoves(char *moves[20]);
extern void applyScrambleInstant(Cube *cube, char *moves[20]);

static Camera3D camera = {0};
static float angleX = 1.0f;
static float angleY = 0.5f;
static float radius = 8.0f;

void twistCube_init(void) {
    initCube(&my_cube);
    myIdInternal = -1;
    eliminated = false;
    solve_progress = 0.0f;
    game_started = false;
    solve_start_time = 0;
    current_solve_time = 0;
    is_solved = false;
    
    camera.position = (Vector3){6.0f, 6.0f, 6.0f};
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

void twistCube_onData(s32 playerId, u8 action, const void* data, u16 len) {
    if (action != ACTION_CODE_JOIN_ACK) {
        if (playerId < 0 || (playerId >= MAX_CLIENTS && playerId != 999)) {
            printf("[RUBIK] Data received from invalid player ID: %d\n", (int)playerId);
            return;
        }
    }
    if (data == NULL && action != ACTION_CODE_RUBIK_SCRAMBLE) return;

    if (action == ACTION_CODE_JOIN_ACK) {
        if (len >= sizeof(u16)) {
            u16 netId;
            memcpy(&netId, data, sizeof(u16));
            myIdInternal = (int)ntohs(netId);
        }
    }
    else if (action == ACTION_CODE_RUBIK_SCRAMBLE) {
        u32 seed;
        memcpy(&seed, data, sizeof(u32));
        srand(ntohl(seed));
        initCube(&my_cube);
        char *moves[20];
        scrambleMoves(moves);
        applyScrambleInstant(&my_cube, moves);
        game_started = true;
        eliminated = false;
        is_solved = false;
        solve_start_time = GetTime();
        printf("[RUBIK] Rubik's scramble started with seed %u\n", ntohl(seed));
    }
    else if (action == ACTION_CODE_RUBIK_ELIMINATE) {
        int targetId;
        memcpy(&targetId, data, sizeof(int));
        if ((int)ntohl(targetId) == myIdInternal) {
            eliminated = true;
        }
    }
}

static float calculate_progress(Cube* c) {
    if (isCubeSolve(c)) return 100.0f;
    
    int correct = 0;
    // Count matches for each face
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (c->front[i][j] == c->front[1][1]) correct++;
            if (c->back[i][j] == c->back[1][1]) correct++;
            if (c->left[i][j] == c->left[1][1]) correct++;
            if (c->right[i][j] == c->right[1][1]) correct++;
            if (c->up[i][j] == c->up[1][1]) correct++;
            if (c->down[i][j] == c->down[1][1]) correct++;
        }
    }
    
    // 54 stickers total (6 faces * 9). Centers always match (6 centers).
    // Progress = (correct - 6) / (54 - 6) * 100
    float progress = ((float)correct - 6.0f) / 48.0f * 100.0f;
    if (progress < 0) progress = 0;
    return progress;
}

void twistCube_update(float dt) {
    if (myIdInternal == -1) {
        if (networkSocket < 0) {
            // Solo mode: no server, auto-assign ID so we can play immediately
            myIdInternal = 0;
        } else {
            static float join_timer = 0;
            join_timer += dt;
            if (join_timer > 1.0f) {
                GameTLVHeader_St tlv = { .gameId = MINI_GAME_ID_TWIST_CUBE, .action = ACTION_CODE_JOIN_GAME, .length = 0 };
                RUDPHeader_St h;
                rudpGenerateHeader(&serverConnection, ACTION_CODE_GAME_DATA, &h);
                h.senderId = htons(0);
                u8 buf[64];
                memcpy(buf, &h, sizeof(h));
                memcpy(buf + sizeof(h), &tlv, sizeof(tlv));
                send(networkSocket, buf, sizeof(h) + sizeof(tlv), 0);
                join_timer = 0;
            }
            return;
        }
    }

    if (eliminated) return;

    // Movement and camera
    extern void UpdateCameraOrbit(Camera3D *camera, Vector3 target, float radius, float *angleX, float *angleY);
    UpdateCameraOrbit(&camera, (Vector3){0,0,0}, radius, &angleX, &angleY);
    
    if (game_started) {
        if (!is_solved) {
            mouvement(&my_cube);
            current_solve_time = GetTime() - solve_start_time;
            
            if (isCubeSolve(&my_cube)) {
                is_solved = true;
                solve_progress = 100.0f;
            } else {
                solve_progress = calculate_progress(&my_cube);
            }
        }
        
        // Always sync progress periodically
        static float sync_timer = 0;
        sync_timer += dt;
        if (sync_timer > 0.5f) {
            GameTLVHeader_St tlv = { .gameId = MINI_GAME_ID_TWIST_CUBE, .action = ACTION_CODE_RUBIK_PROGRESS, .length = htons(sizeof(float)) };
            RUDPHeader_St h;
            rudpGenerateHeader(&serverConnection, ACTION_CODE_GAME_DATA, &h);
            h.senderId = htons((u16)(myIdInternal != -1 ? myIdInternal : 0));
            u8 buf[128];
            memset(buf, 0, sizeof(buf));
            memcpy(buf, &h, sizeof(h));
            memcpy(buf + sizeof(h), &tlv, sizeof(tlv));
            memcpy(buf + sizeof(h) + sizeof(tlv), &solve_progress, sizeof(float));
            send(networkSocket, buf, sizeof(h) + sizeof(tlv) + sizeof(float), 0);
            sync_timer = 0;
        }
    } else {
        // Wait for start
        if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_SEMICOLON)) {
            if (networkSocket < 0) {
                // Solo mode: start directly, scramble cube locally
                initCube(&my_cube);
                char* moves[20];
                scrambleMoves(moves);
                applyScrambleInstant(&my_cube, moves);
                game_started = true;
                is_solved = false;
                solve_start_time = GetTime();
            } else {
                GameTLVHeader_St tlv = { .gameId = MINI_GAME_ID_TWIST_CUBE, .action = ACTION_CODE_START_GAME, .length = 0 };
                RUDPHeader_St h;
                rudpGenerateHeader(&serverConnection, ACTION_CODE_GAME_DATA, &h);
                h.senderId = htons((u16)myIdInternal);
                u8 buf[128];
                memset(buf, 0, sizeof(buf));
                memcpy(buf, &h, sizeof(h));
                memcpy(buf + sizeof(h), &tlv, sizeof(tlv));
                send(networkSocket, buf, sizeof(h) + sizeof(tlv), 0);
            }
        }
    }

    // Keyboard Shortcuts 
    // removed local ESC handler to use lobby pause menu instead
}

void twistCube_draw(void) {
    if (eliminated) {
        DrawText("ELIMINÉ !", GetScreenWidth()/2 - 100, GetScreenHeight()/2, 40, RED);
        return;
    }
    
    display3D(&my_cube, camera);
    
    if (!game_started) {
        DrawText("RUBIK BATTLE ROYALE", 10, 10, 30, GOLD);
        if (myIdInternal != -1) DrawText("Appuyez sur M pour lancer", 10, 50, 20, GREEN);
        else DrawText("En attente de l'hôte...", 10, 50, 20, LIGHTGRAY);
    } else {
        DrawText(TextFormat("PROGRÈS: %.1f%%", solve_progress), 10, 10, 20, SKYBLUE);
        int min = (int)current_solve_time / 60;
        int sec = (int)current_solve_time % 60;
        DrawText(TextFormat("TEMPS: %02d:%02d", min, sec), 10, 40, 20, WHITE);
        if (is_solved) {
            DrawText("RÉSOLU !", GetScreenWidth()/2 - 80, 100, 40, LIME);
        }
    }
    DrawText("ESC pour quitter", GetScreenWidth() - 150, 10, 15, GRAY);
}

GameClientInterface_St twistCubeClientInterface = {
    .id = MINI_GAME_ID_TWIST_CUBE,
    .name = "Twist Cube",
    .init = twistCube_init,
    .onData = twistCube_onData,
    .update = twistCube_update,
    .draw = twistCube_draw
};