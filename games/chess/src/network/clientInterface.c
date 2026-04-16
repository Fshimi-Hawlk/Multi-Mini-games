/**
    @file clientInterface.c
    @author i-Charlys
    @date 2026-04-02
    @date 2026-04-14
    @brief Client-side network interface for Chess.
*/

#include "chessAPI.h"
#include "networkInterface.h"
#include "globals.h"
#include "game.h"
#include "event.h"
#include "rendering.h"
#include "ai.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern s32 networkSocket;
extern RUDPConnection_St serverConnection;

/**
    @brief Local copy of the chess board for the client.
*/
static Board_t current_board;

/**
    @brief The player's assigned color (0: White, 1: Black, -1: Spectator).
*/
static s32 my_color = -1; 

/**
    @brief Flag indicating if the network game has started.
*/
static bool game_started = false;

/**
    @brief Current game status (0: WAITING, 1: PLAYING).
*/
static s32 game_status = 0; 

/**
    @brief Selected bot difficulty level (0: None, 1: Easy, 2: Normal, 3: Hard).
*/
static s32 selected_bot_level = 0; 

/**
    @brief The player's internal network ID.
*/
static s32 my_id_internal = -1;

#pragma pack(push, 1)
/**
    @brief Network payload for a chess move.
*/
typedef struct {
    u8 from_x, from_y;  ///< Starting coordinates
    u8 to_x, to_y;      ///< Ending coordinates
    u8 promotion;       ///< Promotion choice (0: None, 1: Queen, 2: Rook, 3: Bishop, 4: Knight)
} ChessMovePayload_St;
#pragma pack(pop)

extern void chess_initAudio(void);

/**
    @brief Initialize the chess client module.
*/
void chess_init(void) {
    chess_initAudio();
    initTextures();
    initPlayers();
    initBoard(current_board);
    resetGame();
    if (moveMade == NULL) {
        moveMade = calloc(12, sizeof(char));
    }
    my_color = -1;
    my_id_internal = -1;
    game_status = 0;
    selected_bot_level = 0;
    game_started = false;
}

/**
    @brief Callback function called when data is received from the server.
    @param[in] player_id ID of the player who sent the data
    @param[in] action    Action code associated with the data
    @param[in] data      Pointer to the received data
    @param[in] len       Length of the received data
*/
void chess_onData(s32 player_id, u8 action, const void* data, u16 len) {
    if (action != ACTION_CODE_JOIN_ACK) {
        if (player_id < 0 || (player_id >= MAX_CLIENTS && player_id != 999)) {
            printf("[CHESS] Data received from invalid player ID: %d\n", (int)player_id);
            return;
        }
    }
    if (data == NULL) return;

    if (action == ACTION_CODE_JOIN_ACK) {
        if (len >= sizeof(u16)) {
            u16 net_id;
            memcpy(&net_id, data, sizeof(u16));
            my_id_internal = (s32)ntohs(net_id);
            my_color = (my_id_internal == 0) ? 0 : 1; // 0: White, 1: Black
            printf("[CHESS] Assigned internal ID: %d, color: %s\n", (int)my_id_internal, my_color == 0 ? "White" : "Black");
        }
    }
    else if (action == ACTION_CODE_START_GAME) {
        if (len >= sizeof(s32)) {
            s32 net_level;
            memcpy(&net_level, data, sizeof(s32));
            selected_bot_level = (s32)ntohl((uint32_t)net_level);
        }
        game_status = 1;
        game_started = true;
        printf("[CHESS] Game started with bot level %d!\n", (int)selected_bot_level);
    }
    else if (action == ACTION_CODE_CHESS_SYNC) {
        // Full board sync
    }
    else if (action == ACTION_CODE_CHESS_MOVE) {
        if (len >= sizeof(ChessMovePayload_St)) {
            ChessMovePayload_St move;
            memcpy(&move, data, sizeof(ChessMovePayload_St));
            // Apply move to local board
            Piece_st* p = current_board[move.from_y][move.from_x];
            if (p) {
                Piece_st* captured = current_board[move.to_y][move.to_x];
                if (captured) captured->isTaken = true;
                current_board[move.to_y][move.to_x] = p;
                current_board[move.from_y][move.from_x] = NULL;
                p->pos.x = move.to_x;
                p->pos.y = move.to_y;
                playerTurn = !playerTurn;
            }
        }
    }
}

/**
    @brief Update the chess client state.
    @param[in] dt Delta time since the last update
*/
void chess_update(float dt) {
    // Solo mode support: if not connected, assign local ID and color
    if (my_id_internal == -1 && networkSocket < 0) {
        my_id_internal = 0;
        my_color = 0; // Local player plays White
        printf("[CHESS] Solo mode: Assigned local ID 0, color White\n");
    }

    if (my_id_internal == -1) {
        // Send join request if not already done
        static f32 join_timer = 0;
        join_timer += dt;
        if (join_timer > 1.0f) {
            GameTLVHeader_St tlv = { .gameId = MINI_GAME_ID_CHESS, .action = ACTION_CODE_JOIN_GAME, .length = 0 };
            RUDPHeader_St h;
            rudpGenerateHeader(&serverConnection, ACTION_CODE_GAME_DATA, &h);
            h.senderId = htons((u16)(my_id_internal != -1 ? my_id_internal : 0));
            u8 buf[128];
            memset(buf, 0, sizeof(buf));
            u8* ptr = buf;
            memcpy(ptr, &h, sizeof(h)); ptr += sizeof(h);
            memcpy(ptr, &tlv, sizeof(tlv)); ptr += sizeof(tlv);
            send(networkSocket, buf, (size_t)(ptr - buf), 0);
            join_timer = 0;
        }
        return;
    }

    if (game_status == 0) {
        if (my_id_internal == 0) { // Host
            if (IsKeyPressed(KEY_RIGHT)) selected_bot_level = (selected_bot_level + 1) % 4;
            if (IsKeyPressed(KEY_LEFT)) selected_bot_level = (selected_bot_level + 3) % 4;
            
            if (IsKeyPressed(KEY_ENTER)) {
                printf("[CHESS] Host sending START_GAME...\n");
                GameTLVHeader_St tlv = { .gameId = MINI_GAME_ID_CHESS, .action = ACTION_CODE_START_GAME, .length = htons(sizeof(s32)) };
                RUDPHeader_St h;
                rudpGenerateHeader(&serverConnection, ACTION_CODE_GAME_DATA, &h);
                h.senderId = htons((u16)(my_id_internal != -1 ? my_id_internal : 0));
                u8 buf[128];
                memset(buf, 0, sizeof(buf));
                u8* ptr = buf;
                memcpy(ptr, &h, sizeof(h)); ptr += sizeof(h);
                memcpy(ptr, &tlv, sizeof(tlv)); ptr += sizeof(tlv);
                s32 net_level = (s32)htonl((uint32_t)selected_bot_level);
                memcpy(ptr, &net_level, sizeof(s32)); ptr += sizeof(s32);
                send(networkSocket, buf, (size_t)(ptr - buf), 0);
            }
        }
    } else {
        // Playing
        if (playerTurn == my_color) {
            handleEvents(current_board);
            if (saveMove) {
                ChessMovePayload_St payload = {
                    .from_x = (u8)previousMoveCell[0].x, .from_y = (u8)previousMoveCell[0].y,
                    .to_x = (u8)previousMoveCell[1].x, .to_y = (u8)previousMoveCell[1].y,
                    .promotion = 0
                };
                GameTLVHeader_St tlv = { .gameId = MINI_GAME_ID_CHESS, .action = ACTION_CODE_CHESS_MOVE, .length = htons(sizeof(payload)) };
                RUDPHeader_St h;
                rudpGenerateHeader(&serverConnection, ACTION_CODE_GAME_DATA, &h);
                h.senderId = htons((u16)(my_id_internal != -1 ? my_id_internal : 0));
                u8 buf[128];
                memset(buf, 0, sizeof(buf));
                u8* ptr = buf;
                memcpy(ptr, &h, sizeof(h)); ptr += sizeof(h);
                memcpy(ptr, &tlv, sizeof(tlv)); ptr += sizeof(tlv);
                memcpy(ptr, &payload, sizeof(payload)); ptr += sizeof(payload);
                send(networkSocket, buf, (size_t)(ptr - buf), 0);
                saveMove = false;
            }
        } else if (my_id_internal == 0 && selected_bot_level > 0) {
            // Host plays for the bot
            static f32 bot_delay = 0;
            bot_delay += dt;
            if (bot_delay >= 1.0f && whitePlayer != NULL && blackPlayer != NULL) {
                u8 depth = (u8)((selected_bot_level == 1) ? 2 : (selected_bot_level == 2 ? 3 : 3)); // Max depth 3
                ChessMove_st best = ai_getBestMove(current_board, whitePlayer, blackPlayer, playerTurn, depth);
                if (best.from.x != -1) {
                    ChessMovePayload_St payload = {
                        .from_x = (u8)best.from.x, .from_y = (u8)best.from.y,
                        .to_x = (u8)best.to.x, .to_y = (u8)best.to.y,
                        .promotion = 0
                    };
                    
                    // Apply locally
                    Piece_st* p = current_board[payload.from_y][payload.from_x];
                    if (p) {
                        Piece_st* captured = current_board[payload.to_y][payload.to_x];
                        if (captured) captured->isTaken = true;
                        current_board[payload.to_y][payload.to_x] = p;
                        current_board[payload.from_y][payload.from_x] = NULL;
                        p->pos.x = payload.to_x;
                        p->pos.y = payload.to_y;
                        playerTurn = !playerTurn;
                        
                        // Broadcast to others
                        GameTLVHeader_St tlv = { .gameId = MINI_GAME_ID_CHESS, .action = ACTION_CODE_CHESS_MOVE, .length = htons(sizeof(payload)) };
                        RUDPHeader_St h;
                        rudpGenerateHeader(&serverConnection, ACTION_CODE_GAME_DATA, &h);
                        h.senderId = htons((u16)(my_id_internal != -1 ? my_id_internal : 0));
                        u8 buf[128];
                        memset(buf, 0, sizeof(buf));
                        u8* ptr = buf;
                        memcpy(ptr, &h, sizeof(h)); ptr += sizeof(h);
                        memcpy(ptr, &tlv, sizeof(tlv)); ptr += sizeof(tlv);
                        memcpy(ptr, &payload, sizeof(payload)); ptr += sizeof(payload);
                        send(networkSocket, buf, (size_t)(ptr - buf), 0);
                    }
                }
                bot_delay = 0;
            }
        }
    }

    // removed local ESC handler to use lobby pause menu instead
}

/**
    @brief Draw the chess client UI.
*/
void chess_draw(void) {
    if (game_status == 0) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));
        DrawText("ÉCHECS - SALLE D'ATTENTE", GetScreenWidth()/2 - MeasureText("ÉCHECS - SALLE D'ATTENTE", 40)/2, 100, 40, GOLD);
        
        if (my_id_internal != -1) {
            DrawText(TextFormat("Vous êtes le JOUEUR %d (%s)", (int)my_id_internal, my_color == 0 ? "BLANCS" : "NOIRS"), 150, 200, 30, WHITE);
            if (my_id_internal == 0) {
                const char* bot_names[] = {"AUCUN (2 Joueurs)", "BOT FACILE", "BOT NORMAL", "BOT DIFFICILE"};
                DrawText(TextFormat("OPPOSANT: < %s >", bot_names[selected_bot_level]), 150, 300, 30, GREEN);
                DrawText("Flèches GAUCHE/DROITE pour changer", 150, 340, 20, GRAY);
                DrawText("Appuyez sur ENTRÉE pour lancer la partie", 150, 450, 30, GOLD);
            } else {
                DrawText("En attente de l'hôte pour lancer...", 150, 300, 30, LIGHTGRAY);
            }
        } else {
            DrawText("Connexion au serveur...", 150, 200, 30, GRAY);
        }
    } else {
        renderFrame(current_board);
        DrawText(TextFormat("Vous jouez les %s", my_color == 0 ? "BLANCS" : "NOIRS"), 10, 10, 20, GREEN);
    }
    DrawText("ESC pour quitter", GetScreenWidth() - 150, 10, 15, DARKGRAY);
}

/**
    @brief Chess client module interface definition.
*/
GameClientInterface_St chess_clientInterface = {
    .id = MINI_GAME_ID_CHESS,
    .name = "Echecs",
    .init = chess_init,
    .onData = chess_onData,
    .update = chess_update,
    .draw = chess_draw
};