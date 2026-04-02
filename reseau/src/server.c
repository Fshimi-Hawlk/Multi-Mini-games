/**
<<<<<<< HEAD
 * @file server.c
 * @brief Serveur RUDP autoritaire centralisé.
 * 
 * Ce serveur gère le multiplexage des connexions entrantes sur un port unique (8080).
 * Il supporte la découverte locale (Room Query) et le routage des messages vers 
 * les modules de jeu (Lobby, King for Four, etc.) via une interface générique.
 * 
 * @author i-Charlys
 * @date 2026-03-31
 */
=======
    @file server.c
    @author i-Charlys (CAILLON Charles)
    @date 2026-03-18
    @date 2026-03-30
    @brief Authoritative RUDP server with multi-module support.

    Manages multiple client connections, routes packets to the active game module
    (lobby or mini-games), handles discovery queries, timeouts and broadcasting.

    Communication diagram :
    [CLIENT 1] <----RUDP----> [ SERVEUR ] <----RUDP----> [CLIENT 2]
                                  |
                                  v
                       +---------------------+
                       |   ROUTING MODULES   |
                       +---------------------+
                       | action = SWITCH?    | ----> Change active_module
                       | action = GAME_DATA? | ----> on_action(active_game)
                       +---------------------+
    

    @note Currently supports lobby and King-for-Four. Easy to extend for more modules.
*/
>>>>>>> origin/mgit-PR1-20-03

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#include "APIs/generalAPI.h"
#include "networkInterface.h"
<<<<<<< HEAD
#include "APIs/generalAPI.h"

#define SERVER_PORT 8080                                /**< Port unique pour le Jeu et la Découverte. */
#define TICK_US 16666                                   /**< Fréquence de mise à jour (60 Hz). */
#define TIMEOUT_US 60000000LL                            /**< Délai de déconnexion automatique (60 secondes). */
#define SERVER_NAME "MMG-Lobby-Server"                  /**< Nom du serveur. */
#define MAX_PAYLOAD_SIZE (2048 - sizeof(RUDPHeader_St)) /**< Taille maximale de la charge utile. */

// Instances des modules liées à la compilation
extern GameServerInterface_St lobby_module; 
extern GameServerInterface_St king_module;  
extern GameServerInterface_St chess_module;
extern GameServerInterface_St rubik_module;

/**
 * @struct UDP_Client
 * @brief Données de session pour chaque client connecté.
 */

typedef struct {
    bool active;                                  /**< Indique si le client est actif. */
    struct sockaddr_in address;                   /**< Adresse IP et port du client. */
    RUDPConnection_St rudp_state;                 /**< État RUDP du client. */
    struct timeval last_seen;                     /**< Dernière fois que le client a été vu. */
} UDP_Client;

int rudp_socket = -1;                             /**< Socket UDP utilisé pour le serveur. */
UDP_Client clients[MAX_CLIENTS];                  /**< Tableau des clients connectés. */
GameServerInterface_St *active_module = NULL;     /**< Module actif pour la gestion des jeux. */
void* active_game_state = NULL;                   /**< État actuel du jeu. */

/**
 * @brief Recherche un client existant ou alloue un nouveau slot.
 */
int find_or_create_client(struct sockaddr_in *addr) {
    
    
    /** Recherche un client existant avec la même adresse IP et port */
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(clients[i].active && 
           clients[i].address.sin_addr.s_addr == addr->sin_addr.s_addr &&     
           clients[i].address.sin_port == addr->sin_port) {
            return i;
        }
    }
    
    /** Alloue un nouveau slot pour un client non actif */
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(!clients[i].active) {
            clients[i].active = true;
            clients[i].address =    *addr;
            rudpInitConnection(&clients[i].rudp_state);
            gettimeofday(&clients[i].last_seen, NULL);
            return i;
        }
    }
    
    /** Aucun slot disponible, retourne -1 */
=======
#include "rudp_core.h"
#include "logger.h"

// ────────────────────────────────────────────────
// Configuration constants
// ────────────────────────────────────────────────

#define MACROSECONDS_IN_A_SECOND 1000000LL

#define SERVER_PORT             8080
#define CLIENT_TIMEOUT_US       (60 * MACROSECONDS_IN_A_SECOND)
#define SERVER_DISPLAY_NAME     "Multi-Mini-Games Server"
#define MAX_PAYLOAD_SIZE        (2048 - sizeof(RUDPHeader_St))

RUDPConnection_St serverConnection;

// ────────────────────────────────────────────────
// Action codes (lobby-specific)
// ────────────────────────────────────────────────

enum {
    ACTION_CODE_LOBBY_MOVE          = firstAvailableActionCode,
    ACTION_CODE_LOBBY_ROOM_QUERY,
    ACTION_CODE_LOBBY_ROOM_INFO,
    ACTION_CODE_LOBBY_CHAT,
    ACTION_CODE_LOBBY_SWITCH_GAME
};

// ────────────────────────────────────────────────
// Internal client representation
// ────────────────────────────────────────────────

/**
    @brief Server-side representation of a connected client.
*/
typedef struct {
    bool              active;           ///< True when slot is occupied
    struct sockaddr_in address;         ///< Client UDP endpoint
    RUDPConnection_St rudpState;        ///< Per-client RUDP state
    struct timeval    lastSeen;         ///< Last activity timestamp
} UDPClient_St;

// ────────────────────────────────────────────────
// Globals
// ────────────────────────────────────────────────

static UDPClient_St clients[MAX_CLIENTS] = {0};
static int          masterSocket = -1;  ///< Main server listening socket

// Active game module (lobby or mini-game)
static GameServerInterface_St* activeModule = NULL;
static void*                   activeGameState = NULL;

// ────────────────────────────────────────────────
// External module interfaces (defined in their .c files)
// ────────────────────────────────────────────────

extern GameServerInterface_St lobbyServerInterface;
extern GameServerInterface_St bingoServerInterface;
extern GameServerInterface_St kingServerInterface;

static GameServerInterface_St* gameInterfaces[__miniGameCount] = {
    [MINI_GAME_LOBBY] = &lobbyServerInterface,
    [MINI_GAME_KFF] = &kingServerInterface,
    [MINI_GAME_BINGO] = &bingoServerInterface,
};

// ────────────────────────────────────────────────
// Helper functions
// ────────────────────────────────────────────────

/**
    @brief Finds existing client by address or creates a new slot.
    @param addr Client address
    @return Client index (0..MAX_CLIENTS-1) or -1 if server is full
*/
static int findOrCreateClient(struct sockaddr_in* addr) {
    // Look for existing client
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].active &&
            clients[i].address.sin_addr.s_addr == addr->sin_addr.s_addr &&
            clients[i].address.sin_port == addr->sin_port) {
            return i;
        }
    }

    // Create new client
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (!clients[i].active) {
            clients[i].active = true;
            clients[i].address = *addr;
            rudpInitConnection(&clients[i].rudpState);
            gettimeofday(&clients[i].lastSeen, NULL);

            log_info("Client slot %d allocated for %s:%d",
                     i, inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
            return i;
        }
    }

    log_warn("Server is full - rejected new client");
>>>>>>> origin/mgit-PR1-20-03
    return -1;
}

/**
<<<<<<< HEAD
 * @brief Envoie un message à un ou plusieurs clients.
 * 
 * @param room_id    >= 0 : Broadcast dans la room (exclut sender_id).
 *                   < 0  : Unicast vers le client ID (-room_id - 1).
 * @param sender_id  ID de l'expéditeur à inscrire dans le header.
 */
void server_broadcast(int room_id, int sender_id, u8 action, const void *payload, u16 len) {
    u8 buffer[2048];                                        /** Buffer pour le paquet à envoyer */
    if (len > MAX_PAYLOAD_SIZE) len = MAX_PAYLOAD_SIZE;     /** Limite la taille de la charge utile */

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            bool should_send = false;
            if (room_id >= 0) {
                if (i != sender_id) should_send = true;
            } else {
                int target_id = (-room_id) - 1;
                if (i == target_id) should_send = true;
            }
    
            /** Envoie le message au client si should_send est vrai */
            if (should_send) {
                RUDPHeader_St header;
                rudpGenerateHeader(&clients[i].rudp_state, action, &header);
                header.sender_id = htons((u16)sender_id); 
                memcpy(buffer, &header, sizeof(RUDPHeader_St));
                if (len > 0 && payload) memcpy(buffer + sizeof(RUDPHeader_St), payload, len);
                sendto(rudp_socket, buffer, sizeof(RUDPHeader_St) + len, 0, (struct sockaddr *)&clients[i].address, sizeof(struct sockaddr_in));
            }
=======
    @brief Broadcasts or unicasts a message to clients.
    @param roomId -1 = unicast, >=0 = broadcast (exclude excludeId)
    @param excludeId Client to exclude (broadcast) or target (unicast)
    @param action Action code
    @param payload Message payload
    @param len Payload length
*/
static void serverBroadcast(int roomId, int excludeId, u8 action, const void* payload, u16 len) {
    u8 buffer[2048];
    if (len > MAX_PAYLOAD_SIZE) len = MAX_PAYLOAD_SIZE;

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        bool shouldSend = false;

        if (roomId == UNICAST) {
            // Unicast to specific client
            if (i == excludeId) shouldSend = true;
        } else {
            // Broadcast to everyone except excludeId
            if (clients[i].active && i != excludeId) shouldSend = true;
        }

        if (shouldSend) {
            RUDPHeader_St header;
            rudpGenerateHeader(&clients[i].rudpState, action, &header);
            header.sender_id = htons((u16)excludeId);  // Note: using excludeId as sender for now

            memcpy(buffer, &header, sizeof(RUDPHeader_St));
            if (len > 0 && payload != NULL) {
                memcpy(buffer + sizeof(RUDPHeader_St), payload, len);
            }

            sendto(masterSocket, buffer, sizeof(RUDPHeader_St) + len, 0,
                   (struct sockaddr*)&clients[i].address, sizeof(struct sockaddr_in));
>>>>>>> origin/mgit-PR1-20-03
        }
    }
}

/**
<<<<<<< HEAD
 * @brief Répond aux requêtes de découverte réseau.
 */
void handle_discovery_query(struct sockaddr_in *client_addr) {
    RUDPHeader_St response;                                                     /** Header de réponse pour les requêtes de découverte */
    RUDPConnection_St temp_conn;                                                /** Connexion temporaire pour générer le header */
    rudpInitConnection(&temp_conn);                                             /** Initialise la connexion temporaire */
    rudpGenerateHeader(&temp_conn, ACTION_CODE_LOBBY_ROOM_INFO, &response);     /** Génère le header de réponse */
    response.sender_id = htons(999);                                            /** ID spécial pour le serveur */
    u8 buffer[512];
    memcpy(buffer, &response, sizeof(RUDPHeader_St));                           /** Copie le header dans le buffer */
    char info[256];

    /** Informations sur le serveur */
    snprintf(info, sizeof(info), "%s [%s]", SERVER_NAME, active_module->game_name ? active_module->game_name : "Lobby");
        
    memcpy(buffer + sizeof(RUDPHeader_St), info, strlen(info) + 1);             /** Copie les informations dans le buffer */
    sendto(rudp_socket, buffer, sizeof(RUDPHeader_St) + strlen(info) + 1, 0, (struct sockaddr *)client_addr, sizeof(struct sockaddr_in));
}

/**
 * @brief Nettoie les sessions inactives.
 */
void check_timeouts(void) {
    struct timeval now;
    gettimeofday(&now, NULL);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            long long elapsed = (long long)(now.tv_sec - clients[i].last_seen.tv_sec) * 1000000LL + (now.tv_usec - clients[i].last_seen.tv_usec);
            if (elapsed > TIMEOUT_US) {
                printf("[SERVER] Client %d déconnecté (timeout)\n", i);
                clients[i].active = false;
                server_broadcast(0, i, ACTION_CODE_QUIT_GAME, NULL, 0);
                if (active_module && active_module->on_player_leave) active_module->on_player_leave(active_game_state, i);
=======
    @brief Handles server discovery queries from clients.
    @param clientAddr Address of querying client
*/
static void handleDiscoveryQuery(struct sockaddr_in* clientAddr) {
    RUDPHeader_St response;
    RUDPConnection_St tempConn;
    rudpInitConnection(&tempConn);

    rudpGenerateHeader(&tempConn, ACTION_CODE_LOBBY_ROOM_INFO, &response);
    response.sender_id = htons(999);

    u8 buffer[512];
    memcpy(buffer, &response, sizeof(RUDPHeader_St));

    char info[256];
    const char* moduleName = (activeModule && activeModule->game_name)
                           ? activeModule->game_name : "Unknown";

    snprintf(info, sizeof(info), "%s [%s]", SERVER_DISPLAY_NAME, moduleName);
    memcpy(buffer + sizeof(RUDPHeader_St), info, strlen(info) + 1);

    sendto(masterSocket, buffer, sizeof(RUDPHeader_St) + strlen(info) + 1, 0,
           (struct sockaddr*)clientAddr, sizeof(struct sockaddr_in));
}

/**
    @brief Checks for timed-out clients and disconnects them.
*/
static void checkTimeouts(void) {
    struct timeval now;
    gettimeofday(&now, NULL);

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].active) {
            long long elapsed = \
                (now.tv_sec - clients[i].lastSeen.tv_sec) * MACROSECONDS_IN_A_SECOND 
              + (now.tv_usec - clients[i].lastSeen.tv_usec);

            if (elapsed > CLIENT_TIMEOUT_US) {
                log_info("Client %d timed out", i);
                clients[i].active = false;

                // Notify others
                serverBroadcast(UNICAST, i, ACTION_CODE_QUIT_GAME, NULL, 0);

                if (activeModule && activeModule->on_player_leave) {
                    activeModule->on_player_leave(activeGameState, i);
                }
>>>>>>> origin/mgit-PR1-20-03
            }
        }
    }
}

<<<<<<< HEAD

/**
 * @brief Met une socket en mode non-bloquant.
 */
void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);                              /** Récupère les flags actuels de la socket */
    if (flags != -1) fcntl(fd, F_SETFL, flags | O_NONBLOCK);        /* Met la socket en mode non-bloquant */
}


/**
 * @brief Retourne le temps actuel en microsecondes.
 */
long long get_time_us(void) {
    struct timeval tv;                                          /** Structure pour stocker le temps actuel */
    gettimeofday(&tv, NULL);                                    /** Récupère le temps actuel */
    return (long long)tv.tv_sec * 1000000LL + tv.tv_usec;       /** Retourne le temps actuel en microsecondes */
}

/**
 * @brief Point d'entrée principal du serveur 
 */
int main(void) {
    struct sockaddr_in rudp_addr;                                             /** Adresse du serveur */
    memset(clients, 0, sizeof(clients));                                      /** Initialise tous les clients à 0 */

    if ((rudp_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {                 /** Crée la socket UDP */
        perror("Échec création socket");                                      /** Affiche l'erreur si la création échoue */
        exit(1);
    }

    int opt = 1;                                                            /** Option pour réutiliser l'adresse */
    setsockopt(rudp_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));   /** Permet de réutiliser l'adresse du serveur */
    int brd = 1;                                                            /** Option pour envoyer des broadcasts */
    setsockopt(rudp_socket, SOL_SOCKET, SO_BROADCAST, &brd, sizeof(brd));   /** Permet d'envoyer des broadcasts */
=======
// ────────────────────────────────────────────────
// Main
// ────────────────────────────────────────────────

/**
    @brief Server program entry point.
    @return 0 on normal exit
*/
int main(void) {
    struct sockaddr_in serverAddr;

    // Initialize clients
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        clients[i].active = false;
    }

    // Create socket
    if ((masterSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Bind socket
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

    if (bind(masterSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    // Start with lobby as active module
    activeModule = &lobbyServerInterface;

    if (activeModule && activeModule->create_instance) {
        activeGameState = activeModule->create_instance();
    } else {
        log_error("Active module does not implement create_instance");
        exit(1);
    }

    log_info("[SERVER %s] RUDP SERVER STARTED ON PORT %d", 
        getPreciseTimeString(), SERVER_PORT
    );

    log_info("[SERVER %s] ACTIVE MODULE: %s",
        getPreciseTimeString(), activeModule->game_name ? activeModule->game_name : "NULL"
    );
>>>>>>> origin/mgit-PR1-20-03

    rudp_addr.sin_family = AF_INET;                                         /** Famille d'adresse (IPv4) */
    rudp_addr.sin_addr.s_addr = INADDR_ANY;                                 /** Adresse IP du serveur (INADDR_ANY pour écouter sur toutes les interfaces) */
    rudp_addr.sin_port = htons(SERVER_PORT);                                /** Port du serveur */

    /** Écoute sur toutes les interfaces sur le port spécifié */
    if (bind(rudp_socket, (struct sockaddr *)&rudp_addr, sizeof(rudp_addr)) < 0) {
        perror("Échec bind");
        exit(1);
    }

    
    set_nonblocking(rudp_socket);                                            /** Met la socket en mode non-bloquante */

    active_module = &lobby_module;                                           /** Module actif pour la gestion des jeux */
    active_game_state = active_module->create_instance();                    /** État actuel du jeu */
    

    printf(">>> SERVEUR MMG DÉMARRÉ SUR LE PORT %d <<<\n", SERVER_PORT);
    long long next_tick = get_time_us() + TICK_US;                           /** Prochain tick à exécuter */

    
    /** Boucle principale du serveur */
    while (1) {
<<<<<<< HEAD
        long long now = get_time_us();                                       /** Temps actuel en microsecondes */
        
        if (now >= next_tick) {                                              /** Si le temps actuel est supérieur ou égal au prochain tick, exécuter le tick */
            check_timeouts();
            if (active_module->on_tick) active_module->on_tick(active_game_state);
            next_tick += TICK_US;
            if (next_tick < now) next_tick = now + TICK_US;
        }

        fd_set readfds;                                                      /** Ensemble des sockets à surveiller pour les données entrantes */
        FD_ZERO(&readfds);                                                   /** Initialiser l'ensemble des sockets à surveiller */
        FD_SET(rudp_socket, &readfds);                                       /** Ajouter le socket UDP au set pour la surveillance */
        
        long long remaining = next_tick - now;                               /** Temps restant avant le prochain tick, en microsecondes */
        if (remaining < 0) remaining = 0;                                    /** Si le temps restant est négatif, le mettre à 0 */
        
        struct timeval timeout;                                              /** Timeout pour la fonction select, en secondes et microsecondes */
        timeout.tv_sec = remaining / 1000000LL;                              /** Secondes du timeout */
        timeout.tv_usec = remaining % 1000000LL;                             /** Microsecondes du timeout */
        int max_fd = rudp_socket;                                            /** Maximum des descripteurs de fichier surveillés */

        if (select(max_fd + 1, &readfds, NULL, NULL, &timeout) > 0) {        /** Appel à select avec le timeout calculé */
            if (FD_ISSET(rudp_socket, &readfds)) {                           /** Le socket a des données disponibles pour la lecture */
                struct sockaddr_in c_addr; socklen_t a_len; u8 buf[2048];    /** Buffer pour les données reçues et adresse du client */
                ssize_t len;                                                 /** Taille des données reçues */
                
                
                /** Boucle pour lire toutes les données disponibles */
                while (1) {
                    a_len = sizeof(c_addr);
                    len = recvfrom(rudp_socket, buf, sizeof(buf), 0, (struct sockaddr*)&c_addr, &a_len);
                    if (len < 0) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                        break;
                    }
                    
                    /** Si la taille des données reçues est inférieure à la taille du header, on ignore */
                    if (len < (ssize_t)sizeof(RUDPHeader_St)) continue;
                    RUDPHeader_St* h = (RUDPHeader_St*)buf;

                    /** Si l'action est une requête de découverte, on traite sans session RUDP */
                    if (h->action == ACTION_CODE_LOBBY_ROOM_QUERY) {
                        handle_discovery_query(&c_addr);
                        continue;
                    }

                    /** Si l'action est une demande de join, on traite avec session RUDP */
                    if (h->action == ACTION_CODE_JOIN_GAME) {
                        int id = find_or_create_client(&c_addr);
                        if (id != -1) {
                            // On ne réinitialise PAS rudpInitConnection ici car find_or_create_client s'en charge déjà si nouveau.
                            // Sinon on casserait le séquencement si c'est juste un paquet JOIN retransmis.
                            rudpProcessIncoming(&clients[id].rudp_state, h);
                            gettimeofday(&clients[id].last_seen, NULL);
                            
                            u16 assigned_id = htons((u16)id);
                            server_broadcast(-(id + 1), id, ACTION_CODE_JOIN_ACK, &assigned_id, sizeof(u16));
                            
                            /** Envoyer aux autres clients la position du nouveau client */
                            for (int other = 0; other < MAX_CLIENTS; other++) {
                                if (clients[other].active && other != id) {
                                    // On ne connaît pas leurs pseudos/angles ici car server.c est minimaliste
                                    // Mais on va quand même envoyer un message LOBBY_MOVE factice ou minimal
                                    // pour que le client sache qu'ils existent.
                                    // Note: Une meilleure approche serait que les clients ré-émettent périodiquement.
                                    // Pour l'instant on force un broadcast de tout le monde vers le nouveau.
                                }
                            }
                            
                            /** Appeler la fonction on_action du module actif si elle existe */
                            if (active_module->on_action) {
                                active_module->on_action(active_game_state, id, h->action, buf + sizeof(RUDPHeader_St), (u16)(len - sizeof(RUDPHeader_St)), server_broadcast);
                            }
                        }
                        
                    /** Traiter les actions globales (switch de jeu, etc.) sans session RUDP */
                    } else {
                        int id = find_or_create_client(&c_addr);
                        if (id != -1 && rudpProcessIncoming(&clients[id].rudp_state, h)) {
                            gettimeofday(&clients[id].last_seen, NULL);
                            
                            /** Routage centralisé pour les actions globales */
                            if (h->action == ACTION_CODE_LOBBY_SWITCH_GAME) {
                                u8 target = 0xFF;
                                if (len >= (ssize_t)(sizeof(RUDPHeader_St) + 1)) {
                                    memcpy(&target, buf + sizeof(RUDPHeader_St), 1);
                                }
                                
                                if (target == 0 /* MINI_GAME_LOBBY */ && active_module != &lobby_module) {
                                    active_module->destroy_instance(active_game_state);
                                    active_module = &lobby_module;
                                    active_game_state = active_module->create_instance();
                                    printf("[SERVER] Retour au Lobby\n");
                                }
                                else if (target == MINI_GAME_KFF && active_module != &king_module) {
                                    active_module->destroy_instance(active_game_state);
                                    active_module = &king_module; 
                                    active_game_state = active_module->create_instance();
                                    printf("[SERVER] Passage au module King For Four\n");
                                }
                                else if (target == MINI_GAME_CHESS && active_module != &chess_module) {
                                    active_module->destroy_instance(active_game_state);
                                    active_module = &chess_module; 
                                    active_game_state = active_module->create_instance();
                                    printf("[SERVER] Passage au module Chess\n");
                                }
                                else if (target == MINI_GAME_CUBE && active_module != &rubik_module) {
                                    active_module->destroy_instance(active_game_state);
                                    active_module = &rubik_module; 
                                    active_game_state = active_module->create_instance();
                                    printf("[SERVER] Passage au module Rubik\n");
                                }
                                server_broadcast(0, -1, ACTION_CODE_LOBBY_SWITCH_GAME, &target, 1);
                                
                            /**  */
                            } else if (h->action == ACTION_CODE_LOBBY_CHAT) {
                                server_broadcast(0, id, ACTION_CODE_LOBBY_CHAT, buf + sizeof(RUDPHeader_St), (u16)(len - sizeof(RUDPHeader_St)));
                            } else if (active_module->on_action) {
                                active_module->on_action(active_game_state, id, h->action, buf + sizeof(RUDPHeader_St), (u16)(len - sizeof(RUDPHeader_St)), server_broadcast);
                            }
=======
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(masterSocket, &readfds);

        struct timeval tv = {0, 16666};  // ~60 Hz tick
        int activity = select(masterSocket + 1, &readfds, NULL, NULL, &tv);

        if (activity > 0 && FD_ISSET(masterSocket, &readfds)) {
            struct sockaddr_in clientAddr;
            socklen_t addrLen = sizeof(clientAddr);
            u8 buffer[2048];

            ssize_t received = recvfrom(
                masterSocket, buffer, sizeof(buffer), 0,
                (struct sockaddr*)&clientAddr, &addrLen
            );

            if (received >= (ssize_t) sizeof(RUDPHeader_St)) {
                RUDPHeader_St header;
                memcpy(&header, buffer, sizeof(RUDPHeader_St));

                if (header.action == ACTION_CODE_LOBBY_ROOM_QUERY) {
                    handleDiscoveryQuery(&clientAddr);
                } else {
                    int clientId = findOrCreateClient(&clientAddr);

                    if (clientId == -1 || !rudpProcessIncoming(&clients[clientId].rudpState, &header)) {
                        log_warn("Invalid client id OR invalid package");
                        continue;
                    }

                    gettimeofday(&clients[clientId].lastSeen, NULL);

                    if (header.action == ACTION_CODE_LOBBY_SWITCH_GAME) {
                        MiniGame_Et targetGameId = buffer[sizeof(RUDPHeader_St)];
                        log_info(
                            "[SERVER %s] Client %d requested switch to game ID: %d", 
                            getPreciseTimeString(), clientId, targetGameId
                        );

                        GameServerInterface_St* interface = gameInterfaces[targetGameId];
                        if (interface == NULL) {
                            log_error("Received non-integrated game id");
                            continue;
                        }

                        if (activeModule != interface) {
                            if (activeModule && activeModule->destroy_instance) {
                                    activeModule->destroy_instance(activeGameState);
                                }

                            activeModule = interface;
                            activeGameState = activeModule->create_instance();

                            log_info(
                                "[SERVER %s] Game switch requested for %s",
                                getPreciseTimeString(), interface->game_name
                            );

                            // Confirm switch to requesting client (unicast)
                            u8 switchPayload = targetGameId;
                            serverBroadcast(
                                UNICAST, clientId, ACTION_CODE_LOBBY_SWITCH_GAME, 
                                &switchPayload, 1
                            );
>>>>>>> origin/mgit-PR1-20-03
                        }
                    } else if (header.action == ACTION_CODE_LOBBY_CHAT) {
                        serverBroadcast(0, clientId, ACTION_CODE_LOBBY_CHAT,
                                        buffer + sizeof(RUDPHeader_St),
                                        received - sizeof(RUDPHeader_St));

                    // Sink for the every other mini-game actions
                    } else if (activeModule && activeModule->on_action && activeGameState) {
                        activeModule->on_action(
                            activeGameState,
                            clientId,
                            header.action,
                            buffer + sizeof(RUDPHeader_St),
                            received - sizeof(RUDPHeader_St),
                            serverBroadcast
                        );
                    }
                }
            }
        }
<<<<<<< HEAD
    }
    return 0;
}
=======

        checkTimeouts();

        if (activeModule && activeModule->on_tick) {
            activeModule->on_tick(activeGameState);
        }
    }

    // Cleanup (unreachable in current loop)
    if (activeModule && activeModule->destroy_instance) {
        activeModule->destroy_instance(activeGameState);
    }

    close(masterSocket);
    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"
>>>>>>> origin/mgit-PR1-20-03
