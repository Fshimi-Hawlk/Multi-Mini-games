/**
    @file bingoAPI.c
    @author Fshimi-Hawlk
    @date 2026-01-25
    @date 2026-03-19
    @brief One clear sentence that tells what this file is actually for.
  
    Contributors:
        - Fshimi-Hawlk:
            - Provided documentation start-up
        - <Name>:
            - What you added / changed / fixed (keep it short)
  
    If the file needs more context than fits in @brief, write 2-5 lines here.
    @note Put warnings, important limitations, "we know it's ugly but...", or future plans here

    // Try to align the `for` for better readability
    // Try to keep the same order of the includes
    Use @see `path/to/related/file.h` when this file depends heavily on another one.
*/

#include "core/game.h"

#include "setups/game.h"

#include "ui/app.h"
#include "ui/game.h"

#include "utils/utils.h"
#include "utils/globals.h"

#include "APIs/generalAPI.h"
#include "bingoAPI.h"

// ─────────────────────────────────────────────────────────────────────────────
// Private internal structure definition
// Only visible inside this translation unit
// ─────────────────────────────────────────────────────────────────────────────

/**
    @brief Core mutable game data – split into logical sub-structures.
*/
struct BingoGame_St{
    BaseGame_St base;
    CardUI_St*      previouslySelectedCard;
    PlayerCard_St   player;
    BallSystem_St   balls;
    CallState_St    currentCall;
    GameProgress_St progress;
    Layout_St       layout;
};

// ─────────────────────────────────────────────────────────────────────────────
// Implementation
// ─────────────────────────────────────────────────────────────────────────────

/// @note: that wrapper serve as an interface to avoid any annoying warning
Error_Et bingo_freeGameWrapper(void* game) {
    return bingo_freeGame((BingoGame_St**) game);
}

Error_Et bingo_initGame__full(BingoGame_St** gameRef, BingoConfigs_St configs) {
    UNUSED(configs);

    if (gameRef == NULL) {
        log_error("NULL gameRef double-pointer passed to init");
        return ERROR_NULL_POINTER;
    }

    *gameRef = calloc(1, sizeof(**gameRef));
    if (*gameRef == NULL) {
        log_error("Failed to allocate BingoGame_St");
        return ERROR_ALLOC;
    }

    // Have a game reference to avoid constant deferencement
    BingoGame_St* game = *gameRef;

    // Initialize common base fields
    game->base.running = true;

    // Player card
    memset(game->player.numbers,   0, sizeof(game->player.numbers));
    memset(game->player.daubs,     0, sizeof(game->player.daubs));
    memset(game->player.misclicks, 0, sizeof(game->player.misclicks));

    // Free space (center)
    game->player.daubs[2][2] = true;
    game->player.numbers[2][2] = UINT32_MAX;  // or keep 0 and rely on daubs check

    // Fill card numbers (random from 0..99 without replacement)
    uint available[100];
    for (uint i = 0; i < 100; ++i) {
        available[i] = i;
    }
    
    for (uint i = 0; i < 12; ++i) {
        bingo_generateCard(game->layout.choiceCards[i].values, available, 100);
    }

    // generateCard(game->player.numbers, available, 100);

    // Ball system
    game->balls.remainingCount = 500;

    uint b = 0;
    for (uint n = 0; n < 100; ++n) {
        for (uint col = 1; col <= 5; ++col) {
            game->balls.encodedBalls[b++] = 100 * col + n;
        }
    }
    shuffleArrayT(uint, game->balls.encodedBalls, 500, rand);

    game->balls.choiceDelay = 3.5f;
    game->balls.showDelay = 1.5f;
    game->balls.graceDelay = 1.0f;

    // Current call
    game->currentCall.encodedValue = 0;
    game->currentCall.column       = 0;
    game->currentCall.number       = 0;
    game->currentCall.timer        = game->balls.showDelay;
    game->currentCall.displayedText[0] = '\0';


    // Game progress
    game->progress.scene         = GAME_SCENE_CARD_CHOICE;
    game->progress.resultMessage = NULL;

    // Game layout
    bingo_computeLayout(&game->layout);

    log_info("Bingo initialized successfully");
    return OK;
}



Error_Et bingo_gameLoop(BingoGame_St* const game) {
    if (game == NULL) {
        log_debug("NULL game pointer in gameLoop");
        return ERROR_NULL_POINTER;
    }

    if (!game->base.running) {
        return OK;
    }

    f32Vector2 mousePos = GetMousePosition();
    f32 dt = GetFrameTime();

    switch (game->progress.scene) {
        case GAME_SCENE_CARD_CHOICE: {
            for (uint i = 0; i < 12; ++i) {
                CardUI_St* card = &game->layout.choiceCards[i];
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePos, card->backgroundRect)) {
                    if (game->previouslySelectedCard) game->previouslySelectedCard->selected = false;
                    game->previouslySelectedCard = card;
                    card->selected = true;
                    break;
                }
            }

            if (game->previouslySelectedCard != NULL && IsKeyPressed(KEY_ENTER)) {
                game->progress.scene = GAME_SCENE_LAUNCHING;
                game->currentCall.timer = 6.5;
            }
        } break;

        case GAME_SCENE_LAUNCHING: {
            game->currentCall.timer -= dt;

            if (game->currentCall.timer <= 2) {
                game->progress.scene = GAME_SCENE_PLAYING;
                game->currentCall.timer = game->balls.showDelay + game->balls.graceDelay;

                memcpy(game->player.numbers, game->previouslySelectedCard->values, sizeof(Card_t));
            }
        } break;

        case GAME_SCENE_PLAYING: {
            game->currentCall.timer += dt;

            // New ball
            if (game->currentCall.timer >= game->balls.choiceDelay) {
                game->currentCall.timer = 0.0f;
                if (game->balls.remainingCount > 0) {
                    game->currentCall.encodedValue = game->balls.encodedBalls[--game->balls.remainingCount];
                    game->currentCall.column  = (game->currentCall.encodedValue / 100) - 1;
                    game->currentCall.number  = game->currentCall.encodedValue - (game->currentCall.column + 1) * 100;
                    sprintf(game->currentCall.displayedText, "%s %u", LETTERS[game->currentCall.column], game->currentCall.number);
                }
            }

            // Input
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                bool inGrace = (game->currentCall.timer <= (game->balls.showDelay + game->balls.graceDelay));

                bool handled = false;
                for (u8 r = 0; r < 5 && !handled; ++r) {
                    for (u8 c = 0; c < 5 && !handled; ++c) {
                        Rectangle rect = {
                            game->layout.cardRectsRect.x + game->layout.cardRectsRect.width * c,
                            game->layout.cardRectsRect.y + game->layout.cardRectsRect.height * r,
                            game->layout.cardRectsRect.width - 1,
                            game->layout.cardRectsRect.height - 1
                        };

                        if (!CheckCollisionPointRec(mousePos, rect)) continue;

                        if (inGrace && bingo_isValidDaub(&game->currentCall, &game->player, r, c)) {
                            game->player.daubs[r][c] = true;
                            game->player.misclicks[r][c] = 0;
                        } else if (!game->player.daubs[r][c]) {
                            game->player.misclicks[r][c]++;
                            if (game->player.misclicks[r][c] >= 3) {
                                game->player.daubs[r][c] = false;
                            }
                        }
                        handled = true;
                    }
                }
            }

            // Win check
            if (bingo_hasBingo(&game->player)) {
                game->progress.scene         = GAME_SCENE_END;
                game->progress.resultMessage = "BINGO! You win!";
            } else if (game->balls.remainingCount == 0) {
                game->progress.scene         = GAME_SCENE_END;
                game->progress.resultMessage = "No more balls - Game Over";
            }
        } break;

        case GAME_SCENE_END: return OK;

        default: {
            log_error("GameScene_Et");
            return ERROR_INVALID_ENUM_VAL;
        }
    }

    BeginDrawing(); {
        ClearBackground(APP_BACKGROUND_COLOR);

        switch (game->progress.scene) {
            case GAME_SCENE_CARD_CHOICE: {
                bingo_drawChoiceCards(&game->layout);
            } break;

            case GAME_SCENE_LAUNCHING: {
                bingo_drawChoiceCards(&game->layout);

                char text[2] = {0};
                sprintf(text, "%.0f", game->currentCall.timer / 2);
                f32Vector2 textSize = MeasureTextEx(bingo_fonts[FONT48], text, 128, 0);

                DrawTextEx(
                    bingo_fonts[FONT48], text,
                    Vector2Subtract(game->layout.windowCenter, Vector2Scale(textSize, 0.5)), 
                    128, 0, BLACK
                );
            } break;

            case GAME_SCENE_PLAYING: {
                bingo_drawCard(&game->layout, &game->player);
                bingo_drawUI(&game->layout, &game->balls, &game->currentCall);
            } break;

            case GAME_SCENE_END: {
                f32 fontSize = 64;
                u32 w = MeasureText(game->progress.resultMessage, fontSize);
                Color col = (game->progress.resultMessage[0] == 'B') ? GREEN : RED;

                f32Vector2 textPos = {
                    .x = game->layout.windowCenter.x - w / 2.0f,
                    .y = game->layout.windowCenter.y - fontSize / 2.0f,
                };

                DrawTextEx(bingo_fonts[FONT48], game->progress.resultMessage, textPos, fontSize, 0, col);
            } break;
        }

        // Debug timer (remove later)
        // DrawText(TextFormat("%.2f", game->currentCall.timer), 10, 10, 16, BLACK);
    } EndDrawing();

    return OK;
}

Error_Et bingo_freeGame(BingoGame_St** game) {
    if (game == NULL || *game == NULL) {
        return ERROR_NULL_POINTER;
    }

    BingoGame_St* gameRef = *game;

    // Unload game-specific resources
    // e.g. UnloadTexture(gameRef->playerSprite);

    free(gameRef);
    *game = NULL;

    log_debug("Bingo resources freed");
    return OK;
}