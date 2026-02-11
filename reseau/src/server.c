/**
 * @file server.c
 * @brief Cœur du serveur de jeu utilisant des sockets TCP.
 * * Ce fichier gère la multiplexion des connexions clients, la réception des paquets
 * via le protocole défini et la communication avec le module de jeu (lobby).
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

#include "protocol.h"
#include "reseauAPI.h"

/** @brief Port d'écoute du serveur. */
#define PORT 8080

/** * @brief Tableau global stockant les descripteurs de sockets des clients connectés.
 * Un index à 0 indique un emplacement libre.
 */
int client_sockets[MAX_CLIENTS];

/**
 * @brief Envoie un paquet de données à tous les clients connectés, sauf un.
 * * Cette fonction construit un paquet complet (Header + Payload) et le diffuse.
 * Elle est passée en tant que callback au module de jeu.
 * * @param room_id ID de la salle concernée par le broadcast.
 * @param exclude_id ID du client à exclure de la diffusion (généralement l'émetteur).
 * @param action Type d'action à diffuser (défini dans LobbyAction).
 * @param payload Pointeur vers les données spécifiques à envoyer.
 * @param len Taille en octets des données du payload.
 */
void server_broadcast(int room_id, int exclude_id, uint8_t action, void *payload, uint16_t len) {
    uint8_t buffer[1024];
    PacketHeader header;
    
    // Conversion vers le format réseau (Big Endian)
    header.room_id = htons(room_id);
    header.sender_id = htons((uint16_t)exclude_id); 
    header.action = action;
    header.length = htons(len);

    // Construction du paquet dans le buffer
    memcpy(buffer, &header, sizeof(PacketHeader));
    if (len > 0 && payload != NULL) {
        memcpy(buffer + sizeof(PacketHeader), payload, len);
    }

    int total_size = sizeof(PacketHeader) + len;

    // Diffusion aux sockets valides
    for (int i = 0; i < MAX_CLIENTS; i++) {
        int sock = client_sockets[i];
        if (sock > 0 && i != exclude_id) {
            send(sock, buffer, total_size, 0);
        }
    }
}

/**
 * @brief Point d'entrée principal du serveur.
 * * Initialise la socket maître, configure le multiplexage avec select(),
 * accepte les nouvelles connexions et délègue le traitement des paquets
 * reçus au module lobby_module.
 * * @param argc Nombre d'arguments.
 * @param argv Tableau des arguments.
 * @return int Code de sortie du programme.
 */
int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    int master_socket, addrlen, new_socket, activity, valread, sd;
    int max_sd;
    struct sockaddr_in address;
    uint8_t buffer[1025];
    fd_set readfds;

    // Initialisation du tableau des clients
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = 0;
    }

    // Création de la socket maître (IPv4, TCP)
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Echec socket");
        exit(EXIT_FAILURE);
    }

    // Autorise la réutilisation rapide du port après un redémarrage
    int opt = 1;
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Liaison au port
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Echec bind");
        exit(EXIT_FAILURE);
    }

    // Mise en mode écoute
    if (listen(master_socket, 3) < 0) {
        perror("Listen");
        exit(EXIT_FAILURE);
    }

    // Initialisation de l'état du jeu via l'API
    void* game_state = lobby_module.create_instance();
    printf(">>> SERVEUR DÉMARRÉ SUR LE PORT %d <<<\n", PORT);

    addrlen = sizeof(address);

    // Boucle de service infinie
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        // Préparation du set de descripteurs pour select()
        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = client_sockets[i];
            if (sd > 0) FD_SET(sd, &readfds);
            if (sd > max_sd) max_sd = sd;
        }

        // Attente d'un événement sur l'une des sockets
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("Erreur select\n");
        }

        // Gestion d'une nouvelle connexion entrante
        if (FD_ISSET(master_socket, &readfds)) {
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            // Ajout du nouveau client dans le premier emplacement vide
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    break;
                }
            }
        }

        // Lecture des données pour chaque client actif
        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = client_sockets[i];

            if (FD_ISSET(sd, &readfds)) {
                // Lecture de l'en-tête (taille fixe)
                valread = read(sd, buffer, sizeof(PacketHeader));
                
                if (valread == 0) {
                    // Gestion de la déconnexion
                    close(sd);
                    client_sockets[i] = 0;
                    lobby_module.on_player_leave(game_state, i);
                }
                else {
                    PacketHeader *header = (PacketHeader*)buffer;
                    uint16_t dataLen = ntohs(header->length);
                    
                    // Lecture du payload si présent
                    if (dataLen > 0) {
                        read(sd, buffer + sizeof(PacketHeader), dataLen);
                    }

                    // Transmission de l'action au module métier
                    lobby_module.on_action(
                        game_state, 
                        i, 
                        header->action, 
                        buffer + sizeof(PacketHeader), 
                        dataLen, 
                        server_broadcast
                    );
                }
            }
        }
        
        // Mise à jour périodique du jeu
        lobby_module.on_tick(game_state);
    }

    return 0;
}