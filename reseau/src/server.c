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

#include "protocol.h"
#include "reseauAPI.h" // Contient la référence externe à lobby_module

#define PORT 8080

// Tableau des sockets clients (0 = vide)
int client_sockets[MAX_CLIENTS];

/**
 * Fonction de diffusion (Broadcast)
 * C'est elle qui envoie les données aux autres joueurs !
 */
void server_broadcast(int room_id, int exclude_id, uint8_t action, void *payload, uint16_t len) {
    uint8_t buffer[1024];
    PacketHeader header;
    header.room_id = htons(room_id);
    
    // Le serveur informe les autres que c'est "exclude_id" qui a fait l'action
    header.sender_id = htons((uint16_t)exclude_id); 
    
    header.action = action;
    header.length = htons(len);

    // On copie l'en-tête au début du buffer
    memcpy(buffer, &header, sizeof(PacketHeader));
    // On copie les données (payload) juste après
    if (len > 0 && payload != NULL) {
        memcpy(buffer + sizeof(PacketHeader), payload, len);
    }

    int total_size = sizeof(PacketHeader) + len;

    // 2. Envoyer à tous les clients (sauf celui qui a envoyé le message)
    for (int i = 0; i < MAX_CLIENTS; i++) {
        int sock = client_sockets[i];
        
        // Si la socket est valide ET ce n'est pas l'expéditeur
        if (sock > 0 && i != exclude_id) {
            send(sock, buffer, total_size, 0);
        }
    }
}

int main(int argc, char *argv[]) {
    (void)argc; (void)argv; // Silence warnings

    int master_socket, addrlen, new_socket, activity, valread, sd;
    int max_sd;
    struct sockaddr_in address;
    
    // Buffer de réception
    uint8_t buffer[1025];

    // Ensemble de descripteurs pour select()
    fd_set readfds;

    // Initialiser les sockets clients à 0
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = 0;
    }

    // 1. Créer la socket maître
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Echec socket");
        exit(EXIT_FAILURE);
    }

    // Options pour permettre de relancer le serveur tout de suite après l'avoir coupé
    int opt = 1;
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // 2. Configurer l'adresse (Port 8080, toutes les IPs)
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 3. Lier la socket (Bind)
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Echec bind");
        exit(EXIT_FAILURE);
    }

    // 4. Écouter (Listen)
    if (listen(master_socket, 3) < 0) {
        perror("Listen");
        exit(EXIT_FAILURE);
    }

    // Initialiser le module de jeu (le Lobby)
    void* game_state = lobby_module.create_instance();
    printf(">>> SERVEUR DÉMARRÉ SUR LE PORT %d <<<\n", PORT);
    printf("En attente de connexions...\n");

    addrlen = sizeof(address);

    // BOUCLE PRINCIPALE DU SERVEUR
    while (1) {
        // Nettoyer l'ensemble de sockets
        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        // Ajouter les sockets clients à l'ensemble
        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = client_sockets[i];
            if (sd > 0) FD_SET(sd, &readfds);
            if (sd > max_sd) max_sd = sd;
        }

        // Attendre une activité (connexion ou données)
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("Erreur select\n");
        }

        // --- NOUVELLE CONNEXION ---
        if (FD_ISSET(master_socket, &readfds)) {
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("Nouvelle connexion : socket fd %d, ip : %s, port : %d\n",
                   new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            // Ajouter à la liste des clients
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    printf("Ajouté à la liste des clients comme index %d\n", i);
                    break;
                }
            }
        }

        // --- DONNÉES REÇUES (IO) ---
        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = client_sockets[i];

            if (FD_ISSET(sd, &readfds)) {
                // Essayer de lire l'en-tête
                valread = read(sd, buffer, sizeof(PacketHeader));
                
                if (valread == 0) {
                    // Déconnexion
                    getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
                    printf("Hôte déconnecté, ip %s, port %d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                    close(sd);
                    client_sockets[i] = 0;
                    
                    // Prévenir le module Lobby
                    lobby_module.on_player_leave(game_state, i);
                }
                else {
                    // On a reçu un paquet !
                    PacketHeader *header = (PacketHeader*)buffer;
                    uint16_t dataLen = ntohs(header->length);
                    
                    // Lire la suite (le payload) si nécessaire
                    if (dataLen > 0) {
                        read(sd, buffer + sizeof(PacketHeader), dataLen);
                    }

                    // *** C'EST ICI QUE LA MAGIE OPÈRE ***
                    // On passe le relais au Lobby, et on lui donne notre fonction de broadcast
                    lobby_module.on_action(
                        game_state, 
                        i,              // ID du joueur qui a envoyé
                        header->action, 
                        buffer + sizeof(PacketHeader), // Pointeur vers les données
                        dataLen, 
                        server_broadcast // Notre fonction pour répondre aux autres !
                    );
                }
            }
        }
        
        // Mise à jour du monde (Tick)
        lobby_module.on_tick(game_state);
    }

    return 0;
}