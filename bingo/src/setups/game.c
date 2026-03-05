/**
    @file game.h (setups)
    @author Fshimi Hawlk
    @date 2026-03-02
    @date 2026-03-05
    @brief Full game state initialization implementation.
*/

#include "setups/game.h"

#include "utils/globals.h"
#include "utils/utils.h"

static void computeLayout(Layout_St* layout) {
    layout->windowCenter = (f32Vector2) {
        WINDOW_WIDTH / 2.0f,
        WINDOW_HEIGHT / 2.0f
    };

    // Main card (play area) - unchanged from your original
    f32 mainCell = WINDOW_WIDTH / 15.0f;
    layout->cardRectsRect = (Rectangle) {
        .width  = mainCell,
        .height = mainCell
    };

    f32Vector2 offset = {
        layout->windowCenter.x - mainCell / 2.0f,
        layout->windowCenter.y + mainCell / 2.0f
    };

    layout->cardRectsRect.x = offset.x - mainCell * 2;
    layout->cardRectsRect.y = offset.y - mainCell * 2;

    layout->cardRect = (Rectangle) {
        layout->cardRectsRect.x - 15,
        layout->cardRectsRect.y - mainCell * 1.05f,
        mainCell * 5 + 30,
        mainCell * 6 + 15
    };

    layout->cardRectBorder = (Rectangle) {
        layout->cardRect.x + 7.5f,
        layout->cardRect.y + 7.5f,
        layout->cardRect.width - 15,
        layout->cardRect.height - 15
    };

    // ── Choice cards (precomputed grid) ─────────────────────────────────────
    const f32 scale  = 0.75f;
    const f32 margin = 45.0f;
    const uint rows  = 3;
    const uint cols  = 4;

    // Prototype small card size
    layout->choiceCardProto.width  = layout->cardRect.width * scale / 1.75;
    layout->choiceCardProto.height = layout->cardRect.height * scale / 1.75;

    // f32 cellW = layout->choiceCardProto.width / 5.0f;
    // f32 cellH = layout->choiceCardProto.height / 5.0f;

    // Total grid dimensions
    f32 totalW = cols * (layout->choiceCardProto.width + margin) - margin;
    f32 totalH = rows * (layout->choiceCardProto.height + margin) - margin;

    f32 startX = layout->windowCenter.x - totalW / 2.0f;
    f32 startY = layout->windowCenter.y - totalH / 2.0f + 25.0f;

    for (uint idx = 0; idx < 12; ++idx) {
        uint r = idx / cols;
        uint c = idx % cols;

        f32 pad = 12.0f * scale;

        // Card content area
        layout->choiceCards[idx].innerRect = (Rectangle) {
            startX + c * (layout->choiceCardProto.width + margin),
            startY + r * (layout->choiceCardProto.height + margin),
            layout->choiceCardProto.width,
            layout->choiceCardProto.height
        };

        // Border (slightly larger)
        layout->choiceCards[idx].bordersRect = (Rectangle) {
            layout->choiceCards[idx].innerRect.x - pad,
            layout->choiceCards[idx].innerRect.y - pad,
            layout->choiceCards[idx].innerRect.width + pad * 2,
            layout->choiceCards[idx].innerRect.height + pad * 2
        };

        pad = 5 * scale;

        layout->choiceCards[idx].backgroundRect = (Rectangle) {
            layout->choiceCards[idx].bordersRect.x - pad,
            layout->choiceCards[idx].bordersRect.y - pad,
            layout->choiceCards[idx].bordersRect.width + pad * 2,
            layout->choiceCards[idx].bordersRect.height + pad * 2,
        };
    }
}

bool generateCard(Card_t card, uint *available, uint count) {
    if (available == NULL || count == 0) return false;
    shuffleArrayT(uint, available, count, rand);
    
    uint idx = 0;

    for (uint r = 0; r < 5; ++r) {
        for (uint c = 0; c < 5; ++c) {
            if (r == 2 && c == 2) continue;
            card[r][c] = available[idx++];
        }
    }

    return true;
}

void bingo_initGame(BingoGame_St* game) {
    // ── 1. Player card ──────────────────────────────────────────────────────
    memset(game->player.numbers,   0, sizeof(game->player.numbers));
    memset(game->player.daubs,     0, sizeof(game->player.daubs));
    memset(game->player.misclicks, 0, sizeof(game->player.misclicks));

    // Free space (center)
    game->player.daubs[2][2] = true;
    // Optional: mark free space number as sentinel so it never matches calls
    game->player.numbers[2][2] = UINT32_MAX;  // or keep 0 and rely on daubs check

    // ── 2. Fill card numbers (random from 0..99 without replacement) ────────
    uint available[100];
    for (uint i = 0; i < 100; ++i) {
        available[i] = i;
    }
    
    for (uint i = 0; i < 12; ++i) {
        generateCard(game->layout.choiceCards[i].values, available, 100);
    }

    // generateCard(game->player.numbers, available, 100);

    // ── 3. Ball system ──────────────────────────────────────────────────────
    game->balls.remainingCount = 500;

    uint b = 0;
    for (uint n = 0; n < 100; ++n) {
        for (uint col = 1; col <= 5; ++col) {
            game->balls.encodedBalls[b++] = 100 * col + n;
        }
    }
    shuffleArrayT(uint, game->balls.encodedBalls, 500, rand);


    // ── 4. Current call ─────────────────────────────────────────────────────
    game->currentCall.encodedValue = 0;
    game->currentCall.column       = 0;
    game->currentCall.number       = 0;
    game->currentCall.timer        = SHOW_DELAY;
    game->currentCall.displayedText[0] = '\0';


    // ── 5. Game progress ────────────────────────────────────────────────────
    game->progress.scene         = GAME_SCENE_CARD_CHOICE;
    game->progress.resultMessage = NULL;

    // ── 6. Game layout ────────────────────────────────────────────────────
    computeLayout(&game->layout);
}