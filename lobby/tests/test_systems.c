/**
 * @file test_systems.c
 * @brief Unit tests for Network synchronization.
 */

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include "utils/userTypes.h"

void test_network_serialization() {
    printf("Testing Network Serialization...\n");
    Player_st p = {0};
    p.position.x = 123.45f;
    p.position.y = 678.90f;
    p.angle = 45.0f;
    p.skin_id = 1;
    p.active = true;

    // Simulate sending (Serialization)
    PlayerNet_st net = {
        .x = p.position.x,
        .y = p.position.y,
        .angle = p.angle,
        .skin_id = p.skin_id,
        .active = p.active
    };

    // Verify size and packing
    printf("  - Size of PlayerNet_st: %zu bytes\n", sizeof(PlayerNet_st));
    assert(sizeof(PlayerNet_st) == (sizeof(float)*3 + sizeof(int) + sizeof(bool)));

    // Simulate receiving (Deserialization)
    Player_st other = {0};
    other.position.x = net.x;
    other.position.y = net.y;
    other.angle = net.angle;
    other.skin_id = net.skin_id;
    other.active = net.active;

    assert(fabs(other.position.x - 123.45f) < 0.01f);
    assert(fabs(other.position.y - 678.90f) < 0.01f);
    assert(other.angle == 45.0f);
    assert(other.skin_id == 1);
    assert(other.active == true);
    printf("  - Serialization/Deserialization: OK (Packed struct works)\n");
}

int main() {
    test_network_serialization();
    printf("ALL SYSTEM TESTS PASSED!\n");
    return 0;
}
