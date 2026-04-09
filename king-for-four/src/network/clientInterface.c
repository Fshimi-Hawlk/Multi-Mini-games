/**
 * @file king_client_module.c
 * @author i-Charlys
 * @author Fshimi-Hawlk
 * @date 2026-03-18
 * @date 2026-03-30
 * @brief Client-side module for the King-for-Four game, handling network synchronization and UI.
 */

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "core/game.h"
#include "ui/renderer.h"
#include "networkInterface.h"
#include "logger.h"
#include "APIs/generalAPI.h"

enum {
    ACTION_CODE_KFF_PLAY_CARD = firstAvailableActionCode,
    ACTION_CODE_KFF_DRAW_CARD,
    ACTION_CODE_KFF_SYNC_HAND,
    ACTION_CODE_KFF_SET_PLAYER_COUNT,
};

#pragma pack(push, 1)
typedef struct {
    int current_player;
    int active_color;
    Card top_card;
    int hand_sizes[4];
    int status;
    int host_id;
    int last_player_id;
    int last_action;
    int num_players;
    int requested_players;
} GameSyncPayload;

typedef struct {
    int card_index;
    int chosen_color; 
} ActionPlayPayload_St;
#pragma pack(pop)

static GameState local_state;
static GameAssets assets;
static bool assets_loaded = false;
static int my_internal_id = -1;
static int game_status = 0;
static float join_retry_timer = 0;

static float turn_overlay_timer = 0;
static int winner_id = -1;
static float last_move_timer = 0;
static int last_player_who_moved = -1;
static int last_action_type = -1;
static Card last_seen_top_card = {CARD_BLACK, ZERO};
static float card_pop_timer = 0;
static bool is_choosing_color = false;
static int pending_card_index = -1;

static void send_to_server(u8 action, void* data, u16 len) {
    GameTLVHeader_St tlv = { .game_id = MINI_GAME_KFF, .action = action, .length = htons(len) };
    RUDPHeader_St h; rudpGenerateHeader(&serverConnection, ACTION_CODE_GAME_DATA, &h);
    h.sender_id = htons((u16)(my_internal_id != -1 ? my_internal_id : 0));
    u8 buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    memcpy(buffer, &h, sizeof(h));
    memcpy(buffer + sizeof(h), &tlv, sizeof(tlv));
    if (len > 0 && data) memcpy(buffer + sizeof(h) + sizeof(tlv), data, len);
    send(networkSocket, buffer, sizeof(h) + sizeof(tlv) + len, 0);
}

void king_client_init(void) {
    if (!assets_loaded) {
        assets = LoadAssets();
        assets_loaded = true;
    }
    memset(&local_state, 0, sizeof(GameState));
    init_game_logic(&local_state);
    my_internal_id = -1;
    game_status = 0;
    winner_id = -1;
    join_retry_timer = 0;
    is_choosing_color = false;
    pending_card_index = -1;
}

static int selected_players = 4;

void king_client_on_data(s32 player_id, u8 action, const void* data, u16 len) {
    if (action != ACTION_CODE_JOIN_ACK) {
        if (player_id < 0 || (player_id >= MAX_CLIENTS && player_id != 999)) {
            log_warn("[KING] Data received from invalid player ID: %d", player_id);
            return;
        }
    }
    if (data == NULL) return;
    if (action == ACTION_CODE_JOIN_ACK) {
        if (len >= sizeof(u16)) {
            u16 net_id;
            memcpy(&net_id, data, sizeof(u16));
            my_internal_id = (int)ntohs(net_id);
            log_info("[KING] Mon ID interne: %d", my_internal_id);
        }
    } else if (action == ACTION_CODE_SYNC_GAME) {
        if (len >= (u16) sizeof(GameSyncPayload)) {
            GameSyncPayload sync;
            memcpy(&sync, data, sizeof(GameSyncPayload));

            selected_players = sync.requested_players;

            if (sync.current_player != local_state.current_player && sync.current_player == my_internal_id) {
                turn_overlay_timer = 2.0f;
            }
            local_state.current_player = sync.current_player;
            local_state.active_color = sync.active_color;
            local_state.num_players = sync.num_players;
            game_status = sync.status;

            extern void updateWaitingRoomInfo(int players, int max, bool host);
            updateWaitingRoomInfo(sync.num_players, 4, (my_internal_id == 0));

            if (sync.last_player_id != -1) {
                last_player_who_moved = sync.last_player_id;
                last_action_type = sync.last_action;
                last_move_timer = 1.5f;
                if (sync.top_card.color != last_seen_top_card.color || sync.top_card.value != last_seen_top_card.value) {
                    card_pop_timer = 0.5f;
                    last_seen_top_card = sync.top_card;
                }
            }
            if (local_state.discard_pile.size == 0) push_card(&local_state.discard_pile, sync.top_card);
            else local_state.discard_pile.cards[local_state.discard_pile.size - 1] = sync.top_card;
            
            int total_cards = 0;
            for (int i = 0; i < sync.num_players; i++) {
                total_cards += sync.hand_sizes[i];
                local_state.players[i].hand.size = sync.hand_sizes[i];
            }
            if (total_cards > 10 && game_status == 1) {
                for (int i = 0; i < sync.num_players; i++) {
                    if (sync.hand_sizes[i] == 0) winner_id = i;
                }
            }
        }
    } else if (action == ACTION_CODE_KFF_SYNC_HAND) {
        int count = len / sizeof(Card);
        if (my_internal_id >= 0 && my_internal_id < 4) {
            clear_deck(&local_state.players[my_internal_id].hand);
            for (int i = 0; i < count; i++) {
                Card c; memcpy(&c, (u8*)data + (i * sizeof(Card)), sizeof(Card));
                push_card(&local_state.players[my_internal_id].hand, c);
            }
        }
    }
}

void king_client_update(float dt) {
    if (!assets_loaded) return;
    if (my_internal_id == -1) {
        join_retry_timer += dt;
        if (join_retry_timer > 1.0f) {
            send_to_server(ACTION_CODE_JOIN_GAME, NULL, 0);
            join_retry_timer = 0;
        }
    }
    if (turn_overlay_timer > 0) turn_overlay_timer -= dt;
    if (last_move_timer > 0) last_move_timer -= dt;
    if (card_pop_timer > 0) card_pop_timer -= dt;

    if (game_status == 1) {
        if (is_choosing_color) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 m = GetMousePosition();
                int sw = GetScreenWidth(); int sh = GetScreenHeight();
                Rectangle colors[4] = {
                    { sw / 2.0f - 100, sh / 2.0f - 100, 100, 100 }, { sw / 2.0f, sh / 2.0f - 100, 100, 100 },
                    { sw / 2.0f - 100, sh / 2.0f, 100, 100 }, { sw / 2.0f, sh / 2.0f, 100, 100 }
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
            if (my_internal_id >= 0 && my_internal_id < 4) {
                int clickedHandIndex = GetHoveredCardIndex(&local_state.players[my_internal_id], assets);
                if (clickedHandIndex != -1) {
                    if (local_state.players[my_internal_id].hand.cards[clickedHandIndex].color == CARD_BLACK) {
                        is_choosing_color = true; pending_card_index = clickedHandIndex;
                    } else {
                        ActionPlayPayload_St payload = { .card_index = clickedHandIndex, .chosen_color = -1 };
                        send_to_server(ACTION_CODE_KFF_PLAY_CARD, &payload, sizeof(payload));
                    }
                } else if (CheckCollisionPointRec(GetMousePosition(), GetDeckRect(assets))) {
                    send_to_server(ACTION_CODE_KFF_DRAW_CARD, NULL, 0);
                }
            }
        }
    }
    if (game_status == 0 && my_internal_id == 0) {
        bool changed = false;
        if (IsKeyPressed(KEY_UP) && selected_players < 4) { selected_players++; changed = true; }
        if (IsKeyPressed(KEY_DOWN) && selected_players > 2) { selected_players--; changed = true; }
        
        if (changed) {
            send_to_server(ACTION_CODE_KFF_SET_PLAYER_COUNT, &selected_players, sizeof(int));
        }

        if (IsKeyPressed(KEY_ENTER)) send_to_server(ACTION_CODE_START_GAME, &selected_players, sizeof(int));
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        send_to_server(ACTION_CODE_QUIT_GAME, NULL, 0);
        extern void switch_minigame(u8 game_id);
        switch_minigame(0);
    }
}

void king_client_draw(void) {
    if (!assets_loaded) return;
    if (game_status == 0) {
        DrawText("KING FOR FOUR - SALLE D'ATTENTE", 100, 100, 40, GOLD);
        if (my_internal_id != -1) {
            DrawText(TextFormat("Vous êtes le JOUEUR %d", my_internal_id), 100, 180, 30, WHITE);
            DrawText(TextFormat("Joueurs connectés : %d", local_state.num_players), 100, 215, 22, LIGHTGRAY);
            if (my_internal_id == 0) {
                DrawText(TextFormat("HÔTE: Flèches HAUT/BAS : %d JOUEURS (dont bots)", selected_players), 100, 250, 25, GREEN);
                DrawText("Appuyez sur ENTRÉE pour lancer.", 100, 285, 25, GREEN);
            } else {
                DrawText(TextFormat("En attente de l'hôte... (%d JOUEURS)", selected_players), 100, 250, 30, LIGHTGRAY);
            }
        } else DrawText("Connexion au serveur...", 100, 180, 30, GRAY);
        return;
    }
    RenderTable(&local_state, assets, card_pop_timer > 0 ? (card_pop_timer * 0.2f) : 0);
    RenderOpponents(&local_state, assets, my_internal_id);
    if (my_internal_id >= 0 && my_internal_id < 4) {
        RenderHand(&local_state.players[my_internal_id], assets);
    }
    if (last_move_timer > 0 && last_player_who_moved != -1) {
        const char* pName = (last_player_who_moved == my_internal_id) ? "VOUS" : TextFormat("JOUEUR %d", last_player_who_moved);
        const char* actionName = (last_action_type == 0) ? "a JOUÉ une carte" : "a PIOCHÉ une carte";
        float alpha = last_move_timer > 0.5f ? 1.0f : last_move_timer * 2.0f;
        DrawText(TextFormat("%s %s", pName, actionName), GetScreenWidth()/2 - 100, GetScreenHeight()/2 + 80, 20, Fade(GOLD, alpha));
    }
    if (local_state.current_player == my_internal_id) {
        float pulse = (sinf(GetTime() * 10.0f) + 1.0f) * 0.5f;
        DrawText("C'EST VOTRE TOUR !", 10, 40, 25, Fade(GREEN, 0.5f + pulse * 0.5f));
    } else DrawText(TextFormat("Tour du Joueur %d", local_state.current_player), 10, 40, 25, YELLOW);
    if (local_state.active_color != -1) {
        Color c = RED; const char* name = "ROUGE";
        if (local_state.active_color == 1) { c = YELLOW; name = "JAUNE"; }
        else if (local_state.active_color == 2) { c = GREEN; name = "VERT"; }
        else if (local_state.active_color == 3) { c = BLUE; name = "BLEU"; }
        DrawText(TextFormat("COULEUR DEMANDÉE : %s", name), 10, 70, 20, c);
    }
    if (is_choosing_color) {
        int sw = GetScreenWidth(); int sh = GetScreenHeight();
        DrawRectangle(0, 0, sw, sh, Fade(BLACK, 0.7f));
        DrawText("CHOISISSEZ UNE COULEUR", sw / 2.0f - 150, sh / 2.0f - 150, 25, WHITE);
        DrawRectangle(sw / 2.0f - 100, sh / 2.0f - 100, 100, 100, RED);
        DrawRectangle(sw / 2.0f, sh / 2.0f - 100, 100, 100, YELLOW);
        DrawRectangle(sw / 2.0f - 100, sh / 2.0f, 100, 100, GREEN);
        DrawRectangle(sw / 2.0f, sh / 2.0f, 100, 100, BLUE);
    }
    if (turn_overlay_timer > 0) {
        float alpha = turn_overlay_timer > 1.0f ? 0.8f : turn_overlay_timer * 0.8f;
        DrawRectangle(0, GetScreenHeight()/2 - 60, GetScreenWidth(), 120, Fade(GOLD, alpha));
        DrawText("C'EST VOTRE TOUR", GetScreenWidth()/2 - MeasureText("C'EST VOTRE TOUR", 60)/2, GetScreenHeight()/2 - 30, 60, WHITE);
    }
    if (winner_id != -1) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));
        const char* txt = (winner_id == my_internal_id) ? "VICTOIRE !" : TextFormat("JOUEUR %d A GAGNÉ !", winner_id);
        DrawText(txt, GetScreenWidth()/2 - MeasureText(txt, 80)/2, GetScreenHeight()/2 - 100, 80, winner_id == my_internal_id ? GREEN : RED);
        DrawText("Appuyez sur ESC pour quitter", GetScreenWidth()/2 - 150, GetScreenHeight()/2 + 50, 20, GRAY);
    }
    DrawText("ESC pour quitter", GetScreenWidth() - 150, 10, 15, GRAY);
}

GameClientInterface_St kingForFourClientInterface = {
    .id = MINI_GAME_KFF,
    .name = "King For Four",
    .init = king_client_init,
    .on_data = king_client_on_data,
    .update = king_client_update,
    .draw = king_client_draw
};
