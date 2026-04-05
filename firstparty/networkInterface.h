/**
    @file networkInterface.h
    @author i-Charlys (CAILLON Charles)
    @author Fshimi Hawlk
    @date 2025-03-20
    @brief Defines the network message format and per-game module interfaces for client and server sides
    @note GameTLVHeader_St uses #pragma pack(1) — exact 4-byte layout is required for wire format
*/

#ifndef NETWORK_INTERFACE_H
#define NETWORK_INTERFACE_H

#include "baseTypes.h"
#include "rudp_core.h"
#include "APIs/generalAPI.h"

/**
    @brief External reference to the network socket.
*/
extern s32 networkSocket;

/**
    @brief External reference to the server connection.
*/
extern RUDPConnection_St serverConnection;

#define BROADCAST_ALL -1

enum BaseActionCodes_e {
    ACTION_CODE_JOIN_ACK     = 0x01,    ///< Acknowledging a join request.
    ACTION_CODE_JOIN_GAME    = 0x02,    ///< Joining a game.
    ACTION_CODE_START_GAME   = 0x03,    ///< Starting a game.
    ACTION_CODE_SYNC_GAME    = 0x04,    ///< Synchronizing the game state.
    ACTION_CODE_GAME_DATA    = 0x05,    ///< Game data transmission.
    ACTION_CODE_QUIT_GAME    = 0x06,    ///< quitting the game.

    // ── Multi-instance actions ─────────────────────────────────────
    ACTION_CODE_CREATE_INSTANCE   = 0x10,   ///< Client requests new game instance
    ACTION_CODE_JOIN_INSTANCE     = 0x11,   ///< Client joins specific instance
    ACTION_CODE_LEAVE_INSTANCE    = 0x12,   ///< Client leaves current instance
    ACTION_CODE_LIST_INSTANCES    = 0x13,   ///< Client asks for list of instances of a game type
    ACTION_CODE_INSTANCE_INFO     = 0x14,   ///< Server broadcasts / replies with instance list

    firstAvailableActionCode            ///< First action code usable by the codes specific to each sub-game
};

/**
    @brief Port used for server communication.
*/
#define SERVER_PORT 8080


// ────────────────────────────────────────────────
// Payload types for multi-instance actions
// ────────────────────────────────────────────────

#pragma pack(push, 1)
/**
    @brief Payload sent by a client when requesting creation of a new game instance.
*/
typedef struct {
    MiniGame_Et gameType;   ///< Which mini-game to start (MINI_GAME_KFF, MINI_GAME_BINGO, ...)
} CreateInstancePayload_St;

/**
    @brief Payload for joining a specific instance.
*/
typedef struct {
    u32 instanceId;         ///< Target instance ID to join
} JoinInstancePayload_St;

/**
    @brief Payload when client requests list of instances of a game type.
*/
typedef struct {
    MiniGame_Et gameType;   ///< Which game type to list instances for
} ListInstancesPayload_St;

/**
    @brief Compact info for one instance sent in INSTANCE_INFO responses.
*/
typedef struct {
    u32         instanceId;
    u8          playerCount;
    u8          maxPlayers;     // 0 = no limit (lobby)
    char        hostName[32];   // name of the host player (or "Lobby" for ID 0)
} InstanceInfo_St;
#pragma pack(pop)


#pragma pack(push, 1)

/**
    @brief Type-Length-Value header used on the network for mini-game messages
*/
typedef struct {
    u8  game_id;        ///< 0 = lobby, otherwise mini-game identifier
    u8  action;         ///< game-specific command / event code
    u16 length;         ///< payload size after this header
    u32 instanceId;     ///< which game instance this message belongs to (0 = lobby)
} GameTLVHeader_St;

#pragma pack(pop)

/**
    @brief Contract that every client-side mini-game module must implement
*/
typedef struct {
    u8          id;             ///< Network identifier (matches GameTLVHeader_St.game_id)
    const char* name;           ///< Human-readable name (for debug/UI)

    void (*init)(void);         ///< Load textures, models, sounds, initialize static data

    /**
        @brief Called when a network packet routed to this game is received
        @param player_id    Sending player (client-local ID or network ID)
        @param action       Value from GameTLVHeader_St.action
        @param data         Pointer to the payload bytes (after header)
        @param len          Payload length
    */
    void (*on_data)(int player_id, u8 action, const void* data, u16 len);

    void (*update)(float dt);   ///< Local simulation / animation update
    void (*draw)(void);         ///< Raylib rendering pass
} GameClientInterface_St;

// Forward declaration for boardcast message function type
typedef struct GameInstance_St GameInstance_St;

/**
    @brief Function pointer type for broadcasting messages to room participants
    Provided by the networking/server layer to each game instance.
*/
typedef void (*BroadcastMessage_Ft)(
    struct GameInstance_St* instance,
    s32         exclude_id,
    u8          action,
    const void* payload,
    u16         len
);

/**
    @brief Server-side game module interface (per-room game instance)
*/
typedef struct {
    char* game_name;            ///< Identifier / debug name (e.g. "lobby", "race", "tag")

    /**
        @brief Creates and initializes a fresh game state for a new room
        @return Allocated and initialized game-specific state
    */
    void* (*createInstance)(void);

    /**
        @brief Handles an incoming player action / event for a specific game instance.

        @param instance     The full GameInstance_St that owns this game.
                            Use `instance->gameState` to access the concrete game data.
        @param playerId     ID of the player who sent this message (UDPClient_St slot).
        @param action       Action code received from the client.
        @param payload      Pointer to the raw payload bytes (after GameTLVHeader_St).
        @param len          Length of the payload in bytes.
        @param broadcast    Function to send messages **only to players inside this instance**.
                            Always call it with the same `instance` pointer you received.
    */
    void (*onAction)(
        GameInstance_St*    instance,
        int                 playerId,
        u8                  action,
        const void*         payload,
        u16                 len,
        BroadcastMessage_Ft broadcast
    );

    /**
        @brief Main server tick / simulation step for this instance.
        @param gameState    The concrete game state (same pointer returned by createInstance).
    */
    void (*onTick)(void* gameState);

    /**
        @brief Called when a player disconnects or leaves the instance.
        @param gameState    The concrete game state.
        @param playerId     ID of the player who left.
    */
    void (*onPlayerLeave)(void* gameState, int playerId);

    /**
        @brief Destroys the game instance and frees all owned memory.
        @param gameState    The concrete game state returned by createInstance.
    */
    void (*destroyInstance)(void* gameState);
} GameServerInterface_St;

/** @brief One running game room (lobby or mini-game). */
struct GameInstance_St {
    u32                     id;                     ///< Unique instance ID (0 = lobby)
    MiniGame_Et             gameType;               ///< Which mini-game (or lobby)
    GameServerInterface_St* interface;              ///< Pointer to the game’s interface
    void*                   gameState;              ///< Opaque game-specific state
    bool                    active;                 ///< False after destroy
    s32                     players[MAX_CLIENTS];   ///< Client IDs currently in this instance (-1 = free slot)
    u8                      playerCount;            ///< Current number of players
    s32                     hostClientId;           ///< Creator / host (or -1)
};

#endif // NETWORK_INTERFACE_H