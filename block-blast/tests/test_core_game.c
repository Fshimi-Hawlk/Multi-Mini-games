#include <assert.h>

#include "utils/userTypes.h"

extern GameState_St game;
GameState_St game = {0};

#include "core/game/game.h"
#include "core/game/game.c"

s32 main(void) {
    printf("Running tests for core/game/game...\n");

    // Test buildScoreRelatedTexts (modifies game.scoreText/streakText)
    {
        game.score = 100;
        game.streakCount = 5;
        buildScoreRelatedTexts();
        assert(strcmp(game.scoreText, "Score: 100") == 0 && "Score text built correctly");
        assert(strcmp(game.streakText, "Streak: 5") == 0 && "Streak text built correctly");
    }

    // Test calculateScore
    {
        Board_St board = {0};
        board.width = 8;
        board.height = 8;
        board.rowsToClear[0] = 1;
        board.columnsToClear[0] = 1;

        f32 score = calculateScore(&board);
        assert(score == 2 * SCORE_PER_LINE_CLEAR * 1.5f && "Score with multi-bonus");
    }

    // Test manageScore (truncated, test visible part)
    {
        GameState_St testGame = {0};
        Prefab_St prefab = {0};
        prefab.blockCount = 3;

#define SCORE_PER_UNIT_PLACED 5
        manageScore(&testGame, &prefab);
        assert(testGame.score == 3 * SCORE_PER_UNIT_PLACED && "Score added per unit");
#undef SCORE_PER_UNIT_PLACED
    }

    printf("All tests passed for core/game/game!\n");
    return 0;
}