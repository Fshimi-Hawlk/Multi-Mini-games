/**
    @file utils/userTypes.h
    @author Fshimi-Hawlk
    @date 2026-01-25
    @date 2026-03-31
    @brief Core type definitions used throughout the Bingo mini-game (networked).

    Contributors:
        - Fshimi-Hawlk:
            - Moved BingoGame_St here (bingoAPI.c will be removed)
            - Added BingoStatus_Et and per-player support for networked play
  
    @see core/game.h for shared helper declarations (bingo_isValidDaub, bingo_hasBingo)
*/

#ifndef USER_TYPES_H
#define USER_TYPES_H

#include "common.h"
#include "APIs/generalAPI.h"

typedef uint Card_t[5][5];

/**
    @brief All data related to one player's bingo card.
*/
typedef struct {
    Card_t          numbers;      ///< 0..99, free space usually UINT32_MAX
    bool            daubs[5][5];
    Card_t          misclicks;    ///< 0-2 ok, >=3 = penalized square
} PlayerCard_St;

/**
    @brief Ball pool and draw history management (shared on server).
*/
typedef struct {
    uint encodedBalls[500];
    uint remainingCount;
    f32  showDelay;
    f32  choiceDelay;
    f32  graceDelay;
} BallSystem_St;

/**
    @brief Current active call being displayed / available for daubing.
*/
typedef struct {
    uint            encodedValue;
    uint            column;             ///< 0..4 (B/I/N/G/O)
    uint            number;
    char            displayedText[8];
    f32             timer;
} CallState_St;

typedef enum {
    GAME_SCENE_WAITING_ROOM,
    GAME_SCENE_CARD_CHOICE,
    GAME_SCENE_LAUNCHING,
    GAME_SCENE_PLAYING,
    GAME_SCENE_END,
} GameScene_Et;

/**
    @brief Overall game flow and outcome state.
*/
typedef struct {
    GameScene_Et    scene;
    const char     *resultMessage;
    f32             gameDuration;
    uint            ballsDrawnTotal;
} GameProgress_St;

typedef struct {
    Rectangle   innerRect;
    Rectangle   bordersRect;
    Rectangle   backgroundRect;
    Card_t      values;
    bool        selected;
} CardUI_St;

/**
    @brief Visual/layout constants and computed positions.
*/
typedef struct {
    f32Vector2  windowCenter;
    Rectangle   cardRectsRect;
    Rectangle   cardRect;
    Rectangle   cardRectBorder;
    Rectangle   choiceCardProto;
    CardUI_St   choiceCards[12];
} Layout_St;

/**
    @brief Complete Bingo game state used by both client and server interfaces.
           Server uses extra fields (playerCards, numPlayers). Client uses the local `player` and layout.
           bingoAPI.c will be removed after this change.
*/
typedef struct {
    BaseGame_St     base;

    s32             clientID;
    CardUI_St*      previouslySelectedCard;     ///< Only relevant during card choice phase (client)
    PlayerCard_St   player;
    BallSystem_St   balls;
    CallState_St    currentCall;
    GameProgress_St progress;
    Layout_St       layout;
} BingoGame_St;

#endif // USER_TYPES_H