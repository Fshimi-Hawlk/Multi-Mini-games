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
#include "firstparty/progress.h"
#include "firstparty/leaderboard.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern PlayerProgress_St g_progress;

/** @brief External reference to the network socket. */
extern s32 network_socket;
/** @brief External reference to the server connection. */
extern RUDP_Connection server_conn;
/** @brief External reference to the client ID. */
extern s32 my_id;

/** @brief Network action code for game data. */
#define ACTION_GAME_DATA 5

/** @brief Local copy of the game state. */
static GameState local_state;
/** @brief Graphical assets. */
static GameAssets assets;
/** @brief Flag indicating if assets are loaded. */
static bool assets_loaded = false;
/** @brief This client's internal player ID assigned by server. */
static s32 my_internal_id = -1;
/** @brief Current status of the game. */
static s32 game_status = 0; 
/** @brief Winner of the game. */
static s32 winner_id = -1; 
/** @brief Timer for retrying to join the game. */
static f32 join_retry_timer = 0;

// Color selection state
static bool is_choosing_color = false;
static s32 pending_card_index = -1;

/**
 * @brief Sends a game action to the server.
 */
static void send_to_server(u8 action, void* data, u16 len) {
    GameTLVHeader tlv = { .game_id = 1, .action = action, .length = len };
    RUDP_Header h;
    RUDP_GenerateHeader(&server_conn, ACTION_GAME_DATA, &h);
    h.sender_id = htons((u16)my_id); // Use the global my_id from main.c
    
    u8 buffer[1024];
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
 */
void king_client_on_data(s32 player_id, u8 action, void* data, u16 len) {
    (void)player_id;
    if (data == NULL) return;

    if (action == ACTION_JOIN_ACK) {
        if (len >= sizeof(s32)) {
            memcpy(&my_internal_id, data, sizeof(s32));
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
            winner_id = sync.winner_id;

            // Récompenses de victoire
            if (game_status == 2 && winner_id == my_internal_id) {
                u8 game_id = 1; 
                if (!g_progress.skin_unlocked[game_id][0][0]) {
                    g_progress.skin_unlocked[game_id][0][0] = true;
                    printf("[KING] NOUVEAU SKIN DÉBLOQUÉ !\n");
                }
                
                s32 score = 1000; 
                s32 rank = SubmitScore(game_id, "Joueur", score);
                g_progress.leaderboard_rank[game_id][0][0] = rank;
                
                Leaderboard_St lb = LoadLeaderboard(game_id);
                g_progress.total_players[game_id][0][0] = lb.count;
                g_progress.current_ap[game_id] = CalculateAPTier(rank, lb.count);
                
                if (rank == 1) {
                    g_progress.has_crown = true;
                    g_progress.jewel_unlocked[game_id][0][0] = true;
                    printf("[KING] JOYAU OBTENU !\n");
                }
                
                SaveProgress(&g_progress);
            }
            
            if (local_state.discard_pile.head == NULL) {
                push_card(&local_state.discard_pile, sync.top_card);
            } else {
                local_state.discard_pile.head->card = sync.top_card;
            }
            
            for (s32 i = 0; i < 4; i++) {
                // Important : On ne met à jour la taille des mains QUE pour les autres joueurs.
                // Notre propre taille de main est gérée par ACTION_SYNC_HAND.
                if (i != my_internal_id) {
                    local_state.players[i].hand.size = sync.hand_sizes[i];
                }
            }
        }
    }
    else if (action == ACTION_SYNC_HAND) {
        if (my_internal_id < 0) return;
        s32 count = len / sizeof(Card);
        u8* ptr = (u8*)data;
        
        clear_deck(&local_state.players[my_internal_id].hand);
        
        for (s32 i = count - 1; i >= 0; i--) {
            Card c;
            memcpy(&c, ptr + (i * sizeof(Card)), sizeof(Card));
            push_card(&local_state.players[my_internal_id].hand, c);
        }
    }
}

/** @brief Currently selected number of players for the next game. */
static s32 selected_players = 4;

/**
 * @brief Updates client logic and processes user input.
 * @param dt Delta time since last update.
 */
void king_client_update(f32 dt) {
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
                s32 sw = GetScreenWidth(); s32 sh = GetScreenHeight();
                Rectangle colors[4] = {
                    { sw/2.0f - 100, sh/2.0f - 100, 100, 100 }, // Red
                    { sw/2.0f,       sh/2.0f - 100, 100, 100 }, // Yellow
                    { sw/2.0f - 100, sh/2.0f,       100, 100 }, // Green
                    { sw/2.0f,       sh/2.0f,       100, 100 }  // Blue
                };
                for (s32 i = 0; i < 4; i++) {
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
            if (my_internal_id < 0) return;
            s32 clickedHandIndex = GetHoveredCardIndex(&local_state.players[my_internal_id], assets);
            if (clickedHandIndex != -1) {
                // Check if card is black
                Node* curr = local_state.players[my_internal_id].hand.head;
                for (s32 i = 0; i < clickedHandIndex && curr; i++) curr = curr->next;
                
                if (curr && curr->card.color == CARD_BLACK) {
                    is_choosing_color = true;
                    pending_card_index = clickedHandIndex;
                } else if (curr) {
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
            send_to_server(ACTION_START_GAME, &selected_players, sizeof(s32));
        }
    }
    
    if (IsKeyPressed(KEY_ESCAPE)) {
        send_to_server(ACTION_QUIT_GAME, NULL, 0);
        extern void switch_minigame(u8 game_id);
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

    if (game_status == 2) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));
        const char* text = (winner_id == my_internal_id) ? "VICTOIRE !" : "DÉFAITE...";
        Color color = (winner_id == my_internal_id) ? GOLD : RED;
        DrawText(text, GetScreenWidth()/2 - MeasureText(text, 60)/2, GetScreenHeight()/2 - 100, 60, color);
        
        const char* sub = TextFormat("Le Joueur %d a gagné la partie.", winner_id);
        DrawText(sub, GetScreenWidth()/2 - MeasureText(sub, 25)/2, GetScreenHeight()/2, 25, WHITE);
        
        DrawText("Appuyez sur ESC pour quitter", GetScreenWidth()/2 - MeasureText("Appuyez sur ESC pour quitter", 20)/2, GetScreenHeight()/2 + 100, 20, GRAY);
        return;
    }

    RenderTable(&local_state, assets);
    if (my_internal_id >= 0) RenderHand(&local_state.players[my_internal_id], assets);
    
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
        s32 sw = GetScreenWidth(); s32 sh = GetScreenHeight();
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
