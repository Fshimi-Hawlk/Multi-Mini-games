/**
 * @file test_rudp.c
 * @brief Unit tests for RUDP implementation.
 * @author i-Charlys
 */

#include "rudp_core.h"
#include "networkInterface.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <arpa/inet.h>



/** 
 * Test rudpInitConnection initializes a connection struct correctly.
 */
void test_rudp_init() {
    RUDPConnection_St conn;
    rudpInitConnection(&conn);
    assert(conn.local_sequence == 0);
    assert(conn.remote_sequence == 65535);
    printf("test_rudp_init passed\n");
}

/**
 * Test rudpGenerateHeader generates a header with the correct sequence and action.
 */
void test_rudp_header_gen() {
    RUDPConnection_St conn;
    rudpInitConnection(&conn);
    RUDPHeader_St h;
    rudpGenerateHeader(&conn, ACTION_GAME_DATA, &h);
    assert(ntohs(h.sequence) == 0);
    assert(h.action == ACTION_GAME_DATA);
    assert(conn.local_sequence == 1);
    printf("test_rudp_header_gen passed\n");
}

/**
 * Test rudpProcessIncoming processes incoming packets correctly.
 */
void test_rudp_process_incoming() {
    RUDPConnection_St conn;
    rudpInitConnection(&conn);
    
    RUDPHeader_St h;
    memset(&h, 0, sizeof(h));
    h.sequence = htons(0);
    h.action = ACTION_GAME_DATA;

    assert(rudpProcessIncoming(&conn, &h) == true);
    assert(conn.remote_sequence == 0);
    
    // Test duplicate
    assert(rudpProcessIncoming(&conn, &h) == false);
    assert(conn.remote_sequence == 0);
    
    // Test newer
    h.sequence = htons(1);
    assert(rudpProcessIncoming(&conn, &h) == true);
    assert(conn.remote_sequence == 1);
    
    printf("test_rudp_process_incoming passed\n");
}

/**
 * Test rudpSendPacket sends a packet with the correct sequence and action.
 */
void test_rudp_send_packet() {
    RUDPConnection_St conn;
    rudpInitConnection(&conn);
    
    RUDPHeader_St h;
    memset(&h, 0, sizeof(h));
    h.sequence = htons(0);
    h.action = ACTION_GAME_DATA;

    rudpSendPacket(&conn, &h, NULL, 0);
    assert(conn.local_sequence == 1);
    printf("test_rudp_send_packet passed\n");
}

/**
 * Test rudpSendPacket sends a packet with the correct sequence and action.
 */
int main() {
    test_rudp_init();
    test_rudp_header_gen();
    test_rudp_process_incoming();
    printf("All RUDP tests passed!\n");
    return 0;
}
