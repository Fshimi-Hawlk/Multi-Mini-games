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

#define LOBBY_SWITCH_GAME 0x20

#pragma pack(push, 1)
typedef struct {
    int current_player;
    int active_color;
    Card top_card;
    int hand_sizes[4];
} GameSyncPayload;
#pragma pack(pop)

// État local du module client
static GameState local_state;
static GameAssets assets;
static bool assets_loaded = false;

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
    
    // On notifie le serveur qu'on rejoint King for Four
    send_to_server(ACTION_JOIN_GAME, NULL, 0);
}

void king_client_on_data(int player_id, uint8_t action, void* data, uint16_t len) {
    if (action == ACTION_SYNC_GAME) {
        GameSyncPayload* sync = (GameSyncPayload*)data;
        local_state.current_player = sync->current_player;
        local_state.active_color = sync->active_color;
        local_state.num_players = 4; // On assume 4 pour le rendu des scores
        
        // Mise à jour du talon
        if (local_state.discard_pile.head == NULL) {
            push_card(&local_state.discard_pile, sync->top_card);
        } else {
            local_state.discard_pile.head->card = sync->top_card;
        }
        
        // Mise à jour des tailles de main
        for (int i = 0; i < 4; i++) {
            local_state.players[i].hand.size = sync->hand_sizes[i];
        }
    }
    else if (action == ACTION_SYNC_HAND) {
        int count = len / sizeof(Card);
        Card* cards = (Card*)data;
        
        // On vide la main locale avant de la remplir
        while(local_state.players[0].hand.size > 0) {
            pop_card(&local_state.players[0].hand);
        }
        
        for (int i = 0; i < count; i++) {
            push_card(&local_state.players[0].hand, cards[i]);
        }
    }
}

void king_client_update(float dt) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        // 1. Clic sur sa propre main (toujours players[0] localement pour RenderHand)
        int clickedHandIndex = GetHoveredCardIndex(&local_state.players[0], assets);
        if (clickedHandIndex != -1) {
            send_to_server(ACTION_PLAY_CARD, &clickedHandIndex, sizeof(int));
        }
        else {
            // 2. Clic sur la pioche
            Rectangle deckRect = GetDeckRect(assets);
            if (CheckCollisionPointRec(GetMousePosition(), deckRect)) {
                send_to_server(ACTION_DRAW_CARD, NULL, 0);
            }
        }
    }

    if (IsKeyPressed(KEY_ENTER)) {
        send_to_server(ACTION_START_GAME, NULL, 0);
    }
    
    // Retour au lobby local
    if (IsKeyPressed(KEY_ESCAPE)) {
        // On pourrait envoyer une action de départ au serveur ici
        extern void switch_minigame(uint8_t game_id);
        switch_minigame(0);
    }
}

void king_client_draw(void) {
    ClearBackground((Color){0, 80, 0, 255}); // Vert un peu plus foncé
    
    RenderTable(&local_state, assets);
    RenderHand(&local_state.players[0], assets);
    
    DrawText("Appuyez sur ENTRÉE pour démarrer (Hôte seulement)", 10, 10, 20, RAYWHITE);
    DrawText("ECHAP pour quitter", 10, 35, 20, RAYWHITE);
    
    if (local_state.current_player != -1) {
        DrawText(TextFormat("Tour du joueur: %d", local_state.current_player), 10, 60, 20, YELLOW);
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
