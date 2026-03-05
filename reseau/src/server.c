/**
 * @file server.c
 * @brief Cœur du serveur de jeu utilisant des datagrammes UDP et RUDP (Bitfield ACK).
 * Ce fichier gère la réception sans connexion, le routage des paquets via l'IP/Port
 * et la communication avec le module de jeu (lobby) sans Head-of-Line Blocking.
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-05
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <stdbool.h>

#include "rudp_core.h"
#include "reseauAPI.h"

/** @brief Port d'écoute statique du serveur. */
#define PORT 8080
/** @brief Temps maximum d'inactivité avant éjection (en microsecondes). */
#define TIMEOUT_US 60000000

/**
 * @brief Représentation virtuelle d'un client en architecture UDP.
 */
typedef struct {
    bool active;
    struct sockaddr_in address;
    RUDP_Connection rudp_state;
    struct timeval last_seen;
} UDP_Client;

// --- VARIABLES GLOBALES ---
int master_socket = -1;
UDP_Client clients[MAX_CLIENTS];

/**
 * @brief Résout l'identité d'un client par son adresse physique.
 * Alloue dynamiquement un nouvel emplacement si l'adresse est inconnue.
 * @return L'index du client (0 à MAX_CLIENTS-1) ou -1 si le serveur est plein.
 */
int find_or_create_client(struct sockaddr_in *addr) {
    // 1. Recherche d'une session existante
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(clients[i].active &&
           clients[i].address.sin_addr.s_addr == addr->sin_addr.s_addr &&
           clients[i].address.sin_port == addr->sin_port) {
            return i;
        }
    }

    // 2. Création d'une nouvelle session si aucune correspondance
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(!clients[i].active) {
            clients[i].active = true;
            clients[i].address = *addr;
            RUDP_InitConnection(&clients[i].rudp_state);
            gettimeofday(&clients[i].last_seen, NULL);
            printf("[SERVEUR] Nouveau joueur virtuel connecté sur le slot %d\n", i);
            return i;
        }
    }

    return -1; // Serveur saturé
}

/**
 * @brief Envoie un paquet de données à tous les clients connectés, sauf un.
 * Encapsule la charge utile avec l'en-tête RUDP pour assurer l'acquittement redondant.
 */

void server_broadcast(int room_id, int exclude_id, uint8_t action, void *payload, uint16_t len) {
    (void)room_id; 
    uint8_t buffer[2048];
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && i != exclude_id) {
            RUDP_Header header;
            RUDP_GenerateHeader(&clients[i].rudp_state, action, &header);
            header.sender_id = htons(exclude_id); // L'ID de celui qui a bougé
            
            memcpy(buffer, &header, sizeof(RUDP_Header));
            if (len > 0 && payload != NULL) {
                memcpy(buffer + sizeof(RUDP_Header), payload, len);
            }
            
            int total_size = sizeof(RUDP_Header) + len;
            ssize_t sent = sendto(master_socket, buffer, total_size, 0, (struct sockaddr *)&clients[i].address, sizeof(struct sockaddr_in));
            
            // LOG DE DEBUG SERVEUR
            if (sent > 0) {
                printf("[BROADCAST] Envoyé %ld octets au Client %d (Action: %d)\n", sent, i, action);
            } else {
                perror("[ERREUR] Échec du sendto");
            }
        }
    }
}

/**
 * @brief Purge les clients silencieux (Timeout UDP).
 */
void check_client_timeouts(void* game_state) {
    struct timeval now;
    gettimeofday(&now, NULL);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            long long elapsed = (now.tv_sec - clients[i].last_seen.tv_sec) * 1000000LL +
                                (now.tv_usec - clients[i].last_seen.tv_usec);

            if (elapsed > TIMEOUT_US) {
                printf("[SERVEUR] Timeout du client %d (%.1f secondes d'inactivité)\n", i, elapsed / 1000000.0);
                clients[i].active = false;
                lobby_module.on_player_leave(game_state, i);
            }
        }
    }
}

/**
 * @brief Point d'entrée principal du serveur UDP.
 */
int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    struct sockaddr_in address;
    fd_set readfds;

    // Initialisation de la mémoire des clients
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].active = false;
    }

    // Création de la socket MAÎTRE en mode Datagramme UDP
    if ((master_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == 0) {
        perror("Echec création socket UDP");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Liaison physique au port
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Echec bind UDP");
        exit(EXIT_FAILURE);
    }

    void* game_state = lobby_module.create_instance();
    printf(">>> SERVEUR RUDP DÉMARRÉ SUR LE PORT %d <<<\n", PORT);

    // Boucle de service isochrone
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);

        // Paramétrage du Tickrate (ex: 60 FPS = ~16.6ms d'attente maximum)
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 16000;

        int activity = select(master_socket + 1, &readfds, NULL, NULL, &timeout);

        if ((activity < 0) && (errno != EINTR)) {
            printf("Erreur select\n");
        }

        // Si des données sont présentes sur l'interface réseau
        if (activity > 0 && FD_ISSET(master_socket, &readfds)) {
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);
            uint8_t buffer[2048];

            // Extraction atomique du datagramme complet en une seule instruction
            ssize_t valread = recvfrom(master_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &addr_len);

            if (valread >= (ssize_t)sizeof(RUDP_Header)) {
                int client_id = find_or_create_client(&client_addr);

                if (client_id != -1) {
                    RUDP_Header *header = (RUDP_Header*)buffer;
                    gettimeofday(&clients[client_id].last_seen, NULL); // Rafraîchissement du "Heartbeat"

                    // Validation Mathématique (Filtre anti-Jitter / anti-Duplicata)
                    if (RUDP_ProcessIncoming(&clients[client_id].rudp_state, header)) {
                        uint16_t dataLen = valread - sizeof(RUDP_Header);

                        // Transmission immédiate au moteur physique du jeu
                        lobby_module.on_action(
                            game_state,
                            client_id,
                            header->action,
                            buffer + sizeof(RUDP_Header),
                            dataLen,
                            server_broadcast
                        );
                    }
                }
            }
        }

        // Maintien du cycle de vie serveur
        check_client_timeouts(game_state);
        lobby_module.on_tick(game_state);
    }

    return 0;
}
