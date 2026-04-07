/**
    @file bingoAPI.c
    @author Fshimi-Hawlk
    @date 2026-01-25
    @date 2026-03-31
    @brief Implementation of the Bingo mini-game API.
*/

#include "bingoAPI.h"
#include "utils/userTypes.h"
#include "utils/utils.h"
#include "utils/globals.h"
#include "core/game.h"
#include "ui/app.h"
#include "ui/game.h"
#include "logger.h"
#include "raymath.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Error_Et bingo_initGame__full(struct BingoGame_St** game, BingoConfigs_St configs) {
    if (game == NULL) return ERROR_NULL_POINTER;
    (void)configs;

    *game = (struct BingoGame_St*)calloc(1, sizeof(struct BingoGame_St));
    if (*game == NULL) return ERROR_ALLOC;

    struct BingoGame_St* g = *game;
    g->base.running = true;

    // Ball system
    g->balls.remainingCount = 500;
    uint b = 0;
    for (uint n = 0; n < 100; ++n) {
        for (uint col = 1; col <= 5; ++col) {
            g->balls.encodedBalls[b++] = 100 * col + n;
        }
    }
    shuffleArray(uint, g->balls.encodedBalls, 500, prng_rand);

    g->balls.choiceDelay = 3.5f;
    g->balls.showDelay   = 1.5f;
    g->balls.graceDelay  = 1.0f;

    g->currentCall.timer = g->balls.showDelay;
    g->currentCall.displayedText[0] = '\0';

    g->progress.scene = GAME_SCENE_CARD_CHOICE;
    memset(g->progress.resultMessage, 0, 64);

    bingo_computeLayout(&g->layout);

    log_info("Bingo initialized successfully");
    return OK;
}

Error_Et bingo_gameLoop(struct BingoGame_St* const game) {
    if (game == NULL) return ERROR_NULL_POINTER;

    f32 dt = GetFrameTime();
    Vector2 mousePos = GetMousePosition();

    switch (game->progress.scene) {
        case GAME_SCENE_CARD_CHOICE: {
            for (uint i = 0; i < 12; ++i) {
                CardUI_St* card = &game->layout.choiceCards[i];
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                    CheckCollisionPointRec(mousePos, card->backgroundRect)) {
                    if (game->previouslySelectedCard) game->previouslySelectedCard->selected = false;
                    game->previouslySelectedCard = card;
                    card->selected = true;
                    break;
                }
            }
            if (game->previouslySelectedCard && IsKeyPressed(KEY_ENTER)) {
                game->progress.scene = GAME_SCENE_LAUNCHING;
                game->currentCall.timer = 6.5f;
            }
        } break;

        case GAME_SCENE_LAUNCHING: {
            game->currentCall.timer -= dt;
            if (game->currentCall.timer <= 0.0f) {
                game->progress.scene = GAME_SCENE_PLAYING;
                game->currentCall.timer = game->balls.showDelay;
            }
        } break;

        case GAME_SCENE_PLAYING: {
            game->currentCall.timer += dt;
            if (game->currentCall.timer >= game->balls.choiceDelay) {
                game->currentCall.timer = 0.0f;
                if (game->balls.remainingCount > 0) {
                    game->currentCall.encodedValue = game->balls.encodedBalls[--game->balls.remainingCount];
                    game->currentCall.column = (game->currentCall.encodedValue / 100) - 1;
                    game->currentCall.number = game->currentCall.encodedValue - (game->currentCall.column + 1) * 100;
                    sprintf(game->currentCall.displayedText, "%s %u", LETTERS[game->currentCall.column], game->currentCall.number);
                }
            }
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                bool inGrace = (game->currentCall.timer <= (game->balls.showDelay + game->balls.graceDelay));
                bool handled = false;
                for (u8 r = 0; r < 5 && !handled; ++r) {
                    for (u8 c = 0; c < 5 && !handled; ++c) {
                        Rectangle rect = { game->layout.cardRectsRect.x + game->layout.cardRectsRect.width*c, game->layout.cardRectsRect.y + game->layout.cardRectsRect.height*r, game->layout.cardRectsRect.width-1, game->layout.cardRectsRect.height-1 };
                        if (CheckCollisionPointRec(mousePos, rect)) {
                            if (inGrace && bingo_isValidDaub(&game->currentCall, &game->player, r, c)) {
                                game->player.daubs[r][c] = true;
                                game->player.misclicks[r][c] = 0;
                            }
                            handled = true;
                        }
                    }
                }
            }
            if (bingo_hasBingo(&game->player)) {
                game->progress.scene = GAME_SCENE_END;
                strncpy(game->progress.resultMessage, "BINGO! You win!", 63);
            } else if (game->balls.remainingCount == 0) {
                game->progress.scene = GAME_SCENE_END;
                strncpy(game->progress.resultMessage, "No more balls - Game Over", 63);
            }
        } break;
        default: break;
    }

    // Rendering
    switch (game->progress.scene) {
        case GAME_SCENE_CARD_CHOICE: bingo_drawChoiceCards(&game->layout); break;
        case GAME_SCENE_LAUNCHING: {
            bingo_drawChoiceCards(&game->layout);
            char text[8]; sprintf(text, "%.0f", game->currentCall.timer / 2.0f);
            Vector2 sz = MeasureTextEx(bingo_fonts[FONT48], text, 128, 0);
            DrawTextEx(bingo_fonts[FONT48], text, Vector2Subtract((Vector2){game->layout.windowCenter.x, game->layout.windowCenter.y}, Vector2Scale(sz, 0.5f)), 128, 0, BLACK);
        } break;
        case GAME_SCENE_PLAYING: {
            bingo_drawCard(&game->layout, &game->player);
            bingo_drawUI(&game->layout, &game->balls, &game->currentCall);
        } break;
        case GAME_SCENE_END: {
            u32 w = MeasureText(game->progress.resultMessage, 64);
            DrawTextEx(bingo_fonts[FONT48], game->progress.resultMessage, (Vector2){game->layout.windowCenter.x - w/2.0f, game->layout.windowCenter.y - 32.0f}, 64, 0, (game->progress.resultMessage[0] == 'B' ? GREEN : RED));
        } break;
    }
    return OK;
}

Error_Et bingo_freeGame(struct BingoGame_St* game) {
    if (game) free(game);
    return OK;
}
