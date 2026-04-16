/**
    @file userTypes.h
    @author Fshimi-Hawlk
    @date 2026-01-25
    @date 2026-04-14
    @brief Core type definitions used throughout the Bingo mini-game (networked).
*/
#ifndef USER_TYPES_H
#define USER_TYPES_H

#include "common.h"
#include "APIs/generalAPI.h"

/**
    @brief Type for a 5x5 bingo card grid of numbers.
*/
typedef uint Card_t[5][5];

/**
    @brief All data related to one player's bingo card.
*/
typedef struct {
    Card_t          numbers;      ///< 0..99, free space usually UINT32_MAX
    bool            daubs[5][5];  ///< True if the number has been marked (daubed)
    Card_t          misclicks;    ///< 0-2 ok, >=3 = penalized square
} PlayerCard_St;

/**
    @brief Ball pool and draw history management (shared on server).
*/
typedef struct {
    uint encodedBalls[500];     ///< Array of all possible balls in the pool
    uint remainingCount;        ///< Number of balls remaining in the pool
    f32  showDelay;             ///< Delay before showing the next ball
    f32  choiceDelay;           ///< Delay allowed for player to choose a ball
    f32  graceDelay;            ///< Grace period after a ball is shown
} BallSystem_St;

/**
    @brief Current active call being displayed / available for daubing.
*/
typedef struct {
    uint            encodedValue;       ///< Encoded value of the current ball
    uint            column;             ///< 0..4 (B/I/N/G/O)
    uint            number;             ///< Number of the current ball
    char            displayedText[16];  ///< Text representation (e.g., "B-12")
    f32             timer;              ///< Time remaining for this call
} CallState_St;

/**
    @brief Different scenes in the Bingo game.
*/
typedef enum {
    GAME_SCENE_CARD_CHOICE,     ///< Players are choosing their cards
    GAME_SCENE_PLAYING,         ///< The game is active
    GAME_SCENE_LAUNCHING,       ///< Transitioning to start
    GAME_SCENE_END,             ///< Game has ended
} GameScene_Et;

/**
    @brief Overall game flow and outcome state.
*/
typedef struct {
    GameScene_Et    scene;              ///< Current scene
    char            resultMessage[64];  ///< Message to display at the end of the game
    f32             gameDuration;       ///< Total duration of the game
    uint            ballsDrawnTotal;    ///< Total number of balls drawn
} GameProgress_St;

/**
    @brief UI representation of a bingo card.
*/
typedef struct {
    Rectangle   innerRect;          ///< Rectangle for the card content
    Rectangle   bordersRect;        ///< Rectangle for the card borders
    Rectangle   backgroundRect;     ///< Rectangle for the card background
    Card_t      values;             ///< Numbers on this card
    bool        selected;           ///< Whether this card is selected by the player
} CardUI_St;

/**
    @brief Visual/layout constants and computed positions.
*/
typedef struct {
    Vector2     windowCenter;       ///< Center of the window
    Rectangle   cardRectsRect;      ///< Rect containing all card choices
    Rectangle   cardRect;           ///< Rect for the main card display
    Rectangle   cardRectBorder;     ///< Border for the main card display
    Rectangle   choiceCardProto;    ///< Prototype rect for card choice
    CardUI_St   choiceCards[12];    ///< Available card choices
} Layout_St;

/**
    @brief Complete Bingo game state used by both client and server interfaces.
*/
typedef struct BingoGame_St {
    BaseGame_St     base;

    s32             roomId;
    s32             clientID;
    CardUI_St*      previouslySelectedCard;     ///< Only relevant during card choice phase (client)
    PlayerCard_St   player;                     ///< Local player's card data
    BallSystem_St   balls;                      ///< Ball drawing system state
    CallState_St    currentCall;                ///< Currently active ball call
    GameProgress_St progress;                   ///< Overall game progress
    Layout_St       layout;                     ///< Layout and UI information
} BingoGame_St;

#endif // USER_TYPES_H