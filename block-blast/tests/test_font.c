/**
 * @file test_font.c
 * @brief Unit tests for font loading and text utilities.
 */

#include "utils/globals.h"

#include "core/game/game.h"

#include "setups/app.h"

#include "ui/game.h"
#include "utils/userTypes.h"

#include <assert.h>

static void test_font_loading(void) {
    assert(initFonts() == true);
    for (FontSize_Et fs = FONT8; fs < _fontSizeCount; ++fs) {
        assert(IsFontValid(fonts[fs]));
    }
    freeFonts();
}

static void test_text_centering(void) {
    const char* text = "Test";
    Font mockFont = {0}; // Mock, but use real MeasureTextEx if needed
    f32 fontSize = 32.0f;
    f32Vector2 pos = {100.0f, 100.0f};
    f32Vector2 textSize = {40.0f, 20.0f}; // Mock MeasureTextEx result
    f32Vector2 center = getTextCenterPosition(text, mockFont, fontSize, pos);
    assert(center.x == (pos.x - textSize.x) / 2.0f);
    assert(center.y == (pos.y - textSize.y) / 2.0f);
}

static void test_score_text_generation(void) {
    GameState_St testGame = {
        .score = 1234,
    };

    buildScoreRelatedTexts(&testGame);
    assert(strcmp(testGame.scoreText, "Score: 1234") == 0);
}

int main(void) {
    test_font_loading();
    test_text_centering();
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