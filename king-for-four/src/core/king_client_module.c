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

// Actions (DOIVENT CORRESPONDRE AU SERVEUR)
#define ACTION_PLAY_CARD 0x10
#define ACTION_DRAW_CARD 0x11
#define ACTION_SYNC_GAME 0x12
#define ACTION_JOIN_GAME 0x13
#define ACTION_START_GAME 0x14
#define ACTION_SYNC_HAND 0x15
#define ACTION_JOIN_ACK 0x16

#define LOBBY_SWITCH_GAME 0x20

#pragma pack(push, 1)
typedef struct {
    int current_player;
    int active_color;
    Card top_card;
    int hand_sizes[4];
    int status; // 0: WAITING, 1: PLAYING
    int host_id;
} GameSyncPayload;
#pragma pack(pop)

// État local du module client
static GameState local_state;
static GameAssets assets;
static bool assets_loaded = false;
static int my_internal_id = -1;
static int game_status = 0; // 0: WAITING
static int game_host_id = -1;
static int my_player_id_network = -1;
static float join_retry_timer = 0;

static void send_to_server(uint8_t action, void* data, uint16_t len) {
    GameTLVHeader tlv = { .game_id = 1, .action = action, .length = len };
    RUDP_Header h;
    RUDP_GenerateHeader(&server_conn, 5 /* ACTION_GAME_DATA */, &h);
    
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
    init_game_logic(&local_state);
    my_internal_id = -1;
    game_status = 0;
    join_retry_timer = 0;
    // On ne l'envoie plus ici, on le fera dans update
}

void king_client_on_data(int player_id, uint8_t action, void* data, uint16_t len) {
    if (action == ACTION_JOIN_ACK) {
        my_internal_id = *(int*)data;
        my_player_id_network = player_id;
        printf("[KING CLIENT] Reçu JOIN_ACK, mon ID interne est %d\n", my_internal_id);
    }
    else if (action == ACTION_SYNC_GAME) {
        GameSyncPayload* sync = (GameSyncPayload*)data;
        local_state.current_player = sync->current_player;
        local_state.active_color = sync->active_color;
        local_state.num_players = 4;
        game_status = sync->status;
        game_host_id = sync->host_id;
        
        if (local_state.discard_pile.head == NULL) {
            push_card(&local_state.discard_pile, sync->top_card);
        } else {
            local_state.discard_pile.head->card = sync->top_card;
        }
        
        for (int i = 0; i < 4; i++) {
            local_state.players[i].hand.size = sync->hand_sizes[i];
        }
    }
    else if (action == ACTION_SYNC_HAND) {
        int count = len / sizeof(Card);
        Card* cards = (Card*)data;
        while(local_state.players[0].hand.size > 0) pop_card(&local_state.players[0].hand);
        for (int i = 0; i < count; i++) push_card(&local_state.players[0].hand, cards[i]);
    }
}

void king_client_update(float dt) {
    // TENTATIVE DE CONNEXION RÉPÉTÉE
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

    if (game_status == 0 && IsKeyPressed(KEY_ENTER)) {
        send_to_server(ACTION_START_GAME, NULL, 0);
    }
    
    if (IsKeyPressed(KEY_ESCAPE)) {
        extern void switch_minigame(uint8_t game_id);
        switch_minigame(0);
    }
}

void king_client_draw(void) {
    ClearBackground((Color){0, 80, 0, 255});
    
    if (game_status == 0) {
        DrawText("KING FOR FOUR - SALLE D'ATTENTE", 100, 100, 40, GOLD);
        if (my_internal_id != -1) {
            DrawText(TextFormat("Vous êtes le JOUEUR %d", my_internal_id), 100, 180, 30, WHITE);
            if (my_internal_id == 0) {
                DrawText("VOUS ÊTES L'HÔTE", 100, 220, 25, GREEN);
                DrawText("Appuyez sur ENTRÉE pour lancer la partie", 100, 280, 30, RAYWHITE);
            } else {
                DrawText("PARTIE NON DÉMARRÉE", 100, 220, 25, ORANGE);
                DrawText("En attente de l'hôte...", 100, 280, 30, LIGHTGRAY);
            }
        } else {
            DrawText("Connexion au serveur en cours...", 100, 180, 30, GRAY);
            DrawText("(Assurez-vous que le serveur est lancé)", 100, 220, 20, DARKGRAY);
        }
        DrawText("ECHAP pour quitter", 10, 10, 20, RAYWHITE);
        return;
    }

    RenderTable(&local_state, assets);
    RenderHand(&local_state.players[0], assets);
    DrawText("ECHAP pour quitter", 10, 10, 20, RAYWHITE);
    
    if (local_state.current_player != -1) {
        if (local_state.current_player == my_internal_id) {
            DrawRectangle(0, 40, 300, 40, (Color){0, 200, 0, 100});
            DrawText("C'EST VOTRE TOUR !", 10, 45, 25, WHITE);
        } else {
            DrawText(TextFormat("Tour du Joueur %d", local_state.current_player), 10, 45, 25, YELLOW);
        }
    }
    
    int y_pos = 100;
    for (int i = 0; i < 4; i++) {
        int count = local_state.players[i].hand.size;
        if (count > 0) {
            Color c = (i == local_state.current_player) ? YELLOW : LIGHTGRAY;
            DrawText(TextFormat("Joueur %d: %d cartes", i, count), 10, y_pos, 20, c);
            y_pos += 25;
        }
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
