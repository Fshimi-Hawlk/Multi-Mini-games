/**
    @file networkInterface.h
    @author Multi Mini-Games Team
    @date 2025-03-20
    @date 2026-04-14
    @brief Defines the network message format and per-game module interfaces for client and server sides
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

/**
    @brief Definition of enum BaseActionCodes_e
*/
enum BaseActionCodes_e {
    ACTION_CODE_JOIN_ACK          = 0x01,    ///< Acknowledging a join request.
    ACTION_CODE_JOIN_GAME         = 0x02,    ///< Joining a game.
    ACTION_CODE_START_GAME        = 0x03,    ///< Starting a game.
    ACTION_CODE_SYNC_GAME         = 0x04,    ///< Synchronizing the game state.
    ACTION_CODE_QUIT_GAME         = 0x05,    ///< quitting the game.
    ACTION_CODE_GAME_DATA         = 0x06,    ///< Multi-Mini-Games specific game data
    ACTION_CODE_LOBBY_MOVE        = 0x07,    ///< Player movement in the lobby.
    ACTION_CODE_LOBBY_ROOM_QUERY  = 0x08,    ///< Querying room information.
    ACTION_CODE_LOBBY_ROOM_INFO   = 0x09,    ///< Receiving room information.
    ACTION_CODE_LOBBY_CHAT        = 0x0A,    ///< Lobby chat message.
    ACTION_CODE_LOBBY_SWITCH_GAME = 0x0B,    ///< Switching to a mini-game.
    ACTION_CODE_JOIN_ERROR        = 0x0C,    ///< Error during join (e.g. duplicate name).
    
    ACTION_CODE_DISCOVERY_QUERY    = 0x10,    ///< Global server discovery query (broadcast).
    ACTION_CODE_DISCOVERY_INFO     = 0x11,    ///< Global server discovery response.

    firstAvailableActionCode       = 0x20     ///< First action code usable by sub-games
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
    u8   gameId;         ///< 0 = lobby, otherwise mini-game identifier
    u8   action;         ///< game-specific command / event code
    u16  length;         ///< size of the payload that follows (not including this header)
    bool isReliable;     ///< true if the message must be delivered reliably
} GameTLVHeader_St;

/**
    @brief Shared structure for room information exchange.
*/
typedef struct {
    u16 id;             ///< Room network index
    u16 playerCount;    ///< Current number of players
    char name[32];      ///< Display name
    char creator[32];   ///< Creator pseudo
} RoomInfo_St;

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
/**
    @brief Global variable )
*/
);

/**
    @brief Server-side game module interface (per-room game instance)
*/
typedef struct {
    char* gameName;            ///< Identifier / debug name (e.g. "lobby", "race", "tag")

    /**
        @brief Creates and initializes a fresh game state for a new room
        @return Allocated and initialized game-specific state
    */
    void* (*createInstance)(void);

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
    void (*onAction)(
        void*               state,
        s32                 roomId,
        s32                 playerId,
        u8                  action,
        const void*         payload,
        u16                 len,
        BroadcastMessage_Ft broadcast
    );

    void (*onTick)(void* state);               ///< Main server tick / simulation step

    /**
        @brief Called when a player disconnects or leaves the room
        @param state        Game instance
        @param player_id    ID of the player who left
    */
    void (*onPlayerLeave)(void* state, s32 player_id);

    /**
        @brief Destroys the game instance and frees all owned memory
        @param state        Pointer previously returned by create_instance
    */
    void (*destroyInstance)(void* state);
} GameServerInterface_St;

#endif // NETWORK_INTERFACE_H
