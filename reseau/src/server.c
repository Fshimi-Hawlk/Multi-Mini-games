// server/main.c
#include <stdio.h>
#include <string.h>   // strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   // close
#include <arpa/inet.h>    // close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> // FD_SET, FD_ISSET, FD_ZERO macros

#include "protocol.h"
#include "game_interface.h"

#define TRUE   1
#define FALSE  0
#define PORT 8080
#define MAX_CLIENTS 30     // Max joueurs simultanés sur le serveur
#define MAX_ROOMS 50
#define BUFFER_SIZE 1024

// On déclare nos modules (plugins)
extern GameInterface lobby_module; 
// extern GameInterface snake_module; 

/*
   SCHEMA : STRUCTURE D'UNE ROOM (Salle)
   =====================================
   Une Room fait le lien entre une "place" dans le serveur (ID)
   et un jeu spécifique (Logique + Données).

   +-----------------+       +---------------------------+
   |   Room Struct   |       |      GameInterface        | (Code statique)
   |-----------------|       |---------------------------|
   | id: 42          |       | .game_name = "snake"      |
   | is_active: 1    |       | .on_action = snake_func() | <--- Pointeur vers fonction
   | logic: *ptr  ---|-----> | .on_tick   = snake_tick() |
   | state: *ptr  ---|--+    +---------------------------+
   +-----------------+  |
                        |    +---------------------------+
                        +--> |      SnakeState           | (Données dynamiques du tas)
                             |---------------------------|
                             | score_p1: 10              |
                             | map: [[0,1,0]...]         |
                             +---------------------------+
*/
typedef struct {
    int id;
    int is_active;
    GameInterface *logic; 
    void *state;          
} Room;

Room rooms[MAX_ROOMS];

// Pour savoir dans quelle room est chaque socket client
// Index = Socket ID, Valeur = Room ID
int client_room_map[1024]; 

void init_rooms() {
    /*
       SCHEMA : TABLEAU DES ROOMS AU DEMARRAGE
       =======================================
       Index [0] est réservé. Les autres sont libres.

       [Room 0] (ACTIVE)        [Room 1] (INACTIVE)      [Room 2] ...
       +-----------------+      +-----------------+      +-----+
       | ID: 0           |      | ID: 1           |      | ... |
       | Logic: Lobby    |      | Logic: NULL     |      |     |
       | State: LobbyData|      | State: NULL     |      |     |
       +-----------------+      +-----------------+      +-----+
             ^
             |__ C'est ici que les joueurs arrivent par défaut
    */
    rooms[0].id = 0;
    rooms[0].is_active = 1;
    rooms[0].logic = &lobby_module;
    rooms[0].state = lobby_module.create_instance();
    
    // Initialiser le mapping à -1 (Pas de room)
    for(int i=0; i<1024; i++) client_room_map[i] = -1;
}

int main(int argc , char *argv[])
{
    int master_socket , addrlen , new_socket , client_socket[MAX_CLIENTS] ,
          max_clients = MAX_CLIENTS , activity, i , valread , sd;
    int max_sd;
    struct sockaddr_in address;
    uint8_t buffer[BUFFER_SIZE];  

    // Set de descripteurs de fichiers (la liste des caméras à surveiller)
    fd_set readfds;

    // Initialiser le tableau des clients à 0 (0 = libre)
    for (i = 0; i < max_clients; i++) {
        client_socket[i] = 0;
    }

/*
   SCHEMA : CYCLE DE VIE SOCKET (Server Side)
   ==========================================
     +-----------------+
     |    socket()     |  <-- Création
     +--------+--------+
              |
     +--------v--------+
     |     bind()      |  <-- Attachement au Port 8080
     +--------+--------+
              |
     +--------v--------+
     |    listen()     |  <-- Ouverture des vannes
     +--------+--------+
              |
     +--------v--------+
     |    select()     |  <-- ATTENTE INTELLIGENTE (Multiplexage)
     +--------+--------+      Au lieu de bloquer sur un seul accept()
              |
      (Si Master active)        (Si Client active)
              |                         |
     +--------v--------+       +--------v--------+
     |    accept()     |       |  read/recv()    |
     +-----------------+       +-----------------+
*/

    // 1. Création de la socket maître
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Options pour relancer le serveur vite (SO_REUSEADDR)
    int opt = 1;
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 ) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    if (listen(master_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    init_rooms();
    printf("--- SERVEUR MULTI-GAMES (Port %d) ---\n", PORT);
    printf("Lobby 'Club Penguin' actif dans la Room 0\n");
    printf("En attente de connexions...\n");

/*
       SURVEILLANCE "SELECT()" (Multiplexage)
       ======================================
       
       [ Sockets actives ]
       +---+   +---+   +---+   +---+
       |SV |   |C_1|   |C_2|   |C_3|  (SV = Serveur Master, C = Clients)
       +---+   +---+   +---+   +---+
         |       |       |       |
         v       v       v       v
      +-----------------------------+
      |      FONCTION SELECT()      | <--- BLOQUANT (avec Timeout)
      |   "Réveille-toi si l'un    |      
      |    d'eux reçoit des data"   |
      +-------------+---------------+
                    |
        +-----------+-----------+
        | Réveil ! Qui a parlé ?|
        +-----------+-----------+
        |                       |
    (Si SV)                 (Si C_2)
       |                       |
  [ ACCEPT() ]            [ READ() ]
  Nouvel ami              Traiter le paquet
*/

    // --- BOUCLE INFINIE PRINCIPALE ---
    while(TRUE) {
        // A. Nettoyer le set de sockets
        FD_ZERO(&readfds);

        // B. Ajouter la socket maître
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        // C. Ajouter les sockets des clients existants
        for ( i = 0 ; i < max_clients ; i++) {
            sd = client_socket[i]; // Récupérer le descripteur

            if(sd > 0) FD_SET( sd , &readfds);

            if(sd > max_sd) max_sd = sd;
        }

        // D. Timeout pour le select (IMPORTANT pour le TickRate des jeux)
        // Si on met NULL, select bloque à l'infini tant que personne ne parle.
        // Ici, on force un réveil toutes les 50ms pour mettre à jour la physique.
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 50000; // 50ms

        // E. ATTENTE
        activity = select( max_sd + 1 , &readfds , NULL , NULL , &timeout);

        if ((activity < 0) && (errno!=EINTR)) {
            printf("Erreur de select");
        }

        // --- CAS 1 : C'est la Master Socket (NOUVEAU CLIENT) ---
        if (FD_ISSET(master_socket, &readfds)) {
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("Nouvelle connexion , socket fd: %d , ip: %s , port: %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

            // Ajouter le nouveau client dans le tableau
            for (i = 0; i < max_clients; i++) {
                if( client_socket[i] == 0 ) {
                    client_socket[i] = new_socket;
                    
                    // Par défaut, on le met dans le LOBBY (Room 0)
                    client_room_map[new_socket] = 0;
                    rooms[0].logic->on_action(rooms[0].state, new_socket, LOBBY_JOIN, NULL, 0);
                    break;
                }
            }
        }

        // --- CAS 2 : C'est une socket Client (RECEPTION DONNEES) ---
        for (i = 0; i < max_clients; i++) {
            sd = client_socket[i];

            if (FD_ISSET( sd , &readfds)) {
                
                // Lecture (non bloquante ici car select a dit "ok")
                if ((valread = read( sd , buffer, BUFFER_SIZE)) == 0) {
                    
                    // a) Déconnexion
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                    printf("Client déconnecté , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

                    // Notifier le jeu que le joueur part
                    int current_room = client_room_map[sd];
                    if (current_room != -1 && rooms[current_room].is_active) {
                         if (rooms[current_room].logic->on_player_leave) {
                             rooms[current_room].logic->on_player_leave(rooms[current_room].state, sd);
                         }
                    }

                    close( sd );
                    client_socket[i] = 0;
                    client_room_map[sd] = -1;
                }
                else {
                    // b) Traitement du paquet reçu

                    /*
                       SCHEMA : DECODAGE DU PAQUET (BUFFER)
                       ====================================
                       Le buffer est une suite d'octets.
                       Mémoire (buffer[]) :
                       [ 0x00 0x00 ] [ 0x02 ] [ 0x00 0x08 ] [ 0x48 0x65 ... ]
                       ^             ^        ^             ^
                       | RoomID (0)  | Action | Length (8)  | <--- PAYLOAD
                       +-------------+--------+-------------+
                    */

                    if (valread >= sizeof(PacketHeader)) {
                        PacketHeader *header = (PacketHeader *)buffer;
                        void *payload = buffer + sizeof(PacketHeader);
                        
                        uint16_t room_id = ntohs(header->room_id);
                        uint16_t length = ntohs(header->length);

                        // Mise à jour de la map si le client change de salle
                        if (room_id != client_room_map[sd] && room_id < MAX_ROOMS) {
                            client_room_map[sd] = room_id;
                        }

                        // Dispatcher vers la bonne Room
                        if (room_id < MAX_ROOMS && rooms[room_id].is_active) {
                            
                            /* SCHEMA : LE DISPATCHER (ROUTER)
                               ===============================
                               [Paquet] -> [ Switch RoomID ] -> [ Module.on_action() ]
                            */

                            rooms[room_id].logic->on_action(
                                rooms[room_id].state, 
                                sd, // Player ID = Socket FD
                                header->action, 
                                payload, 
                                length
                            );
                        }
                    }
                }
            }
        }
        
        // --- TICK RATE (Physique des jeux) ---
        // Grâce au timeout du select, on passe ici toutes les 50ms environ
        // On met à jour le lobby et les jeux actifs
        for(int r=0; r < MAX_ROOMS; r++) {
            if (rooms[r].is_active && rooms[r].logic->on_tick) {
                rooms[r].logic->on_tick(rooms[r].state);
            }
        }
    }

    return 0;
}