/**
 * @file king_client_module.c
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Client-side module for the King-for-Four game, handling network synchronization and UI.
 */

#include "APIs/module_interface.h"
#include "core/game.h"
#include "ui/renderer.h"
#include "rudp_core.h"
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/** @brief External reference to the network socket. */
extern int network_socket;
/** @brief External reference to the server connection. */
extern RUDP_Connection server_conn;

/** @brief Action code for playing a card. */
#define ACTION_PLAY_CARD 0x10
/** @brief Action code for drawing a card. */
#define ACTION_DRAW_CARD 0x11
/** @brief Action code for synchronizing the game state. */
#define ACTION_SYNC_GAME 0x12
/** @brief Action code for joining a game. */
#define ACTION_JOIN_GAME 0x13
/** @brief Action code for starting a game. */
#define ACTION_START_GAME 0x14
/** @brief Action code for synchronizing a player's hand. */
#define ACTION_SYNC_HAND 0x15
/** @brief Action code for acknowledging a join request. */
#define ACTION_JOIN_ACK 0x16

#pragma pack(push, 1)
/**
 * @struct GameSyncPayload
 * @brief Payload for synchronizing game state from server to client.
 */
typedef struct {
    int current_player;     /**< Index of current player */
    int active_color;       /**< Current active color */
    Card top_card;          /**< Card on top of discard pile */
    int hand_sizes[4];      /**< Card count for each player */
    int status;             /**< Game status (0: WAITING, 1: PLAYING) */
    int host_id;            /**< ID of host player */
} GameSyncPayload;
#pragma pack(pop)

/** @brief Local copy of the game state. */
static GameState local_state;
/** @brief Graphical assets. */
static GameAssets assets;
/** @brief Flag indicating if assets are loaded. */
static bool assets_loaded = false;
/** @brief This client's internal player ID assigned by server. */
static int my_internal_id = -1;
/** @brief Current status of the game. */
static int game_status = 0; 
/** @brief Timer for retrying to join the game. */
static float join_retry_timer = 0;

/**
 * @brief Sends a game action to the server.
 * @param action Action code.
 * @param data Pointer to payload data.
 * @param len Length of payload.
 */
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

/**
 * @brief Initializes the client module and loads assets.
 */
void king_client_init(void) {
    if (!assets_loaded) {
        assets = LoadAssets();
        assets_loaded = true;
    }
    memset(&local_state, 0, sizeof(GameState));
    init_game_logic(&local_state);
    my_internal_id = -1;
    game_status = 0;
    join_retry_timer = 0;
}

/**
 * @brief Callback for processing data received from the server.
 * @param player_id ID of the sender.
 * @param action Action code.
 * @param data Payload data.
 * @param len Payload length.
 */
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
        
        clear_deck(&local_state.players[0].hand);
        
        for (int i = count - 1; i >= 0; i--) {
            Card c;
            memcpy(&c, ptr + (i * sizeof(Card)), sizeof(Card));
            push_card(&local_state.players[0].hand, c);
        }
    }
}

/**
 * @brief Updates client logic and processes user input.
 * @param dt Delta time since last update.
 */
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

/**
 * @brief Renders the client-side UI and game state.
 */
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

/** @brief Module interface for the King-for-Four client. */
MiniGameModule KingForFourClientModule = {
    .id = 1,
    .name = "King For Four",
    .init = king_client_init,
    .on_data = king_client_on_data,
    .update = king_client_update,
    .draw = king_client_draw
};
