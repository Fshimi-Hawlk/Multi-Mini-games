/**
    @file rudp_core.h
    @author Multi Mini-Games Team
    @date 2026-03-18              // Creation date
    @date 2026-04-14
    @brief Core structures and functions of the simplified Reliable UDP (RUDP) protocol
*/
#ifndef RUDP_CORE_H
#define RUDP_CORE_H

#include "baseTypes.h"

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>

#include <arpa/inet.h>
#include <sys/socket.h>


#ifndef _WIN32
#include <signal.h>
#include <sys/types.h>
#endif

/**
    @name RUDP protocol tuning constants
    @{
*/
#define MAX_SEQUENCE    65535u  ///< 16-bit sequence number wrap-around value
#define HISTORY_SIZE    32      ///< Number of previous packets tracked in ack bitfield
#define MAX_CLIENTS     8       ///< Maximum simultaneous clients per server instance
/** @} */

/**
    @brief Fixed-size reliable UDP header (exactly 11 bytes on wire: 2+2+4+2+1)
*/
#if defined(_MSC_VER)
#pragma pack(push, 1)
#endif

typedef struct __attribute__((packed)) {
    u16 sequence;       ///< This packet's unique sequence number
    u16 ack;            ///< Highest sequence number received in-order
    u32 ackBitfield;   ///< Bitfield of the previous 32 packets (1 = received)
    u16 senderId;      ///< Unique sender identifier (assigned by server)
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