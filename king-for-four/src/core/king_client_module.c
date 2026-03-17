/**
 * @file king_client_module.c
 * 
 *  BOUCLE CLIENT :
 *  [INPUT] ----> Check Clic? ----> send_to_server(ACTION_PLAY)
 *     ^                                     |
 *     |                                     v
 *  [DRAW]  <---- local_state <---- [ON_DATA] (ACTION_SYNC)
 */

#include "APIs/module_interface.h"
#include "core/game.h"
#include "ui/renderer.h"
#include "rudp_core.h"
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Dépendances externes du lobby
extern int network_socket;
extern RUDP_Connection server_conn;

// Actions
#define ACTION_PLAY_CARD 0x10
#define ACTION_DRAW_CARD 0x11
#define ACTION_SYNC_GAME 0x12
#define ACTION_JOIN_GAME 0x13
#define ACTION_START_GAME 0x14
#define ACTION_SYNC_HAND 0x15
#define ACTION_JOIN_ACK 0x16

#pragma pack(push, 1)
typedef struct {
    int current_player;
    int active_color;
    Card top_card;
    int hand_sizes[4];
    int status; 
    int host_id;
} GameSyncPayload;
#pragma pack(pop)

static GameState local_state;
static GameAssets assets;
static bool assets_loaded = false;
static int my_internal_id = -1;
static int game_status = 0; 
static float join_retry_timer = 0;

static void send_to_server(uint8_t action, void* data, uint16_t len) {
    GameTLVHeader tlv = { .game_id = 1, .action = action, .length = len };
    RUDP_Header h;
    RUDP_GenerateHeader(&server_conn, 5, &h);
    
    uint8_t buffer[1024];
    memcpy(buffer, &h, sizeof(h));
    memcpy(buffer + sizeof(h), &tlv, sizeof(tlv));
    if (len > 0 && data) memcpy(buffer + sizeof(h) + sizeof(tlv), data, len);
    
    send(network_socket, buffer, sizeof(h) + sizeof(tlv) + len, 0);
}

void king_client_init(void) {
    if (!assets_loaded) {
        assets = LoadAssets();
        assets_loaded = true;
    }
    // Mise à zéro totale pour éviter les pointeurs fantômes
    memset(&local_state, 0, sizeof(GameState));
    init_game_logic(&local_state);
    my_internal_id = -1;
    game_status = 0;
    join_retry_timer = 0;
}

void king_client_on_data(int player_id, uint8_t action, void* data, uint16_t len) {
    (void)player_id;
    if (data == NULL) return;

    if (action == ACTION_JOIN_ACK) {
        if (len >= sizeof(int)) {
            memcpy(&my_internal_id, data, sizeof(int));
            printf("[KING CLIENT] Mon ID interne: %d\n", my_internal_id);
        }
    }
    else if (action == ACTION_SYNC_GAME) {
        if (len >= sizeof(GameSyncPayload)) {
            GameSyncPayload sync;
            memcpy(&sync, data, sizeof(GameSyncPayload));
            local_state.current_player = sync.current_player;
            local_state.active_color = sync.active_color;
            game_status = sync.status;
            
            if (local_state.discard_pile.head == NULL) {
                push_card(&local_state.discard_pile, sync.top_card);
            } else {
                local_state.discard_pile.head->card = sync.top_card;
            }
            
            for (int i = 0; i < 4; i++) {
                local_state.players[i].hand.size = sync.hand_sizes[i];
            }
        }
    }
    else if (action == ACTION_SYNC_HAND) {
        int count = len / sizeof(Card);
        uint8_t* ptr = (uint8_t*)data;
        
        // Sécurité : vider proprement la liste chaînée sans crash
        clear_deck(&local_state.players[0].hand);
        
        // On parcourt à l'envers pour que le push_card (qui ajoute en tête) 
        // recrée l'ordre exact du serveur.
        for (int i = count - 1; i >= 0; i--) {
            Card c;
            memcpy(&c, ptr + (i * sizeof(Card)), sizeof(Card));
            push_card(&local_state.players[0].hand, c);
        }
    }
}

void king_client_update(float dt) {
    if (!assets_loaded) return;
    if (my_internal_id == -1) {
        join_retry_timer += dt;
        if (join_retry_timer > 1.0f) {
            send_to_server(ACTION_JOIN_GAME, NULL, 0);
            join_retry_timer = 0;
        }
    }

    if (game_status == 1 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        int clickedHandIndex = GetHoveredCardIndex(&local_state.players[0], assets);
        if (clickedHandIndex != -1) {
            send_to_server(ACTION_PLAY_CARD, &clickedHandIndex, sizeof(int));
        } else {
            Rectangle deckRect = GetDeckRect(assets);
            if (CheckCollisionPointRec(GetMousePosition(), deckRect)) {
                send_to_server(ACTION_DRAW_CARD, NULL, 0);
            }
        }
    }

    if (game_status == 0 && IsKeyPressed(KEY_ENTER) && my_internal_id == 0) {
        send_to_server(ACTION_START_GAME, NULL, 0);
    }
    
    if (IsKeyPressed(KEY_ESCAPE)) {
        extern void switch_minigame(uint8_t game_id);
        switch_minigame(0);
    }
}

void king_client_draw(void) {
    if (!assets_loaded) return;
    ClearBackground((Color){0, 80, 0, 255});
    
    if (game_status == 0) {
        DrawText("KING FOR FOUR - SALLE D'ATTENTE", 100, 100, 40, GOLD);
        if (my_internal_id != -1) {
            DrawText(TextFormat("Vous êtes le JOUEUR %d", my_internal_id), 100, 180, 30, WHITE);
            if (my_internal_id == 0) {
                DrawText("VOUS ÊTES L'HÔTE. Appuyez sur ENTRÉE pour lancer.", 100, 240, 25, GREEN);
            } else {
                DrawText("En attente de l'hôte...", 100, 240, 30, LIGHTGRAY);
            }
        } else {
            DrawText("Connexion au serveur...", 100, 180, 30, GRAY);
        }
        return;
    }

    RenderTable(&local_state, assets);
    RenderHand(&local_state.players[0], assets);
    
    if (local_state.current_player == my_internal_id) {
        DrawText("C'EST VOTRE TOUR !", 10, 40, 25, GREEN);
    } else {
        DrawText(TextFormat("Tour du Joueur %d", local_state.current_player), 10, 40, 25, YELLOW);
    }
}

MiniGameModule KingForFourClientModule = {
    .id = 1,
    .name = "King For Four",
    .init = king_client_init,
    .on_data = king_client_on_data,
    .update = king_client_update,
    .draw = king_client_draw
};
