#include "rubikAPI.h"
#include "networkInterface.h"
#include "rudp_core.h"
#include "raylib.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
static int my_id_internal = -1;
static bool eliminated = false;
static float solve_progress = 0.0f;
static bool game_started = false;

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

void rubik_client_init(void) {
    initCube(&my_cube);
    my_id_internal = -1;
    eliminated = false;
    solve_progress = 0.0f;
    game_started = false;
    
    camera.position = (Vector3){6.0f, 6.0f, 6.0f};
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

void rubik_client_on_data(s32 player_id, u8 action, const void* data, u16 len) {
    if (action != ACTION_CODE_JOIN_ACK) {
        if (player_id < 0 || player_id >= MAX_CLIENTS) {
            printf("[RUBIK] Data received from invalid player ID: %d\n", (int)player_id);
            return;
        }
    }
    if (data == NULL && action != ACTION_CODE_RUBIK_SCRAMBLE) return;

    if (action == ACTION_CODE_JOIN_ACK) {
        if (len >= sizeof(u16)) {
            u16 net_id;
            memcpy(&net_id, data, sizeof(u16));
            my_id_internal = (int)ntohs(net_id);
        }
    }
    else if (action == ACTION_CODE_RUBIK_SCRAMBLE) {
        int seed;
        memcpy(&seed, data, sizeof(int));
        srand(ntohl(seed));
        initCube(&my_cube);
        char *moves[20];
        scrambleMoves(moves);
        applyScrambleInstant(&my_cube, moves);
        game_started = true;
        eliminated = false;
    }
    else if (action == ACTION_CODE_RUBIK_ELIMINATE) {
        int target_id;
        memcpy(&target_id, data, sizeof(int));
        if ((int)ntohl(target_id) == my_id_internal) {
            eliminated = true;
        }
    }
}

static float calculate_progress(Cube* c) {
    // int correct = 0;
    // Count stickers matching their center
    // ColorElmt centers[6] = {c->back[1][1], c->left[1][1], c->front[1][1], c->right[1][1], c->up[1][1], c->down[1][1]};
    // ... simplified count ...
    if (isCubeSolve(c)) return 100.0f;
    return 0.0f; // TODO: Real progress calculation
}

void rubik_client_update(float dt) {
    if (my_id_internal == -1) {
        if (networkSocket < 0) {
            // Solo mode: no server, auto-assign ID so we can play immediately
            my_id_internal = 0;
        } else {
            static float join_timer = 0;
            join_timer += dt;
            if (join_timer > 1.0f) {
                GameTLVHeader_St tlv = { .game_id = MINI_GAME_CUBE, .action = ACTION_CODE_JOIN_GAME, .length = 0 };
                RUDPHeader_St h;
                rudpGenerateHeader(&serverConnection, ACTION_CODE_GAME_DATA, &h);
                h.sender_id = htons(0);
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
        mouvement(&my_cube);
        
        float current_progress = calculate_progress(&my_cube);
        if (current_progress != solve_progress) {
            solve_progress = current_progress;
            GameTLVHeader_St tlv = { .game_id = MINI_GAME_CUBE, .action = ACTION_CODE_RUBIK_PROGRESS, .length = htons(sizeof(float)) };
            RUDPHeader_St h;
            rudpGenerateHeader(&serverConnection, ACTION_CODE_GAME_DATA, &h);
            h.sender_id = htons((u16)(my_id_internal != -1 ? my_id_internal : 0));
            u8 buf[128];
            memset(buf, 0, sizeof(buf));
            memcpy(buf, &h, sizeof(h));
            memcpy(buf + sizeof(h), &tlv, sizeof(tlv));
            memcpy(buf + sizeof(h) + sizeof(tlv), &solve_progress, sizeof(float));
            send(networkSocket, buf, sizeof(h) + sizeof(tlv) + sizeof(float), 0);
        }
    } else {
        // Wait for start
        if (IsKeyPressed(KEY_ENTER)) {
            if (networkSocket < 0) {
                // Solo mode: start directly, scramble cube locally
                char* moves[20];
                scrambleMoves(moves);
                applyScrambleInstant(&my_cube, moves);
                game_started = true;
            } else {
                GameTLVHeader_St tlv = { .game_id = MINI_GAME_CUBE, .action = ACTION_CODE_START_GAME, .length = 0 };
                RUDPHeader_St h;
                rudpGenerateHeader(&serverConnection, ACTION_CODE_GAME_DATA, &h);
                h.sender_id = htons((u16)my_id_internal);
                u8 buf[128];
                memset(buf, 0, sizeof(buf));
                memcpy(buf, &h, sizeof(h));
                memcpy(buf + sizeof(h), &tlv, sizeof(tlv));
                send(networkSocket, buf, sizeof(h) + sizeof(tlv), 0);
            }
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        GameTLVHeader_St tlv = { .game_id = MINI_GAME_CUBE, .action = ACTION_CODE_QUIT_GAME, .length = 0 };
        RUDPHeader_St h;
        rudpGenerateHeader(&serverConnection, ACTION_CODE_GAME_DATA, &h);
        h.sender_id = htons((u16)(my_id_internal != -1 ? my_id_internal : 0));
        u8 buf[64];
        memcpy(buf, &h, sizeof(h));
        memcpy(buf + sizeof(h), &tlv, sizeof(tlv));
        send(networkSocket, buf, sizeof(h) + sizeof(tlv), 0);
        
        extern void switch_minigame(u8 game_id);
        switch_minigame(0);
    }
}

void rubik_client_draw(void) {
    if (eliminated) {
        DrawText("ELIMINÉ !", GetScreenWidth()/2 - 100, GetScreenHeight()/2, 40, RED);
        return;
    }
    
    display3D(&my_cube, camera);
    
    if (!game_started) {
        DrawText("RUBIK BATTLE ROYALE", 10, 10, 30, GOLD);
        if (my_id_internal != -1) DrawText("Appuyez sur ENTRÉE pour lancer", 10, 50, 20, GREEN);
        else DrawText("En attente de l'hôte...", 10, 50, 20, LIGHTGRAY);
    } else {
        DrawText(TextFormat("PROGRÈS: %.1f%%", solve_progress), 10, 10, 20, SKYBLUE);
    }
    DrawText("ESC pour quitter", GetScreenWidth() - 150, 10, 15, GRAY);
}

GameClientInterface_St RubikClientModule = {
    .id = MINI_GAME_CUBE,
    .name = "RubiksCube",
    .init = rubik_client_init,
    .on_data = rubik_client_on_data,
    .update = rubik_client_update,
    .draw = rubik_client_draw
};

Error_Et rubik_initGame(RubikGame_St** game) {
    *game = malloc(sizeof(RubikGame_St));
    if (!*game) return ERROR_ALLOC;
    (*game)->base.running = true;
    (*game)->base.paused = false;
    (*game)->base.freeGame = (FreeGame_Ft)rubik_freeGame;
    return OK;
}

Error_Et rubik_freeGame(RubikGame_St** game) {
    if (game && *game) {
        free(*game);
        *game = NULL;
    }
    return OK;
}
