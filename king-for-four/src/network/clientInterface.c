/**
 * @file king_client_module.c
 * @author i-Charlys
 * @date 2026-03-18
 * @brief Client-side module for the King-for-Four game, handling network synchronization and UI.
 */

#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "core/game.h"
#include "ui/renderer.h"

#include "networkInterface.h"

#include "APIs/generalAPI.h"

enum {
    ACTION_CODE_KFF_PLAY_CARD = firstAvailableActionCode,
    ACTION_CODE_KFF_DRAW_CARD,
    ACTION_CODE_KFF_SYNC_HAND,
};

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
    int last_player_id;     /**< ID of last player who moved */
    int last_action;        /**< 0: Play, 1: Draw */
} GameSyncPayload;

typedef struct {
    int card_index;
    int chosen_color; // 0:Red, 1:Yellow, 2:Green, 3:Blue
} ActionPlayPayload_St;
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

// UI effects state
static float turn_overlay_timer = 0;
static int winner_id = -1;

// Last Move Visuals
static float last_move_timer = 0;
static int last_player_who_moved = -1;
static int last_action_type = -1; // 0: Play, 1: Draw
static Card last_seen_top_card = {CARD_BLACK, ZERO};
static float card_pop_timer = 0;

// Color selection state
static bool is_choosing_color = false;
static int pending_card_index = -1;

/**
 * @brief Sends a game action to the server.
 * @param action Action code.
 * @param data Pointer to payload data.
 * @param len Length of payload.
 */
static void send_to_server(u8 action, void* data, u16 len) {
    GameTLVHeader_St tlv = { .game_id = MINI_GAME_KFF, .action = action, .length = len };
    RUDPHeader_St h;
    rudpGenerateHeader(&serverConnection, ACTION_GAME_DATA, &h);
    
    u8 buffer[1024];
    memcpy(buffer, &h, sizeof(h));
    memcpy(buffer + sizeof(h), &tlv, sizeof(tlv));
    if (len > 0 && data) memcpy(buffer + sizeof(h) + sizeof(tlv), data, len);
    
    send(networkSocket, buffer, sizeof(h) + sizeof(tlv) + len, 0);
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
    
    if (networkSocket == -1) {
        my_internal_id = 0;
        printf("[KING CLIENT] Mode Solo: ID interne fixé à 0\n");
    } else {
        my_internal_id = -1;
    }

    game_status = 0;
    winner_id = -1;
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
void king_client_on_data(int player_id, u8 action, const void* data, u16 len) {
    // if (action == 5 /* LOBBY_CHAT */) {
    //     AddChatMessage(TextFormat("Player %d", player_id), (char*)data);
    // }

    (void)player_id;
    
    if (data == NULL) return;
    if (action == ACTION_CODE_JOIN_ACK) {
        if (len >= (u16) sizeof(int)) {
            memcpy(&my_internal_id, data, sizeof(int));
            printf("[KING CLIENT] Mon ID interne: %d\n", my_internal_id);
        }
    } else if (action == ACTION_CODE_SYNC_GAME) {
        if (len >= (u16) sizeof(GameSyncPayload)) {
            GameSyncPayload sync;
            memcpy(&sync, data, sizeof(GameSyncPayload));
            
            if (sync.current_player != local_state.current_player) {
                if (sync.current_player == my_internal_id) {
                    turn_overlay_timer = 2.0f; // Show "YOUR TURN" for 2s
                }
            }

            local_state.current_player = sync.current_player;
            local_state.active_color = sync.active_color;
            game_status = sync.status;
            local_state.num_players = 4; // Max
            
            // Track last move for visual feedback
            if (sync.last_player_id != -1) {
                last_player_who_moved = sync.last_player_id;
                last_action_type = sync.last_action;
                last_move_timer = 1.5f; // Show move info for 1.5s
                
                if (sync.top_card.color != last_seen_top_card.color || sync.top_card.value != last_seen_top_card.value) {
                    card_pop_timer = 0.5f; // Pop effect for 0.5s
                    last_seen_top_card = sync.top_card;
                }
            }

            if (local_state.discard_pile.size == 0) {
                push_card(&local_state.discard_pile, sync.top_card);
            } else {
                local_state.discard_pile.cards[local_state.discard_pile.size - 1] = sync.top_card;
            }
            
            int total_cards = 0;
            for (int i = 0; i < 4; i++) {
                total_cards += sync.hand_sizes[i];
                local_state.players[i].hand.size = sync.hand_sizes[i];
            }

            // Only declare winner if distribution is finished (total cards > some threshold)
            if (total_cards > 10 && game_status == 1) {
                for (int i = 0; i < 4; i++) {
                    if (sync.hand_sizes[i] == 0) {
                        winner_id = i;
                    }
                }
            }
        }
    } else if (action == ACTION_CODE_KFF_SYNC_HAND) {
        int count = len / sizeof(Card);
        u8* ptr = (u8*)data;
        
        clear_deck(&local_state.players[0].hand);
        
        for (int i = 0; i < count; i++) {
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
    if (!assets_loaded) return;

    if (networkSocket != -1 && my_internal_id == -1) {
        join_retry_timer += dt;
        if (join_retry_timer > 1.0f) {
            send_to_server(ACTION_CODE_JOIN_GAME, NULL, 0);
            join_retry_timer = 0;
        }
    }

    // UpdateChat();
    // if (g_chatState.isOpen) return; // Ignore input when chatting

    if (turn_overlay_timer > 0) turn_overlay_timer -= dt;
    if (last_move_timer > 0) last_move_timer -= dt;
    if (card_pop_timer > 0) card_pop_timer -= dt;

    if (!assets_loaded) return;
    if (my_internal_id == -1) {
        join_retry_timer += dt;
        if (join_retry_timer > 1.0f) {
            send_to_server(ACTION_CODE_JOIN_GAME, NULL, 0);
            join_retry_timer = 0;
        }
    }

    if (game_status == 1) {
        if (is_choosing_color) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 m = GetMousePosition();
                int sw = GetScreenWidth(); int sh = GetScreenHeight();
                Rectangle colors[4] = {
                    { sw / 2.0f - 100, sh / 2.0f - 100, 100, 100 }, // Red
                    { sw / 2.0f,       sh / 2.0f - 100, 100, 100 }, // Yellow
                    { sw / 2.0f - 100, sh / 2.0f,       100, 100 }, // Green
                    { sw / 2.0f,       sh / 2.0f,       100, 100 }  // Blue
                };
                for (int i = 0; i < 4; i++) {
                    if (CheckCollisionPointRec(m, colors[i])) {
                        ActionPlayPayload_St payload = { .card_index = pending_card_index, .chosen_color = i };
                        send_to_server(ACTION_CODE_KFF_PLAY_CARD, &payload, sizeof(payload));
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
                if (local_state.players[0].hand.cards[clickedHandIndex].color == CARD_BLACK) {
                    is_choosing_color = true;
                    pending_card_index = clickedHandIndex;
                } else {
                    ActionPlayPayload_St payload = { .card_index = clickedHandIndex, .chosen_color = -1 };
                    send_to_server(ACTION_CODE_KFF_PLAY_CARD, &payload, sizeof(payload));
                }
            } else {
                Rectangle deckRect = GetDeckRect(assets);
                if (CheckCollisionPointRec(GetMousePosition(), deckRect)) {
                    send_to_server(ACTION_CODE_KFF_DRAW_CARD, NULL, 0);
                }
            }
        }
    }

    if (game_status == 0 && my_internal_id == 0) {
        if (IsKeyPressed(KEY_UP) && selected_players < 4) selected_players++;
        if (IsKeyPressed(KEY_DOWN) && selected_players > 2) selected_players--;
        
        if (IsKeyPressed(KEY_ENTER) || (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), (Rectangle){100, 280, 400, 50}))) {
            send_to_server(ACTION_CODE_START_GAME, &selected_players, sizeof(int));
        }
    }
    
    if (IsKeyPressed(KEY_ESCAPE)) {
        send_to_server(ACTION_CODE_QUIT_GAME, NULL, 0);
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

    RenderTable(&local_state, assets, card_pop_timer > 0 ? (card_pop_timer * 0.2f) : 0);
    RenderOpponents(&local_state, assets, my_internal_id);
    RenderHand(&local_state.players[0], assets);
    
    // Last Action Notification
    if (last_move_timer > 0 && last_player_who_moved != -1) {
        const char* pName = (last_player_who_moved == my_internal_id) ? "VOUS" : TextFormat("JOUEUR %d", last_player_who_moved);
        const char* actionName = (last_action_type == 0) ? "a JOUÉ une carte" : "a PIOCHÉ une carte";
        Color c = (last_player_who_moved == my_internal_id) ? GREEN : SKYBLUE;
        float alpha = last_move_timer > 0.5f ? 1.0f : last_move_timer * 2.0f;
        DrawText(TextFormat("%s %s", pName, actionName), GetScreenWidth()/2 - 100, GetScreenHeight()/2 + 80, 20, Fade(c, alpha));
    }

    if (local_state.current_player == my_internal_id) {
        float pulse = (sinf(GetTime() * 10.0f) + 1.0f) * 0.5f;
        DrawText("C'EST VOTRE TOUR !", 10, 40, 25, Fade(GREEN, 0.5f + pulse * 0.5f));
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
        DrawText("CHOISISSEZ UNE COULEUR", sw / 2.0f - 150, sh / 2.0f - 150, 25, WHITE);
        DrawRectangle(sw / 2.0f - 100, sh / 2.0f - 100, 100, 100, RED);
        DrawRectangle(sw / 2.0f,       sh / 2.0f - 100, 100, 100, YELLOW);
        DrawRectangle(sw / 2.0f - 100, sh / 2.0f,       100, 100, GREEN);
        DrawRectangle(sw / 2.0f,       sh / 2.0f,       100, 100, BLUE);
    }

    // YOUR TURN Overlay
    if (turn_overlay_timer > 0) {
        int sw = GetScreenWidth(); int sh = GetScreenHeight();
        float alpha = turn_overlay_timer > 1.0f ? 0.8f : turn_overlay_timer * 0.8f;
        DrawRectangle(0, sh/2 - 60, sw, 120, Fade(GOLD, alpha));
        const char* turnTxt = "C'EST VOTRE TOUR";
        DrawText(turnTxt, sw/2 - MeasureText(turnTxt, 60)/2, sh/2 - 30, 60, WHITE);
    }

    // Victory/Defeat Screen
    if (winner_id != -1) {
        int sw = GetScreenWidth(); int sh = GetScreenHeight();
        DrawRectangle(0, 0, sw, sh, Fade(BLACK, 0.8f));
        const char* txt = (winner_id == my_internal_id) ? "VICTOIRE !" : TextFormat("JOUEUR %d A GAGNÉ !", winner_id);
        Color c = (winner_id == my_internal_id) ? GREEN : RED;
        DrawText(txt, sw/2 - MeasureText(txt, 80)/2, sh/2 - 100, 80, c);
        DrawText("Appuyez sur ESC pour quitter", sw/2 - 150, sh/2 + 50, 20, GRAY);
    }
    
    DrawText("ESC pour quitter", GetScreenWidth() - 150, 10, 15, GRAY);
}

/** @brief Module interface for the King-for-Four client. */
GameClientInterface_St KingForFourClientModule = {
    .id = MINI_GAME_KFF,
    .name = "King For Four",
    .init = king_client_init,
    .on_data = king_client_on_data,
    .update = king_client_update,
    .draw = king_client_draw
};
