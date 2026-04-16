/**
    @file soloInterface.c
    @brief Solo mode interface for King for Four - plays locally with bots without network.
*/
#include <string.h>
#include <math.h>
#include "../../include/core/game.h"
#include "../../include/core/bot.h"
#include "../../include/ui/renderer.h"
#include "networkInterface.h"
#include "logger.h"
#include "APIs/generalAPI.h"

static KingForFourGameState_St soloState;
static GameAssets_St assets;
static bool assets_loaded = false;
static int selectedPlayers = 4;
static int solo_localPlayerId = 0;
static int gameStatus = 0; // 0 = menu, 1 = playing
static int winnerId = -1;

static float bot_think_timer = 0.0f;
static float turn_overlay_timer = 0.0f;
static float cardPop_timer = 0.0f;
static int lastPlayer_who_moved = -1;
static float last_move_timer = 0.0f;
static int last_action_type = -1;
static Card_St lastSeen_topCard = {CARD_BLACK, ZERO};

static bool isChoosingColor = false;
static int pendingCardIndex = -1;
static bool showInfo_window = false;

static void applySoloCardEffect(Card_St card) {
    if (card.value == SKIP) {
        soloState.currentPlayer = (soloState.currentPlayer + soloState.gameDirection + soloState.numPlayers) % soloState.numPlayers;
        turn_overlay_timer = 2.0f; // For visual feedback
    } else if (card.value == REVERSE) {
        if (soloState.numPlayers == 2) {
            soloState.currentPlayer = (soloState.currentPlayer + soloState.gameDirection + soloState.numPlayers) % soloState.numPlayers;
        } else {
            soloState.gameDirection *= -1;
        }
    } else if (card.value == PLUS_TWO) {
        int victim = (soloState.currentPlayer + soloState.gameDirection + soloState.numPlayers) % soloState.numPlayers;
        kingForFour_playerDrawCard(&soloState, victim);
        kingForFour_playerDrawCard(&soloState, victim);
        soloState.currentPlayer = victim; // Skip victim's turn
    } else if (card.value == PLUS_FOUR) {
        int victim = (soloState.currentPlayer + soloState.gameDirection + soloState.numPlayers) % soloState.numPlayers;
        for (int i = 0; i < 4; i++) kingForFour_playerDrawCard(&soloState, victim);
        soloState.currentPlayer = victim; // Skip victim's turn
    }
}

static void initPlayers(void) {
    for (int i = 0; i < selectedPlayers; i++) {
        soloState.players[i].id = i;
        soloState.players[i].is_local = (i == solo_localPlayerId);
        snprintf(soloState.players[i].name, 50, i == 0 ? "Vous" : "Bot %d", i);
    }
    // Set remaining players as inactive
    for (int i = selectedPlayers; i < 4; i++) {
        soloState.players[i].id = -1;
        soloState.players[i].is_local = 0;
    }
}

void kingForFour_soloInit(void) {
    if (!assets_loaded) {
        assets = kingForFour_loadAssets();
        assets_loaded = true;
    }
    // Reset to menu state
    gameStatus = 0;
}

static void startSoloGame(void) {
    memset(&soloState, 0, sizeof(KingForFourGameState_St));
    kingForFour_initGameLogic(&soloState);
    initPlayers();
    soloState.numPlayers = selectedPlayers;
    kingForFour_distributeCards(&soloState);
    gameStatus = 1;
    winnerId = -1;
    bot_think_timer = 0.5f;
    turn_overlay_timer = 0.0f;
    cardPop_timer = 0.0f;
    lastPlayer_who_moved = -1;
    last_move_timer = 0.0f;
    last_action_type = -1;
    lastSeen_topCard = (Card_St){CARD_BLACK, ZERO};
    isChoosingColor = false;
    pendingCardIndex = -1;
    log_info("[KING-SOLO] Mode solo démarré avec %d joueurs", selectedPlayers);
}

static void handleBotTurn(int playerId, float dt) {
    if (playerId == solo_localPlayerId) return;
    
    bot_think_timer -= dt;
    if (bot_think_timer > 0) return;
    
    bot_think_timer = 0.8f + ((float)(playerId) * 0.2f);
    
    int cardIndex = -1;
    kingForFour_calculateBestMove(&soloState, playerId, &cardIndex);
    
    if (cardIndex >= 0) {
        Card_St card = soloState.players[playerId].hand.cards[cardIndex];
        if (card.color == CARD_BLACK) {
            // Simple bot color choice: pick color they have most of
            int counts[4] = {0,0,0,0};
            for(int i=0; i<soloState.players[playerId].hand.size; i++) {
                if (soloState.players[playerId].hand.cards[i].color != CARD_BLACK)
                    counts[soloState.players[playerId].hand.cards[i].color]++;
            }
            int chosen = 0;
            for(int i=1; i<4; i++) if (counts[i] > counts[chosen]) chosen = i;
            soloState.activeColor = chosen;
        }
        kingForFour_tryPlayCard(&soloState, playerId, cardIndex);
        lastPlayer_who_moved = playerId;
        last_action_type = 0;
        
        applySoloCardEffect(card);
    } else {
        kingForFour_playerDrawCard(&soloState, playerId);
        lastPlayer_who_moved = playerId;
        last_action_type = 1;
        
        // Bot tries to play the drawn card
        int newCardIndex = soloState.players[playerId].hand.size - 1;
        if (newCardIndex >= 0) {
            Card_St newCard = soloState.players[playerId].hand.cards[newCardIndex];
            Card_St top = soloState.discardPile.cards[soloState.discardPile.size - 1];
            if (kingForFour_isMoveValid(soloState.activeColor, newCard, top)) {
                if (newCard.color == CARD_BLACK) soloState.activeColor = GetRandomValue(0, 3);
                kingForFour_tryPlayCard(&soloState, playerId, newCardIndex);
                last_action_type = 0;
                applySoloCardEffect(newCard);
            }
        }
    }
    
    last_move_timer = 1.5f;
    Card_St top = soloState.discardPile.cards[soloState.discardPile.size - 1];
    if (top.color != lastSeen_topCard.color || top.value != lastSeen_topCard.value) {
        cardPop_timer = 0.5f;
        lastSeen_topCard = top;
    }
    
    // Normal advance
    soloState.currentPlayer = (soloState.currentPlayer + soloState.gameDirection + soloState.numPlayers) % soloState.numPlayers;
    
    for (int i = 0; i < soloState.numPlayers; i++) {
        if (soloState.players[i].hand.size == 0) {
            gameStatus = 0;
            winnerId = i;
            break;
        }
    }
}

void kingForFour_soloUpdate(float dt) {
    if (!assets_loaded) return;
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 m = GetMousePosition();
        Rectangle infoIconRect = { (float)GetScreenWidth() - 40, 40, 30, 30 };
        if (CheckCollisionPointRec(m, infoIconRect)) {
            showInfo_window = !showInfo_window;
        } else if (showInfo_window) {
            showInfo_window = false;
        }
    }

    // Menu state - player selection
    if (gameStatus == 0) {
        if (IsKeyPressed(KEY_UP) && selectedPlayers < 4) {
            selectedPlayers++;
        }
        if (IsKeyPressed(KEY_DOWN) && selectedPlayers > 2) {
            selectedPlayers--;
        }
        
        if (IsKeyPressed(KEY_ENTER)) {
            startSoloGame();
        }
        return;
    }
    
    // Playing state
    if (gameStatus == 1) {
        if (IsKeyPressed(KEY_ESCAPE)) {
            gameStatus = 0;
            return;
        }
    }
    
    if (turn_overlay_timer > 0) turn_overlay_timer -= dt;
    if (last_move_timer > 0) last_move_timer -= dt;
    if (cardPop_timer > 0) cardPop_timer -= dt;
    
    if (soloState.currentPlayer != solo_localPlayerId) {
        handleBotTurn(soloState.currentPlayer, dt);
    } else {
        if (isChoosingColor) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 m = GetMousePosition();
                int sw = GetScreenWidth(); int sh = GetScreenHeight();
                Rectangle colors[4] = {
                    { sw / 2.0f - 100, sh / 2.0f - 100, 100, 100 },
                    { sw / 2.0f, sh / 2.0f - 100, 100, 100 },
                    { sw / 2.0f - 100, sh / 2.0f, 100, 100 },
                    { sw / 2.0f, sh / 2.0f, 100, 100 }
                };
                for (int i = 0; i < 4; i++) {
                    if (CheckCollisionPointRec(m, colors[i])) {
                        soloState.activeColor = i;
                        if (pendingCardIndex >= 0) {
                            Card_St card = soloState.players[solo_localPlayerId].hand.cards[pendingCardIndex];
                            kingForFour_tryPlayCard(&soloState, solo_localPlayerId, pendingCardIndex);
                            lastPlayer_who_moved = solo_localPlayerId;
                            last_action_type = 0;
                            last_move_timer = 1.5f;
                            
                            applySoloCardEffect(card);
                            
                            // Advance
                            soloState.currentPlayer = (soloState.currentPlayer + soloState.gameDirection + soloState.numPlayers) % soloState.numPlayers;
                        }
                        isChoosingColor = false;
                        pendingCardIndex = -1;
                        break;
                    }
                }
            }
        } else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            int clickedHandIndex = kingForFour_getHoveredCardIndex(&soloState.players[solo_localPlayerId], assets);
            if (clickedHandIndex != -1) {
                Card_St card = soloState.players[solo_localPlayerId].hand.cards[clickedHandIndex];
                if (card.color == CARD_BLACK) {
                    isChoosingColor = true;
                    pendingCardIndex = clickedHandIndex;
                } else {
                    if (kingForFour_tryPlayCard(&soloState, solo_localPlayerId, clickedHandIndex)) {
                        lastPlayer_who_moved = solo_localPlayerId;
                        last_action_type = 0;
                        last_move_timer = 1.5f;
                        Card_St top = soloState.discardPile.cards[soloState.discardPile.size - 1];
                        if (top.color != lastSeen_topCard.color || top.value != lastSeen_topCard.value) {
                            cardPop_timer = 0.5f;
                            lastSeen_topCard = top;
                        }
                        
                        applySoloCardEffect(card);
                        
                        // Advance
                        soloState.currentPlayer = (soloState.currentPlayer + soloState.gameDirection + soloState.numPlayers) % soloState.numPlayers;
                        
                        for (int i = 0; i < soloState.numPlayers; i++) {
                            if (soloState.players[i].hand.size == 0) {
                                gameStatus = 0;
                                winnerId = i;
                                break;
                            }
                        }
                    }
                }
            } else if (CheckCollisionPointRec(GetMousePosition(), kingForFour_getDeckRect(assets))) {
                kingForFour_playerDrawCard(&soloState, solo_localPlayerId);
                lastPlayer_who_moved = solo_localPlayerId;
                last_action_type = 1;
                last_move_timer = 1.5f;
                Card_St top = soloState.discardPile.cards[soloState.discardPile.size - 1];
                if (top.color != lastSeen_topCard.color || top.value != lastSeen_topCard.value) {
                    cardPop_timer = 0.5f;
                    lastSeen_topCard = top;
                }
                // Advance to next player
                soloState.currentPlayer = (soloState.currentPlayer + soloState.gameDirection + soloState.numPlayers) % soloState.numPlayers;
            }
        }
    }
    
    if (soloState.currentPlayer != solo_localPlayerId) {
        turn_overlay_timer = 0.0f;
    } else {
        if (turn_overlay_timer <= 0) turn_overlay_timer = 2.0f;
    }
}

void kingForFour_soloDraw(void) {
    if (!assets_loaded) return;
    
    // FIX: Clear background since lobby doesn't do it for us anymore to prevent flashing
    ClearBackground((Color){20, 40, 20, 255});

    // Draw menu when not playing
    if (gameStatus == 0) {
        int sw = GetScreenWidth();
        int sh = GetScreenHeight();
        
        DrawText("KING FOR FOUR - MODE SOLO", sw/2 - MeasureText("KING FOR FOUR - MODE SOLO", 40)/2, sh/3, 40, GOLD);
        DrawText(TextFormat("Nombre de joueurs: %d", selectedPlayers), sw/2 - 100, sh/2, 30, WHITE);
        
        DrawText("Utilisez HAUT/BAS pour choisir", sw/2 - 150, sh/2 + 50, 20, LIGHTGRAY);
        DrawText("Appuyez sur ENTREE pour commencer", sw/2 - 180, sh/2 + 80, 20, GREEN);
        
        // Draw bot indicators
        int bots = selectedPlayers - 1;
        DrawText(TextFormat("Vous + %d Bot%s", bots, bots > 1 ? "s" : ""), sw/2 - 80, sh/2 + 120, 25, YELLOW);
        
        DrawText("ESC pour revenir au lobby", sw - 200, sh - 30, 15, GRAY);
        return;
    }
    
    // Draw game when playing
    kingForFour_renderTable(&soloState, assets, cardPop_timer > 0 ? (cardPop_timer * 0.2f) : 0);
    kingForFour_renderOpponents(&soloState, assets, solo_localPlayerId);
    kingForFour_renderHand(&soloState.players[solo_localPlayerId], assets);
    
    if (last_move_timer > 0 && lastPlayer_who_moved != -1) {
        const char* pName = (lastPlayer_who_moved == solo_localPlayerId) ? "VOUS" : soloState.players[lastPlayer_who_moved].name;
        const char* actionName = (last_action_type == 0) ? "a JOUÉ" : "a PIOCHÉ";
        float alpha = last_move_timer > 0.5f ? 1.0f : last_move_timer * 2.0f;
        DrawText(TextFormat("%s %s", pName, actionName), GetScreenWidth()/2 - 100, GetScreenHeight()/2 + 80, 20, Fade(GOLD, alpha));
    }
    
    if (soloState.currentPlayer == solo_localPlayerId) {
        float pulse = (sinf(GetTime() * 10.0f) + 1.0f) * 0.5f;
        DrawText("C'EST VOTRE TOUR !", 10, 40, 25, Fade(GREEN, 0.5f + pulse * 0.5f));
    } else {
        DrawText(TextFormat("Tour de %s", soloState.players[soloState.currentPlayer].name), 10, 40, 25, YELLOW);
    }
    
    if (soloState.activeColor != -1) {
        Color c = RED; const char* name = "ROUGE";
        if (soloState.activeColor == 1) { c = YELLOW; name = "JAUNE"; }
        else if (soloState.activeColor == 2) { c = GREEN; name = "VERT"; }
        else if (soloState.activeColor == 3) { c = BLUE; name = "BLEU"; }
        DrawText(TextFormat("COULEUR : %s", name), 10, 70, 20, c);
    }
    
    if (isChoosingColor) {
        int sw = GetScreenWidth(); int sh = GetScreenHeight();
        DrawRectangle(0, 0, sw, sh, Fade(BLACK, 0.7f));
        DrawText("CHOISISSEZ UNE COULEUR", sw / 2.0f - 150, sh / 2.0f - 150, 25, WHITE);
        DrawRectangle(sw / 2.0f - 100, sh / 2.0f - 100, 100, 100, RED);
        DrawRectangle(sw / 2.0f, sh / 2.0f - 100, 100, 100, YELLOW);
        DrawRectangle(sw / 2.0f - 100, sh / 2.0f, 100, 100, GREEN);
        DrawRectangle(sw / 2.0f, sh / 2.0f, 100, 100, BLUE);
    }
    
    if (turn_overlay_timer > 0 && soloState.currentPlayer == solo_localPlayerId) {
        float alpha = turn_overlay_timer > 1.0f ? 0.8f : turn_overlay_timer * 0.8f;
        DrawRectangle(0, GetScreenHeight()/2 - 60, GetScreenWidth(), 120, Fade(GOLD, alpha));
        DrawText("C'EST VOTRE TOUR", GetScreenWidth()/2 - MeasureText("C'EST VOTRE TOUR", 60)/2, GetScreenHeight()/2 - 30, 60, WHITE);
    }
    
    if (winnerId != -1) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));
        const char* txt = (winnerId == solo_localPlayerId) ? "VICTOIRE !" : TextFormat("%s A GAGNÉ !", soloState.players[winnerId].name);
        DrawText(txt, GetScreenWidth()/2 - MeasureText(txt, 80)/2, GetScreenHeight()/2 - 100, 80, winnerId == solo_localPlayerId ? GREEN : RED);
        DrawText("Appuyez sur ENTREE pour rejouer ou ESC pour menu", GetScreenWidth()/2 - 220, GetScreenHeight()/2 + 50, 20, GRAY);
        
        if (IsKeyPressed(KEY_ENTER)) {
            startSoloGame();
        }
    }
    
    // Info Icon
    Rectangle infoIconRect = { (float)GetScreenWidth() - 40, 40, 30, 30 };
    bool hoverInfo = CheckCollisionPointRec(GetMousePosition(), infoIconRect);
    DrawCircleV((Vector2){infoIconRect.x + 15, infoIconRect.y + 15}, 15, hoverInfo ? SKYBLUE : BLUE);
    DrawText("i", (int)infoIconRect.x + 11, (int)infoIconRect.y + 5, 25, WHITE);

    if (showInfo_window) {
        int sw = GetScreenWidth(); int sh = GetScreenHeight();
        Rectangle win = { sw/2.0f - 250, sh/2.0f - 200, 500, 400 };
        DrawRectangleRec(win, Fade(DARKGRAY, 0.95f));
        DrawRectangleLinesEx(win, 2, GOLD);
        DrawText("POUVOIRS DES CARTES", (int)win.x + 120, (int)win.y + 20, 25, GOLD);
        
        int ty = (int)win.y + 70;
        DrawText("- VALET (Jack) : Passe le tour du suivant", (int)win.x + 30, ty, 18, WHITE); ty += 40;
        DrawText("- DAME (Queen) : Inverse le sens de jeu", (int)win.x + 30, ty, 18, WHITE); ty += 40;
        DrawText("- 10 : Le suivant pioche 2 cartes et passe son tour", (int)win.x + 30, ty, 18, WHITE); ty += 40;
        DrawText("- ROI (King) : Change la couleur demandée", (int)win.x + 30, ty, 18, WHITE); ty += 40;
        DrawText("- ROI NOIR : Le suivant pioche 4 cartes !", (int)win.x + 30, ty, 18, WHITE); ty += 60;
        
        DrawText("Cliquez n'importe où pour fermer", (int)win.x + 100, (int)win.y + 360, 18, GRAY);
    }

    DrawText("ESC pour quitter", GetScreenWidth() - 150, 10, 15, GRAY);
}

void kingForFour_soloDestroy(void) {
    if (assets_loaded) {
        kingForFour_unloadAssets(assets);
        assets_loaded = false;
    }
}

GameClientInterface_St kingForFour_soloClientInterface = {
    .id = MINI_GAME_ID_KING_FOR_FOUR,
    .name = "King for Four (Solo)",
    .init = kingForFour_soloInit,
    .onData = NULL,
    .update = kingForFour_soloUpdate,
    .draw = kingForFour_soloDraw,
    .destroy = kingForFour_soloDestroy
};