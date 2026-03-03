/**
    @file main.c
    @author Fshimi-Hawlk
    @date 2026-03-02
    @date 2026-03-03
    @brief One clear sentence that tells what this file is actually for.
    
    Contributors:
        - Fshimi-Hawlk:
            - Provided documentation start-up
            - Initial implementation
            - Refactored into structured state and helper functions

    @note Uses Raylib for rendering and input.
          Numbers 0-99 placed randomly without column restrictions.
          Calls use B/I/N/G/O format but do not enforce column ranges.
*/

#include "utils/utils.h"
#include "setups/app.h"

#define PLAYER_COUNT 1

static bool hasBingo(uint p, bool daubs[PLAYER_COUNT][5][5], uint misclicks[PLAYER_COUNT][5][5]) {
    // A square is "validly marked" only if daubed AND not penalized
    bool marked[5][5];
    for (u8 r = 0; r < 5; ++r) {
        for (u8 c = 0; c < 5; ++c) {
            marked[r][c] = daubs[p][r][c] && (misclicks[p][r][c] < 3);
        }
    }

    // Rows
    for (u8 r = 0; r < 5; ++r) {
        bool full = true;
        for (u8 c = 0; c < 5; ++c) {
            if (!marked[r][c]) { full = false; break; }
        }
        if (full) return true;
    }

    // Columns
    for (u8 c = 0; c < 5; ++c) {
        bool full = true;
        for (u8 r = 0; r < 5; ++r) {
            if (!marked[r][c]) { full = false; break; }
        }
        if (full) return true;
    }

    // Main diagonal (top-left → bottom-right)
    bool diag1 = true;
    for (u8 i = 0; i < 5; ++i) {
        if (!marked[i][i]) { diag1 = false; break; }
    }
    if (diag1) return true;

    // Anti-diagonal (top-right → bottom-left)
    bool diag2 = true;
    for (u8 i = 0; i < 5; ++i) {
        if (!marked[i][4 - i]) { diag2 = false; break; }
    }
    if (diag2) return true;

    return false;
}

/**
    @brief Program entry point.
    @return 0 on clean exit, non-zero on early failure
*/
int main(void) {
    // ── Initialization ───────────────────────────────────────────────────────
    if (!initApp()) {
        log_fatal("Application initialization failed. Aborting.");
        freeApp();
        return 1;
    }

    bool gameOver = false;
    const char *gameResult = NULL;

    uint playerCards[PLAYER_COUNT][5][5] = {0};
    bool daubs[PLAYER_COUNT][5][5] = {0};
    uint misclicks[PLAYER_COUNT][5][5] = {0};

    uint numbers[100] = {0};
    uint balls[500] = {0};
    for (uint n = 0, b = 0; n < 100; ++n) {
        numbers[n] = n;

        for (uint i = 1; i <= 5; ++i) {
            balls[b++] = 100 * i + n;
        }
    }

    shuffleArrayT(uint, balls, 500, rand);
    shuffleArrayT(uint, numbers, 100, rand);

    for (uint p = 0; p < PLAYER_COUNT; ++p) {
        daubs[p][2][2] = true;

        uint count = 100;
        for (uint i = 0; i < 25; ++i) {
            if (i == 12) continue; // free slot in the middle
            playerCards[p][i / 5][i % 5] = numbers[--count];
        }
    }

    f32Vector2 windowCenter = {
        .x = WINDOW_WIDTH / 2.0f, 
        .y = WINDOW_HEIGHT / 2.0f
    };
    
    u32Vector2 rectSize = {WINDOW_WIDTH / 15.0f, WINDOW_WIDTH / 15.0f};
    u32Vector2 rectCenteredOffset = {
        .x = windowCenter.x - rectSize.x / 2.0f, 
        .y = windowCenter.y + rectSize.y / 2.0f  
    };

    u32Vector2 rectStartPos = {
        .x = rectCenteredOffset.x - rectSize.x * 2,
        .y = rectCenteredOffset.y - rectSize.y * 2
    };

    const char *letters[5] = {"B", "I", "N", "G", "O"};
    
    f32 showDelay = 1.5f; // in seconds
    f32 graceTime = 1.0f; // in seconds
    f32 gracePeriod = showDelay + graceTime; // in seconds
    f32 choiceDelay = 3.5f; // in seconds
    f32 t = showDelay;
    
    uint ballCount = 500;
    char ballText[5] = "";
    uint selectedBall = 0;
    uint selectedCol = 0;
    uint selectedNum = 0;

    Rectangle boardRect = {
        .x = rectStartPos.x - 15, 
        .y = rectStartPos.y - rectSize.x * 1.05, 
        .width = rectSize.x * 5 + 30, 
        .height = rectSize.x * 6 + 15
    };

    Rectangle boardRectBorder = {
        .x = boardRect.x + 7.5,
        .y = boardRect.y + 7.5,
        .width = boardRect.width - 15,
        .height = boardRect.height - 15,
    };


    // ── Main loop ────────────────────────────────────────────────────────────
    while (!WindowShouldClose()) {

        f32Vector2 mousePos = GetMousePosition();

        t += GetFrameTime();
        DrawText(TextFormat("%f", t), 10, 10, 16, BLACK);

        if (t >= choiceDelay) {
            t = 0;
            
            selectedBall = balls[--ballCount];
            selectedCol = (selectedBall / 100) - 1;
            selectedNum = selectedBall - (selectedCol + 1) * 100;

            sprintf(ballText, "%s-%u", letters[selectedCol], selectedNum);
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !gameOver) {
            bool validGrace = (showDelay <= t && t <= gracePeriod);

            for (u8 r = 0; r < 5; ++r) {
                for (u8 c = 0; c < 5; ++c) {
                    Rectangle rect = {
                        .x = rectStartPos.x + rectSize.x * c,
                        .y = rectStartPos.y + rectSize.y * r,
                        .width = rectSize.x - 1,
                        .height = rectSize.y - 1
                    };

                    if (!CheckCollisionPointRec(mousePos, rect)) continue;

                    // ── Handle click ────────────────────────────────────────────────
                    if (validGrace) {
                        // Correct square during grace → daub
                        if (selectedCol == c && selectedNum == playerCards[0][r][c]) {
                            daubs[0][r][c] = true;
                            misclicks[0][r][c] = 0;  // reset if previously misclicked
                        }
                        // Wrong square during grace → penalty
                        else {
                            misclicks[0][r][c]++;
                        }
                    } else if (!daubs[0][r][c]) {
                        // Click outside grace period → always penalty
                        misclicks[0][r][c]++;
                    }

                    // Check penalty threshold
                    if (misclicks[0][r][c] >= 3) {
                        // Square is now lost (cannot be daubed anymore)
                        daubs[0][r][c] = false;
                    }

                    // Early exit after processing one square
                    goto next_frame;
                }
            }
            next_frame:;
        }

        // Check win after any potential change
        if (!gameOver && hasBingo(0, daubs, misclicks)) {
            gameOver = true;
            gameResult = "BINGO! You win!";
        } else if (!gameOver && ballCount == 0) {
            // check if no more possible win (very late game, all valid squares used but no line)
            gameOver = true;
            gameResult = "No more balls - Game Over";
        }

        BeginDrawing(); {
            ClearBackground(APP_BACKGROUND_COLOR);

            f32 roudness = 0.05;
            DrawRectangleRounded(boardRect, roudness, 16, WHITE);
            DrawRectangleRoundedLinesEx(boardRectBorder, roudness - 0.025, 16, 6.5, BROWN);

            for (uint p = 0; p < PLAYER_COUNT; ++p) {
                for (u8 r = 0; r < 5; ++r) {
                    for (u8 c = 0; c < 5; ++c) {
                        Rectangle rect = {
                            .x = rectStartPos.x + rectSize.x * c,
                            .y = rectStartPos.y + rectSize.y * r,
                            .width = rectSize.x - 1,
                            .height = rectSize.y - 1
                        };

                        DrawRectangleLinesEx(rect, 1, BLACK);

                        if (misclicks[0][r][c] > 0) {
                            Rectangle innerRect = {
                                .x = rect.x + 1,
                                .y = rect.y + 1,
                                .width = rect.width - 2,
                                .height = rect.height - 2,
                            };
                            switch (misclicks[0][r][c]) {
                                case 1: DrawRectangleRec(innerRect, YELLOW); break;
                                case 2: DrawRectangleRec(innerRect, ORANGE); break;
                                case 3:
                                default: {
                                    DrawRectangleRec(innerRect, RED);
                                    DrawText(
                                        "X", 
                                        innerRect.x + rectSize.x / 2.0f - 10,
                                        innerRect.y + rectSize.y / 2.0f - 16, 
                                        32, WHITE
                                    );
                                }
                            }
                        }

                        if (daubs[0][r][c]) {
                            DrawCircle(
                                rect.x + rectSize.x / 2.0f,
                                rect.y + rectSize.y / 2.0f,
                                rectSize.x / 2.0f - 5, 
                                GREEN
                            );
                        } else if (misclicks[0][r][c] < 3) {
                            u32 textHeight = 24;
                            const char *text = "Free";

                            if (r != 2 || c != 2) {
                                textHeight = 32;
                                text = TextFormat("%u", playerCards[0][r][c]);
                            }

                            u32 textWidth = MeasureText(text, textHeight);
                            u32Vector2 textPos = {
                                .x = rect.x + (rectSize.x - textWidth) / 2.0f,
                                .y = rect.y + (rectSize.y - textHeight) / 2.0f,
                            };
                            DrawText(text, textPos.x, textPos.y, textHeight, BLACK);
                        }
                    }
                }
    
                for (u8 i = 0; i < 5; ++i) {
                    u32 textWidth = MeasureText(letters[i], 32);
                    u32Vector2 textPos = {
                        .x = rectStartPos.x + rectSize.x * (i + 0.5) - textWidth / 2.0f, 
                        .y = rectStartPos.y - rectSize.y / 1.5
                    };

                    DrawText(letters[i], textPos.x, textPos.y, 32, BLACK);
                }
            }

            if (showDelay <= t && t <= gracePeriod) {
                f32 fontSize = 32;
                const char *text = TextFormat("%.2f", graceTime * (gracePeriod - t));
                uint textWidth = MeasureText(text, fontSize);
                DrawText(
                    text, 
                    windowCenter.x - textWidth / 2.0f, 
                    boardRect.y + boardRect.height + fontSize * 1.5, 
                    fontSize, BLACK
                );
            }

            if (t <= showDelay) {
                f32 fontSize = 48;
                uint textWidth = MeasureText(ballText, fontSize);
                DrawText(
                    ballText, 
                    windowCenter.x - textWidth / 2.0f, 
                    boardRect.y - fontSize * 1.5, 
                    fontSize, Fade(BLACK, 1 - (t / showDelay))
                );
            }

            if (gameOver) {
                DrawText(gameResult, windowCenter.x, windowCenter.y, 64, RED);
            }

        } EndDrawing();
    }

    // ── Cleanup ──────────────────────────────────────────────────────────────
    freeApp();

    return 0;
}

// Required when logger.h is used
#define LOGGER_IMPLEMENTATION
#include "logger.h"

// Required when contextArena.h is used for custom allocators
// #define CONTEXT_ARENA_IMPLEMENTATION
// #include "contextArena.h"