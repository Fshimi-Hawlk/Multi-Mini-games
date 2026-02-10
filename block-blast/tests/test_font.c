/**
 * @file test_font.c
 * @brief Unit tests for font loading and text utilities.
 */

#include "utils/globals.h"

#include "core/game/game.h"

#include "setups/app.h"

#include <assert.h>

static void test_font_loading(void) {
    assert(initFonts() == true);
    log_info("OK");

    for (FontSize_Et fs = FONT8; fs < _fontSizeCount; ++fs) {
        assert(IsFontValid(fonts[fs]));
    }
    log_info("OK");
    freeFonts();
}

static void test_score_text_generation(void) {
    GameState_St testGame = {
        .score = 1234,
    };

    buildScoreRelatedTexts(&testGame);
    assert(strcmp(testGame.scoreText, "Score: 1234") == 0);
    log_info("OK");
}

int main(void) {
    SetTraceLogLevel(LOG_WARNING);
    test_font_loading();
    test_score_text_generation();
    log_info("Font tests passed");
    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"

#define CONTEXT_ARENA_IMPLEMENTATION
#include "contextArena.h"

#define RAND_IMPLEMENTATION
#include "rand.h"