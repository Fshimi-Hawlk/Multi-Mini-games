/**
    @file core/game.c
    @author Fshimi-Hawlk
    @date 2026-03-02
    @date 2026-03-05
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

#include "utils/globals.h"

static bool isValidDaub(const CallState_St* const state, const PlayerCard_St* playerCard, uint row, uint col) {
    return (state->column == col) 
        && (state->number == playerCard->numbers[row][col]);
}

static bool hasBingo(const PlayerCard_St* const playerCard) {
    bool validMarked[5][5] = {0};
    for (u8 r = 0; r < 5; ++r) {
        for (u8 c = 0; c < 5; ++c) {
            validMarked[r][c] = playerCard->daubs[r][c] 
                             && (playerCard->misclicks[r][c] < 3);
        }
    }

    // Rows
    for (u8 r = 0; r < 5; ++r) {
        bool full = true;
        for (u8 c = 0; c < 5; ++c) {
            if (!validMarked[r][c]) { full = false; break; }
        }
        if (full) return true;
    }

    // Columns (symmetric to rows)

    for (u8 c = 0; c < 5; ++c) {
        bool full = true;
        for (u8 r = 0; r < 5; ++r) {
            if (!validMarked[r][c]) { full = false; break; }
        }
        if (full) return true;
    }

    // Diagonals
    bool d1 = true, d2 = true;
    for (u8 i = 0; i < 5; ++i) {
        if (!validMarked[i][i])     d1 = false;
        if (!validMarked[i][4 - i]) d2 = false;
    }
    return d1 || d2;
}

void bingo_updateGame(BingoGame_St* const game, f32 dt, f32Vector2 mousePos) {
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
                game->currentCall.timer = SHOW_DELAY + GRACE_TIME;

                memcpy(game->player.numbers, game->previouslySelectedCard->values, sizeof(Card_t));
            }
        } break;

        case GAME_SCENE_PLAYING: {
            game->currentCall.timer += dt;

            // New ball
            if (game->currentCall.timer >= CHOICE_DELAY) {
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
                bool inGrace = (SHOW_DELAY <= game->currentCall.timer && game->currentCall.timer <= (SHOW_DELAY + GRACE_TIME));

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

                        if (inGrace && isValidDaub(&game->currentCall, &game->player, r, c)) {
                            game->player.daubs[r][c] = true;
                            game->player.misclicks[r][c] = 0;
                        } else {
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
            if (hasBingo(&game->player)) {
                game->progress.scene         = GAME_SCENE_END;
                game->progress.resultMessage = "BINGO! You win!";
            } else if (game->balls.remainingCount == 0) {
                game->progress.scene         = GAME_SCENE_END;
                game->progress.resultMessage = "No more balls - Game Over";
            }
        } break;

        case GAME_SCENE_END: return;
        default: {
            log_error("GameScene_Et");
            return;
        }
    }
}