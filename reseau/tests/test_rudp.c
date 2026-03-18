/**
 * @file test_rudp.c
 * @brief Unit tests for RUDP implementation.
 */

#include "rudp_core.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

void test_rudp_init() {
    RUDP_Connection conn;
    RUDP_InitConnection(&conn);
    assert(conn.local_sequence == 0);
    assert(conn.remote_sequence == 65535);
    printf("test_rudp_init passed\n");
}

void test_rudp_header_gen() {
    RUDP_Connection conn;
    RUDP_InitConnection(&conn);
    RUDP_Header h;
    RUDP_GenerateHeader(&conn, 5, &h);
    assert(h.sequence == 0);
    assert(h.action == 5);
    assert(conn.local_sequence == 1);
    printf("test_rudp_header_gen passed\n");
}

void test_rudp_process_incoming() {
    RUDP_Connection conn;
    RUDP_InitConnection(&conn);
    
    RUDP_Header h = { .sequence = 0, .ack = 0, .action = 5 };
    assert(RUDP_ProcessIncoming(&conn, &h) == true);
    assert(conn.remote_sequence == 0);
    
    // Test duplicate
    assert(RUDP_ProcessIncoming(&conn, &h) == false);
    assert(conn.remote_sequence == 0);
    
    // Test newer
    h.sequence = 1;
    assert(RUDP_ProcessIncoming(&conn, &h) == true);
    assert(conn.remote_sequence == 1);
    
    printf("test_rudp_process_incoming passed\n");
}

int main() {
    test_rudp_init();
    test_rudp_header_gen();
    test_rudp_process_incoming();
    printf("All RUDP tests passed!\n");
    return 0;
}
