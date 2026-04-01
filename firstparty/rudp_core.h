/**
    @file rudpCore.h
    @author i-Charlys
    @date 2026-03-18              // Creation date
    @date 2026-03-20              // Last meaningful change: applied project code style
    @brief Core structures and functions of the simplified Reliable UDP (RUDP) protocol
    
    Provides packet header format, connection state tracking, 
    sequence numbering and basic duplicate / out-of-order detection.
    
    @note Designed to be layered under the game TLV messages (see networkInterface.h)
*/

#ifndef RUDP_CORE_H
#define RUDP_CORE_H

#include "baseTypes.h"

/**
    @name RUDP protocol tuning constants
    @{
*/
#define MAX_SEQUENCE    65535u  ///< 16-bit sequence number wrap-around value
#define HISTORY_SIZE    32      ///< Number of previous packets tracked in ack bitfield
#define MAX_CLIENTS     8       ///< Maximum simultaneous clients per server instance
/** @} */

/**
    @brief Fixed-size reliable UDP header (exactly 9 bytes on wire)
*/
#if defined(_MSC_VER)
#pragma pack(push, 1)
#endif

typedef struct __attribute__((packed)) {
    u16 sequence;       ///< This packet's unique sequence number
    u16 ack;            ///< Highest sequence number received in-order
    u32 ack_bitfield;   ///< Bitfield of the previous 32 packets (1 = received)
    u16 sender_id;      ///< Unique sender identifier (assigned by server)
    u8  action;         ///< RUDP control action or game-specific command
} RUDPHeader_St;

#if defined(_MSC_VER)
#pragma pack(pop)
#endif

/**
    @brief Per-connection reliable UDP state (sender + receiver side)
*/
typedef struct RUDPConnection_St {
    u16 local_sequence;     ///< Next sequence number to send
    u16 remote_sequence;    ///< Highest in-order sequence received from peer
    u32 receive_history;    ///< Bitfield tracking recently received packets
} RUDPConnection_St;

/**
    @brief Resets connection state to initial values
    @param conn     Connection state to initialize
*/
void rudpInitConnection(RUDPConnection_St* conn);

/**
    @brief Prepares a new RUDP header using current connection state
    @param conn         Active connection state
    @param action       Action byte to place in header (RUDP or game)
    @param out_header   Filled with the ready-to-send header
*/
void rudpGenerateHeader(
    RUDPConnection_St*  conn,
    u8                  action,
    RUDPHeader_St*      out_header
);

/**
    @brief Processes an incoming RUDP header and updates connection state
    @param conn         Connection state to update
    @param in_header    Received packet header (must remain valid)
    @return             `true` if packet should be processed (new or needed), `false` if duplicate/old
*/
bool rudpProcessIncoming(
    RUDPConnection_St*      conn,
    const RUDPHeader_St*    in_header
);

#endif // RUDP_CORE_H