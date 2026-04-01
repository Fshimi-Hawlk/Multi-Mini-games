/**
 * @file server.c
 * @brief Authoritative RUDP server with secure multi-module support.
 * 
 * This server handles multiple connections, packet routing to different
 * game modules, and message broadcasting.
 * 
 * Communication Scheme:
 * [CLIENT 1] <----RUDP----> [ SERVER ] <----RUDP----> [CLIENT 2]
 *                               |
 *                               v
 *                    +---------------------+
 *                    |   MODULE ROUTING    |
 *                    +---------------------+
 *                    | action = SWITCH?    | ----> Change active_module
 *                    | action = GAME_DATA? | ----> on_action(active_game)
 *                    +---------------------+
 * 
 * @author i-Charlys (CAILLON Charles)
 * @author Fshimi-Hawlk
 * @date 2026-03-18
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#include "networkInterface.h"

#define PORT 8080                                       /**< Server listening port. */
#define TIMEOUT_US 5000000000                           /**< Disconnection timeout in microseconds (5s). */
#define SERVER_NAME "MMG-Lobby-Server"                  /**< Server display name. */
#define MAX_PAYLOAD_SIZE (2048 - sizeof(RUDPHeader_St)) /**< Maximum payload size. */

// Routing interfaces
extern GameServerInterface_St lobby_module; /**< Lobby module instance. */
extern GameServerInterface_St king_module;  /**< King for Four module instance. */

/**
 * @struct UDP_Client
 * @brief Internal representation of a client on the server.
 */
typedef struct {
    bool active;                  /**< If the slot is occupied by a client. */
    struct sockaddr_in address;   /**< Client IP address and port. */
    RUDPConnection_St rudp_state; /**< RUDP state for this specific client. */
    struct timeval last_seen;     /**< Timestamp of the last activity received. */
} UDP_Client;

int master_socket = -1; /**< Main server socket. */
UDP_Client clients[MAX_CLIENTS]; /**< List of connected clients. */
GameServerInterface_St *active_module = NULL; /**< Pointer to the active game module. */
void* active_game_state = NULL;      /**< Internal state of the active module. */

/**
 * @brief Finds a client index by address or allocates a new slot.
 * 
 * @param addr Source address of the packet.
 * @return int Client index (0 to MAX_CLIENTS-1), or -1 if the server is full.
 */
int find_or_create_client(struct sockaddr_in *addr) {
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(clients[i].active && 
           clients[i].address.sin_addr.s_addr == addr->sin_addr.s_addr &&
           clients[i].address.sin_port == addr->sin_port) {
            return i;
        }
    }
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(!clients[i].active) {
            clients[i].active = true;
            clients[i].address = *addr;
            rudpInitConnection(&clients[i].rudp_state);
            gettimeofday(&clients[i].last_seen, NULL);
            printf("[CONNEXION] Slot %d allocated for %s:%d\n", i, inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
            return i;
        }
    }
    return -1;
}

/**
 * @brief Broadcasts or sends a message to a specific client.
 * 
 * @param room_id Room identifier (-1 for UNICAST).
 * @param exclude_id Client ID to exclude (in BROADCAST) or unique target (in UNICAST).
 * @param action Action type to send.
 * @param payload Pointer to data.
 * @param len Data size.
 */
void server_broadcast(int room_id, int exclude_id, u8 action, const void *payload, u16 len) {
    u8 buffer[2048];
    if (len > MAX_PAYLOAD_SIZE) len = MAX_PAYLOAD_SIZE;

    for (int i = 0; i < MAX_CLIENTS; i++) {
        bool should_send = false;
        if (room_id == -1) {
            // UNICAST mode: send ONLY to exclude_id (which is now target_id)
            if (i == exclude_id) should_send = true;
        } else {
            // BROADCAST mode: send to all EXCEPT exclude_id
            if (clients[i].active && i != exclude_id) should_send = true;
        }

        if (should_send && clients[i].active) {
            RUDPHeader_St header;
            rudpGenerateHeader(&clients[i].rudp_state, action, &header);
            header.sender_id = htons((u16)exclude_id); 
            
            memcpy(buffer, &header, sizeof(RUDPHeader_St));
            if (len > 0 && payload != NULL) {
                memcpy(buffer + sizeof(RUDPHeader_St), payload, len);
            }
            
            sendto(master_socket, buffer, sizeof(RUDPHeader_St) + len, 0, 
                   (struct sockaddr *)&clients[i].address, sizeof(struct sockaddr_in));
        }
    }
}

/**
 * @brief Responds to server discovery queries (ACTION_CODE_LOBBY_ROOM_QUERY).
 * 
 * @param client_addr Requesting client address.
 */
void handle_discovery_query(struct sockaddr_in *client_addr) {
    RUDPHeader_St response;
    RUDPConnection_St temp_conn;
    rudpInitConnection(&temp_conn);
    
    rudpGenerateHeader(&temp_conn, ACTION_CODE_LOBBY_ROOM_QUERY, &response);
    response.sender_id = htons(999); 
    
    u8 buffer[512];
    memcpy(buffer, &response, sizeof(RUDPHeader_St));
    
    char info[256];
    const char *module_name = (active_module && active_module->game_name) ? active_module->game_name : "Unknown";
    snprintf(info, sizeof(info), "%s [%s]", SERVER_NAME, module_name);
    
    memcpy(buffer + sizeof(RUDPHeader_St), info, (size_t)strlen(info) + 1);

    sendto(master_socket, buffer, sizeof(RUDPHeader_St) + strlen(info) + 1, 0,
           (struct sockaddr *)client_addr, sizeof(struct sockaddr_in));
}

/**
 * @brief Checks for inactive clients and disconnects them if necessary.
 */
void check_timeouts(void) {
    struct timeval now;
    gettimeofday(&now, NULL);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            long long elapsed = (long long)(now.tv_sec - clients[i].last_seen.tv_sec) * 1000000LL +
                                (long long)(now.tv_usec - clients[i].last_seen.tv_usec);
            if (elapsed > TIMEOUT_US) {
                printf("[TIMEOUT] Client %d disconnected\n", i);
                clients[i].active = false;
                
                // Notify other clients
                server_broadcast(0, i, ACTION_CODE_QUIT_GAME, NULL, 0);

                if (active_module && active_module->on_player_leave) {
                    active_module->on_player_leave(active_game_state, i);
                }
            }
        }
    }
}

/**
 * @brief Main server entry point.
 * 
 * @return int Exit code.
 */
int main(void) {
    struct sockaddr_in server_addr;
    for (int i = 0; i < MAX_CLIENTS; i++) clients[i].active = false;

    if ((master_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket fail"); exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(master_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind fail"); exit(1);
    }

    // Force server to start on the Lobby
    active_module = &lobby_module;
    
    if (active_module && active_module->create_instance) {
        active_game_state = active_module->create_instance();
    } else {
        printf("[ERROR] Module doesn't implement `create_instance`.\n");
        exit(1);
    }

    printf(">>> RUDP SERVER STARTED ON PORT %d <<<\n", PORT);
    printf(">>> ACTIVE MODULE : %s <<<\n", active_module->game_name ? active_module->game_name : "NULL");

    while (1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);

        struct timeval tv = {0, 16666}; 
        int activity = select(master_socket + 1, &readfds, NULL, NULL, &tv);

        if (activity > 0 && FD_ISSET(master_socket, &readfds)) {
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);
            u8 buffer[2048];
            ssize_t len = recvfrom(master_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &addr_len);

            if (len >= (ssize_t)sizeof(RUDPHeader_St)) {
                RUDPHeader_St header;
                memcpy(&header, buffer, sizeof(RUDPHeader_St));

                if (header.action == ACTION_CODE_LOBBY_ROOM_QUERY) {
                    handle_discovery_query(&client_addr);
                } 
                else if (header.action == ACTION_CODE_JOIN_GAME) {
                    int id = find_or_create_client(&client_addr);
                    if (id != -1) {
                        // Reset RUDP state for fresh join
                        rudpInitConnection(&clients[id].rudp_state);
                        rudpProcessIncoming(&clients[id].rudp_state, &header);
                        gettimeofday(&clients[id].last_seen, NULL);
                        
                        // Send back the assigned ID to the client
                        u16 assigned_id = (u16)id;
                        server_broadcast(-1, id, ACTION_CODE_JOIN_ACK, &assigned_id, sizeof(u16));
                        printf("[CONNEXION] ID %d sent to new client\n", id);

                        if (active_module && active_module->on_action) {
                            active_module->on_action(active_game_state, id, header.action, 
                                buffer + sizeof(RUDPHeader_St), (u16)(len - sizeof(RUDPHeader_St)), server_broadcast);
                        }
                    }
                }
                else {
                    int id = find_or_create_client(&client_addr);
                    if (id != -1 && rudpProcessIncoming(&clients[id].rudp_state, &header)) {
                        gettimeofday(&clients[id].last_seen, NULL);
                        
                        if (header.action == ACTION_CODE_LOBBY_SWITCH_GAME) {
                            u8 target_game_id = buffer[sizeof(RUDPHeader_St)];
                            printf("[MODULE] Requesting switch to ID: %d\n", target_game_id);
                            
                            // For simplicity, ID 1 = King for Four
                            if (target_game_id == 1) {
                                if (active_module != &king_module) {
                                    if (active_module && active_module->destroy_instance) {
                                        active_module->destroy_instance(active_game_state);
                                    }
                                    active_module = &king_module;
                                    active_game_state = active_module->create_instance();
                                    printf("[MODULE] Switched to King For Four.\n");
                                }
                                
                                u8 switch_payload = 1;
                                server_broadcast(-1, id, ACTION_CODE_LOBBY_SWITCH_GAME, &switch_payload, 1);
                                printf("[MODULE] Switch confirmation sent to client %d.\n", id);
                            }
                        }
                        else if (header.action == ACTION_CODE_LOBBY_CHAT) {
                            server_broadcast(0, id, ACTION_CODE_LOBBY_CHAT, buffer + sizeof(RUDPHeader_St), (u16)(len - sizeof(RUDPHeader_St)));
                        }
                        else if (active_module && active_module->on_action && active_game_state) {
                            active_module->on_action(
                                active_game_state, 
                                id, 
                                header.action, 
                                buffer + sizeof(RUDPHeader_St), 
                                (u16)(len - sizeof(RUDPHeader_St)), 
                                server_broadcast
                            );
                        }
                    }
                }
            }
        }

        check_timeouts();
        if (active_module && active_module->on_tick) {
            active_module->on_tick(active_game_state);
        }
    }

    if (active_module && active_module->destroy_instance) {
        active_module->destroy_instance(active_game_state);
    }
    
    close(master_socket);
    return 0;
}
