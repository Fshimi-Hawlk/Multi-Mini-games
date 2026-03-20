/**
 * @file king_client_module.c
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Client-side module for the King-for-Four game, handling network synchronization and UI.
 */

#include "APIs/generalAPI.h"
#include "core/protocol.h"
#include "utils/chat.h"
#include "utils/globals.h"
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

// Color selection state
static bool is_choosing_color = false;
static int pending_card_index = -1;

/**
 * @brief Sends a game action to the server.
 * @param action Action code.
 * @param data Pointer to payload data.
 * @param len Length of payload.
 */
static void send_to_server(uint8_t action, void* data, uint16_t len) {
    GameTLVHeader tlv = { .game_id = 1, .action = action, .length = len };
    RUDP_Header h;
    RUDP_GenerateHeader(&server_conn, ACTION_GAME_DATA, &h);
    
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
    is_choosing_color = false;
    pending_card_index = -1;
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

/** @brief Currently selected number of players for the next game. */
static int selected_players = 4;

/**
 * @brief Updates client logic and processes user input.
 * @param dt Delta time since last update.
 */
void king_client_update(float dt) {
    if (g_chatState.isOpen) return; // Ignore input when chatting

    if (!assets_loaded) return;
    if (my_internal_id == -1) {
        join_retry_timer += dt;
        if (join_retry_timer > 1.0f) {
            send_to_server(ACTION_JOIN_GAME, NULL, 0);
            join_retry_timer = 0;
        }
    }

    if (game_status == 1) {
        if (is_choosing_color) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 m = GetMousePosition();
                int sw = GetScreenWidth(); int sh = GetScreenHeight();
                Rectangle colors[4] = {
                    { sw/2 - 100, sh/2 - 100, 100, 100 }, // Red
                    { sw/2,       sh/2 - 100, 100, 100 }, // Yellow
                    { sw/2 - 100, sh/2,       100, 100 }, // Green
                    { sw/2,       sh/2,       100, 100 }  // Blue
                };
                for (int i = 0; i < 4; i++) {
                    if (CheckCollisionPointRec(m, colors[i])) {
                        ActionPlayPayload_St payload = { .card_index = pending_card_index, .chosen_color = i };
                        send_to_server(ACTION_PLAY_CARD, &payload, sizeof(payload));
                        is_choosing_color = false;
                        break;
                    }
                }
            }
        }
        else if (local_state.current_player == my_internal_id && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            int clickedHandIndex = GetHoveredCardIndex(&local_state.players[0], assets);
            if (clickedHandIndex != -1) {
                // Check if card is black
                Node* curr = local_state.players[0].hand.head;
                for (int i = 0; i < clickedHandIndex; i++) curr = curr->next;
                
                if (curr->card.color == CARD_BLACK) {
                    is_choosing_color = true;
                    pending_card_index = clickedHandIndex;
                } else {
                    ActionPlayPayload_St payload = { .card_index = clickedHandIndex, .chosen_color = -1 };
                    send_to_server(ACTION_PLAY_CARD, &payload, sizeof(payload));
                }
            } else {
                Rectangle deckRect = GetDeckRect(assets);
                if (CheckCollisionPointRec(GetMousePosition(), deckRect)) {
                    send_to_server(ACTION_DRAW_CARD, NULL, 0);
                }
            }
        }
    }

    if (game_status == 0 && my_internal_id == 0) {
        if (IsKeyPressed(KEY_UP) && selected_players < 4) selected_players++;
        if (IsKeyPressed(KEY_DOWN) && selected_players > 2) selected_players--;
        
        if (IsKeyPressed(KEY_ENTER)) {
            send_to_server(ACTION_START_GAME, &selected_players, sizeof(int));
        }
    }
    
    if (IsKeyPressed(KEY_ESCAPE)) {
        send_to_server(ACTION_QUIT_GAME, NULL, 0);
        extern void switch_minigame(uint8_t game_id);
        switch_minigame(0);
    }
}

/**
 * @brief Renders the client-side UI and game state.
 */
void king_client_draw(void) {
    if (!assets_loaded) return;
    
    if (game_status == 0) {
        DrawText("KING FOR FOUR - SALLE D'ATTENTE", 100, 100, 40, GOLD);
        if (my_internal_id != -1) {
            DrawText(TextFormat("Vous êtes le JOUEUR %d", my_internal_id), 100, 180, 30, WHITE);
            if (my_internal_id == 0) {
                DrawText(TextFormat("HÔTE: Flèches HAUT/BAS pour changer : %d JOUEURS", selected_players), 100, 240, 25, GREEN);
                DrawText("Appuyez sur ENTRÉE pour lancer.", 100, 280, 25, GREEN);
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

    if (local_state.active_color != -1) {
        Color c = RED;
        const char* name = "ROUGE";
        if (local_state.active_color == 1) { c = YELLOW; name = "JAUNE"; }
        if (local_state.active_color == 2) { c = GREEN; name = "VERT"; }
        if (local_state.active_color == 3) { c = BLUE; name = "BLEU"; }
        DrawText(TextFormat("COULEUR DEMANDÉE : %s", name), 10, 70, 20, c);
    }

    if (is_choosing_color) {
        int sw = GetScreenWidth(); int sh = GetScreenHeight();
        DrawRectangle(0, 0, sw, sh, Fade(BLACK, 0.7f));
        DrawText("CHOISISSEZ UNE COULEUR", sw/2 - 150, sh/2 - 150, 25, WHITE);
        DrawRectangle(sw/2 - 100, sh/2 - 100, 100, 100, RED);
        DrawRectangle(sw/2,       sh/2 - 100, 100, 100, YELLOW);
        DrawRectangle(sw/2 - 100, sh/2,       100, 100, GREEN);
        DrawRectangle(sw/2,       sh/2,       100, 100, BLUE);
    }
    
    DrawText("ESC pour quitter", GetScreenWidth() - 150, 10, 15, GRAY);
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
