/**
    @file userTypes.h
    @author Fshimi-Hawlk
    @date 2026-01-25
    @date 2026-03-03
    @brief Core type definitions used throughout the game.

    Contributors:
        - Fshimi-Hawlk:
            - Added documentation start-up
            - Added `FontSize_Et`

    If the file needs more context than fits in @brief, write 2-5 lines here.
    @note Put warnings, important limitations, "we know it's ugly but...", or future plans here

    // Try to align the `for` for better readability
    // Try to keep the same order of the includes
    Use @see `path/to/related/file.h` when this file depends heavily on another one.
*/

#ifndef USER_TYPES_H
#define USER_TYPES_H

#include "common.h"

/**
    @brief Available font sizes used for in-game UI and text rendering.

    Values are listed in ascending order.  
    `_fontSizeCount` is **not** a valid font size - it serves as array dimension / loop boundary.
*/
typedef enum {
    FONT8,
    FONT10, FONT12, FONT14, FONT16, FONT18,
    FONT20, FONT22, FONT24, FONT26, FONT28,
    FONT30, FONT32, FONT34, FONT36, FONT38,
    FONT40, FONT42, FONT44, FONT46, FONT48,
    _fontSizeCount
} FontSize_Et;


// Game's Types

typedef uint Card_t[5][5];

/**
    @brief All data related to one player's bingo card.
    @note Currently fixed to PLAYER_COUNT = 1; designed to become array later.
*/
typedef struct {
    Card_t          numbers;      // 0..99, free space usually UINT32_MAX or 0
    bool            daubs[5][5];
    Card_t          misclicks;    // 0-2 ok, >=3 = penalized / lost square
} PlayerCard_St;

/**
    @brief Ball pool and draw history management.
*/
typedef struct {
    uint            encodedBalls[500];  // pre-shuffled 100*col+num values
    uint            remainingCount;     // starts at 500, decrements
    // uint         drawnHistory[500];  // optional: for replay / statistics
} BallSystem_St;

/**
    @brief Current active call being displayed / available for daubing.
*/
typedef struct {
    uint            encodedValue;       // full encoded ball (100*col+1 + num)
    uint            column;             // 0..4  (B/I/N/G/O)
    uint            number;             // 0..99
    char            displayedText[16];  // "B-42", "N-17", etc.
    f32             timer;              // time since this call started
} CallState_St;

typedef enum {
    GAME_SCENE_CARD_CHOICE,
    GAME_SCENE_PLAYING,
    GAME_SCENE_LAUNCHING,
    GAME_SCENE_END,
} GameScene_Et;

/**
    @brief Overall game flow and outcome state.
*/
typedef struct {
    GameScene_Et    scene;
    const char*     resultMessage;      // "BINGO! You win!", "No more balls - Game Over", etc.
    f32             gameDuration;
    uint            ballsDrawnTotal;
} GameProgress_St;

typedef struct {
    Rectangle   innerRect;        // final positioned rectangles for the 12 previews
    Rectangle   bordersRect;      // optional: positioned borders
    Rectangle   backgroundRect;   // optional: positioned background
    Card_t      values;
    bool        selected;
} CardUI_St;

/**
    @brief Visual/layout constants and computed positions.
*/
typedef struct {
    f32Vector2  windowCenter;
    Rectangle   cardRectsRect;          // main game card area (if still needed)
    Rectangle   cardRect;               // main card frame
    Rectangle   cardRectBorder;         // main card border
    Rectangle   choiceCardProto;        // prototype small card (unpositioned)
    CardUI_St   choiceCards[12];
} Layout_St;

/**
    @brief Core mutable game data – split into logical sub-structures.
*/
typedef struct {
    CardUI_St* previouslySelectedCard;
    PlayerCard_St   player;
    BallSystem_St   balls;
    CallState_St    currentCall;
    GameProgress_St progress;
    Layout_St       layout;
} BingoGame_St;

#endif // USER_TYPES_H