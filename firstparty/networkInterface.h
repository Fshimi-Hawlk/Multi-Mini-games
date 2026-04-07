/**
    @file networkInterface.h
    @author i-Charlys
    @author Fshimi Hawlk
    @date 2025-03-20
    @brief Defines the network message format and per-game module interfaces for client and server sides
    @note GameTLVHeader_St uses #pragma pack(1) — exact 4-byte layout is required for wire format
*/

#ifndef NETWORK_INTERFACE_H
#define NETWORK_INTERFACE_H

#include "baseTypes.h"
#include "rudp_core.h"

/**
    @brief External reference to the network socket.
*/
extern s32 networkSocket;

/**
    @brief External reference to the server connection.
*/
extern RUDPConnection_St serverConnection;

#define UNICAST -1

enum BaseActionCodes_e {
    ACTION_CODE_JOIN_ACK     = 0x01,    ///< Acknowledging a join request.
    ACTION_CODE_JOIN_GAME    = 0x02,    ///< Joining a game.
    ACTION_CODE_START_GAME   = 0x03,    ///< Starting a game.
    ACTION_CODE_SYNC_GAME    = 0x04,    ///< Synchronizing the game state.
    ACTION_CODE_QUIT_GAME    = 0x05,    ///< quitting the game.
    ACTION_CODE_GAME_DATA    = 0x06,    ///< Multi-Mini-Games specific game data
    ACTION_CODE_LOBBY_MOVE   = 0x07,    ///< Player movement in the lobby.
    ACTION_CODE_LOBBY_ROOM_QUERY,       ///< Querying room information.
    ACTION_CODE_LOBBY_ROOM_INFO,        ///< Receiving room information.
    ACTION_CODE_LOBBY_CHAT,             ///< Lobby chat message.
    ACTION_CODE_LOBBY_SWITCH_GAME,      ///< Switching to a mini-game.
    
    ACTION_CODE_DISCOVERY_QUERY = 0x10, ///< Global server discovery query (broadcast).
    ACTION_CODE_DISCOVERY_INFO  = 0x11, ///< Global server discovery response.

    firstAvailableActionCode    = 0x20  ///< First action code usable by sub-games
};

/**
    @brief Port used for server communication.
*/
#define SERVER_PORT 8080

#pragma pack(push, 1)

/**
    @brief Type-Length-Value header used on the network for mini-game messages
*/
typedef struct {
    u8  game_id;        ///< 0 = lobby, otherwise mini-game identifier
    u8  action;         ///< game-specific command / event code
    u16 length;         ///< size of the payload that follows (not including this header)
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
    void (*on_data)(s32 player_id, u8 action, const void* data, u16 len);

    void (*update)(float dt);   ///< Local simulation / animation update
    void (*draw)(void);         ///< Raylib rendering pass
    void (*destroy)(void);      ///< Clean up resources and textures
} GameClientInterface_St;

/**
    @brief Function pointer type for broadcasting messages to room participants
    Provided by the networking/server layer to each game instance.
*/
typedef void (*BroadcastMessage_Ft)(
    s32         room_id,
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
    void* (*create_instance)(void);

    /**
        @brief Handles an incoming player action / event
        @param state        Game-specific instance state
        @param room_id      The ID of the room where this action occurred
        @param player_id    Player who sent this message
        @param action       Action code from client
        @param payload      Decoded payload pointer
        @param len          Payload size in bytes
        @param broadcast    Function to send messages to other players in the room
    */
    void (*on_action)(
        void*               state,
        s32                 room_id,
        s32                 player_id,
        u8                  action,
        const void*         payload,
        u16                 len,
        BroadcastMessage_Ft broadcast
    );

    void (*on_tick)(void* state);               ///< Main server tick / simulation step

    /**
        @brief Called when a player disconnects or leaves the room
        @param state        Game instance
        @param player_id    ID of the player who left
    */
    void (*on_player_leave)(void* state, s32 player_id);

    /**
        @brief Destroys the game instance and frees all owned memory
        @param state        Pointer previously returned by create_instance
    */
    void (*destroy_instance)(void* state);
} GameServerInterface_St;

#endif // NETWORK_INTERFACE_H
