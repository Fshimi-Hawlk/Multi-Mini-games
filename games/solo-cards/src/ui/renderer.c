/**
    @file renderer.c
    @author Maxime CHAUVEAU
    @date February 2026
    @date 2026-04-14
    @brief Solitaire rendering functions
*/
#include "ui/renderer.h"
#include "assetPath.h"
#include <string.h>
#include <stdio.h>
#include "raylib.h"
#include "solitaire.h"
#include "raymath.h"
#include "utils/audio.h"

/* Define ASSET_PATH if not set by Makefile */
#ifndef ASSET_PATH
#define ASSET_PATH ""
#endif

#define CARD_TEX_WIDTH 140
#define CARD_TEX_HEIGHT 190
#define WASTE_SPREAD_WIDTH 150

static const CardRegion_St card_atlas[] = {
    { "cardHeartsA.png",   140, 1330, 140, 190 },
    { "cardHearts2.png",   700,  380, 140, 190 },
    { "cardHearts3.png",   280,  950, 140, 190 },
    { "cardHearts4.png",   280,  760, 140, 190 },
    { "cardHearts5.png",   280,  570, 140, 190 },
    { "cardHearts6.png",   280,  380, 140, 190 },
    { "cardHearts7.png",   280,  190, 140, 190 },
    { "cardHearts8.png",   280,    0, 140, 190 },
    { "cardHearts9.png",   140, 1710, 140, 190 },
    { "cardHearts10.png",  140, 1520, 140, 190 },
    { "cardHeartsJ.png",   140, 1140, 140, 190 },
    { "cardHeartsQ.png",   140,  760, 140, 190 },
    { "cardHeartsK.png",   140,  950, 140, 190 },

    { "cardDiamondsA.png",   420,    0, 140, 190 },
    { "cardDiamonds2.png",   420, 1710, 140, 190 },
    { "cardDiamonds3.png",   420, 1520, 140, 190 },
    { "cardDiamonds4.png",   420, 1330, 140, 190 },
    { "cardDiamonds5.png",   420, 1140, 140, 190 },
    { "cardDiamonds6.png",   420,  950, 140, 190 },
    { "cardDiamonds7.png",   420,  760, 140, 190 },
    { "cardDiamonds8.png",   420,  570, 140, 190 },
    { "cardDiamonds9.png",   420,  380, 140, 190 },
    { "cardDiamonds10.png",  420,  190, 140, 190 },
    { "cardDiamondsJ.png",   280, 1710, 140, 190 },
    { "cardDiamondsQ.png",   280, 1330, 140, 190 },
    { "cardDiamondsK.png",   280, 1520, 140, 190 },

    { "cardClubsA.png",   560,  570, 140, 190 },
    { "cardClubs2.png",   280, 1140, 140, 190 },
    { "cardClubs3.png",   700,  190, 140, 190 },
    { "cardClubs4.png",   700,    0, 140, 190 },
    { "cardClubs5.png",   560, 1710, 140, 190 },
    { "cardClubs6.png",   560, 1520, 140, 190 },
    { "cardClubs7.png",   560, 1330, 140, 190 },
    { "cardClubs8.png",   560, 1140, 140, 190 },
    { "cardClubs9.png",   560,  950, 140, 190 },
    { "cardClubs10.png",  560,  760, 140, 190 },
    { "cardClubsJ.png",   560,  380, 140, 190 },
    { "cardClubsQ.png",   560,    0, 140, 190 },
    { "cardClubsK.png",   560,  190, 140, 190 },

    { "cardSpadesA.png",     0,  570, 140, 190 },
    { "cardSpades2.png",   140,  380, 140, 190 },
    { "cardSpades3.png",   140,  190, 140, 190 },
    { "cardSpades4.png",   140,    0, 140, 190 },
    { "cardSpades5.png",     0, 1710, 140, 190 },
    { "cardSpades6.png",     0, 1520, 140, 190 },
    { "cardSpades7.png",     0, 1330, 140, 190 },
    { "cardSpades8.png",     0, 1140, 140, 190 },
    { "cardSpades9.png",     0,  950, 140, 190 },
    { "cardSpades10.png",    0,  760, 140, 190 },
    { "cardSpadesJ.png",     0,  380, 140, 190 },
    { "cardSpadesQ.png",     0,    0, 140, 190 },
    { "cardSpadesK.png",     0,  190, 140, 190 },
};

const int CARD_COUNT = (sizeof(card_atlas) / sizeof(CardRegion_St));

GameAssets_St solitaire_LoadAssets(void) {
    GameAssets_St assets;

    /* Asset path resolution — ASSET_PATH is set by Makefile (e.g. "jeux/solitaire/assets/") */
    static const char* baseCandidates[] = {
        ASSET_PATH "solitaire/",    /* "jeux/solitaire/assets/solitaire/" from project root */
        "jeux/solitaire/assets/solitaire/",  /* explicit fallback */
        "assets/solitaire/",
        NULL
    };

    char basePath[512];
    findAssetBase("playingCards.png", baseCandidates, basePath, sizeof(basePath));

    char cardSheetPath[600], cardBackPath[600];
    snprintf(cardSheetPath, sizeof(cardSheetPath), "%splayingCards.png",   basePath);
    snprintf(cardBackPath,  sizeof(cardBackPath),  "%scardBack_blue3.png", basePath);

    assets.cardSheet = LoadTexture(cardSheetPath);
    assets.cardBack  = LoadTexture(cardBackPath);

    assets.menuBackground = (Texture2D){0};

    if (assets.cardSheet.id == 0) printf("ERREUR CRITIQUE: Texture playingCards.png introuvable !\n");
    if (assets.cardBack.id == 0)  printf("ERREUR CRITIQUE: Texture cardBack_blue3.png introuvable !\n");

    return assets;
}

void solitaire_UnloadAssets(GameAssets_St assets) {
    UnloadTexture(assets.cardSheet);
    UnloadTexture(assets.cardBack);
}

static void DrawCardFromAtlasTinted(GameAssets_St assets, int index, Vector2 position, Color tint) {
    if (index >= 0 && index < CARD_COUNT) {
        CardRegion_St region = card_atlas[index];
        Rectangle source = {region.x, region.y, region.width, region.height};
        Rectangle dest = {position.x, position.y, CARD_WIDTH, CARD_HEIGHT};
        Vector2 origin = {0, 0};
        DrawTexturePro(assets.cardSheet, source, dest, origin, 0.0f, tint);
    }
}

static void DrawCardFromAtlas(GameAssets_St assets, int index, Vector2 position, float scale) {
    (void)scale;
    DrawCardFromAtlasTinted(assets, index, position, WHITE);
}

static void DrawCardBack(GameAssets_St assets, Vector2 position) {
    Rectangle source = {0, 0, assets.cardBack.width, assets.cardBack.height};
    Rectangle dest = {position.x, position.y, CARD_WIDTH, CARD_HEIGHT};
    Vector2 origin = {0, 0};
    DrawTexturePro(assets.cardBack, source, dest, origin, 0.0f, WHITE);
}

static void DrawEmptySlot(Vector2 position, Color borderColor) {
    DrawRectangleRounded((Rectangle){position.x, position.y, CARD_WIDTH, CARD_HEIGHT}, 0.1f, 8, (Color){30, 60, 30, 255});
    DrawRectangleRoundedLines((Rectangle){position.x, position.y, CARD_WIDTH, CARD_HEIGHT}, 0.1f, 8, borderColor);
}

static void RenderSingleCard(const Card_St *card, GameAssets_St assets, Vector2 position) {
    if (card->isFaceUp) {
        int index = card->suit * NUM_RANKS + (card->rank - 1);
        DrawCardFromAtlas(assets, index, position, 1.0f);
    } else {
        DrawCardBack(assets, position);
    }
}

static void DrawEmptySlotWide(Vector2 position, int width, Color borderColor) {
    DrawRectangleRounded((Rectangle){position.x, position.y, width, CARD_HEIGHT}, 0.1f, 8, (Color){30, 60, 30, 255});
    DrawRectangleRoundedLines((Rectangle){position.x, position.y, width, CARD_HEIGHT}, 0.1f, 8, borderColor);
}

static void RenderStock(const Pile_St *stock, GameAssets_St assets) {
    DrawEmptySlot(stock->position, (Color){100, 150, 100, 255});
    
    if (stock->count > 0) {
        DrawCardBack(assets, stock->position);
    }
}

static void RenderWaste(const Pile_St *waste, GameAssets_St assets, bool isDragSource) {
    DrawEmptySlotWide(waste->position, WASTE_ZONE_WIDTH, (Color){100, 150, 100, 255});
    
    if (waste->count > 0) {
        int spread = 25;
        int maxVisible = 3;
        int start = waste->count > maxVisible ? waste->count - maxVisible : 0;
        int end = isDragSource ? waste->count - 1 : waste->count;
        float offsetX = 0;
        for (int i = start; i < end; i++) {
            Vector2 pos = {waste->position.x + offsetX, waste->position.y};
            RenderSingleCard(waste->cards[i], assets, pos);
            offsetX += spread;
        }
    }
}

static void RenderFoundation(const Pile_St *foundation, GameAssets_St assets, int index) {
    DrawEmptySlot(foundation->position, (Color){200, 180, 100, 255});
    
    int aceIndices[] = {0, 13, 26, 39};
    Color tints[] = {
        {150, 80, 80, 200},
        {150, 80, 80, 200},
        {80, 80, 80, 200},
        {80, 80, 80, 200}
    };
    
    if (foundation->count == 0) {
        DrawCardFromAtlasTinted(assets, aceIndices[index], foundation->position, tints[index]);
    } else {
        RenderSingleCard(foundation->cards[foundation->count - 1], assets, foundation->position);
    }
}

static void RenderTableauPile(const Pile_St *pile, GameAssets_St assets, bool isDragSource, int dragIndex, int dragCount) {
    DrawEmptySlot(pile->position, (Color){100, 150, 100, 255});
    
    for (int i = 0; i < pile->count; i++) {
        if (isDragSource && i >= dragIndex && i < dragIndex + dragCount) continue;
        Vector2 pos = {
            pile->position.x,
            pile->position.y + i * CARD_OFFSET_Y
        };
        RenderSingleCard(pile->cards[i], assets, pos);
    }
}

void RenderGame(const SolitaireGameState_St *game, GameAssets_St assets) {
    ClearBackground((Color){34, 85, 51, 255});

    bool isDraggingFromWaste = game->dragState.isDragging && game->dragState.sourcePile == &game->waste;
    
    RenderStock(&game->stock, assets);
    RenderWaste(&game->waste, assets, isDraggingFromWaste);
    
    for (int i = 0; i < NUM_FOUNDATION_PILES; i++) {
        RenderFoundation(&game->foundation[i], assets, i);
    }
    
    for (int i = 0; i < NUM_TABLEAU_PILES; i++) {
        bool isDragSource = game->dragState.isDragging && game->dragState.sourcePile == &game->tableau[i];
        RenderTableauPile(&game->tableau[i], assets, isDragSource, game->dragState.sourceIndex, game->dragState.count);
    }
    
    if (game->dragState.isDragging) {
        Vector2 mousePos = GetMousePosition();
        Vector2 dragPos = Vector2Subtract(mousePos, (Vector2){CARD_WIDTH / 2.0f, CARD_HEIGHT / 2.0f});
        for (int i = 0; i < game->dragState.count; i++) {
            Vector2 cardPos = {dragPos.x, dragPos.y + i * CARD_OFFSET_Y};
            RenderSingleCard(game->dragState.cards[i], assets, cardPos);
        }
    }
    
    /* FIX: buffer was 64 bytes but the format string alone is 50 chars;
     * adding a large score or elapsed time could overflow. Increased to 96
     * and switched to snprintf for safety. */
    char scoreText[96];
    snprintf(scoreText, sizeof(scoreText),
             "Score: %d  |  Temps: %.0fs  |  N: Nouvelle partie",
             game->score, game->gameTime);
    DrawText(scoreText, 10, SCREEN_HEIGHT - 30, 20, WHITE);
}

void solitaire_RenderMenu(const SolitaireGameState_St *game, GameAssets_St assets) {
    (void)assets;
    /* FIX: removed ClearBackground — it was called inside BeginDrawing after
     * RenderGame already drew all the cards, wiping the entire framebuffer and
     * leaving only a plain green screen. The background is already cleared in
     * solitaireAPI.c before solitaire_draw() is called. */
    
    if (game->isWon) {
        DrawRectangle(SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 - 60, 400, 120, (Color){0, 100, 0, 200});
        DrawText("FELICITATIONS !", SCREEN_WIDTH/2 - 90, SCREEN_HEIGHT/2 - 40, 30, GOLD);
        char winText[64];
        snprintf(winText, sizeof(winText), "Score: %d  Temps: %.0fs", game->score, game->gameTime);
        DrawText(winText, SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2, 20, WHITE);
        DrawText("Appuyez sur N pour rejouer", SCREEN_WIDTH/2 - 110, SCREEN_HEIGHT/2 + 30, 18, LIGHTGRAY);
    } else if (game->isLost) {
        DrawRectangle(SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 - 60, 400, 120, (Color){100, 0, 0, 200});
        DrawText("PERDU !", SCREEN_WIDTH/2 - 60, SCREEN_HEIGHT/2 - 40, 30, RED);
        char loseText[64];
        sprintf(loseText, "Score: %d  Temps: %.0fs", game->score, game->gameTime);
        DrawText(loseText, SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2, 20, WHITE);
        DrawText("Appuyez sur N pour rejouer", SCREEN_WIDTH/2 - 110, SCREEN_HEIGHT/2 + 30, 18, LIGHTGRAY);
    }
}

void RenderCard(const Card_St *card, GameAssets_St assets, Vector2 position) {
    RenderSingleCard(card, assets, position);
}

void RenderPile(const Pile_St *pile, GameAssets_St assets) {
    for (int i = 0; i < pile->count; i++) {
        Vector2 pos = {
            pile->position.x,
            pile->position.y + i * CARD_OFFSET_Y
        };
        RenderSingleCard(pile->cards[i], assets, pos);
    }
}
