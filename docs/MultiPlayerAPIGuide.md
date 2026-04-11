# 🛰️ Guide Expert de l'API Multi-Joueurs (RUDP + TLV)

Ce document détaille l'architecture réseau et les interfaces nécessaires pour intégrer un mini-jeu dans l'écosystème **Multi-Mini-Games**.

---

## 1. Architecture Réseau

Le projet utilise un protocole **RUDP (Reliable UDP)** maison au-dessus de l'UDP standard. La communication suit un modèle **Autoritaire** : le serveur gère la logique réelle et les clients ne sont que des terminaux d'affichage synchronisés.

### 📦 Structure d'un Paquet (Empilement)

Chaque message réseau envoyé suit cette structure binaire stricte :

1.  **En-tête RUDP (11 octets)** : Gère la fiabilité et le séquencement.
2.  **En-tête TLV (4 octets)** : Identifie le jeu cible et le type d'action.
3.  **Charge Utile (Payload)** : Données spécifiques au jeu (ex: une structure `GameSyncPayload`).

#### Détail de l'En-tête RUDP (`RUDPHeader_St`)
| Taille | Type | Champ | Description |
| :--- | :--- | :--- | :--- |
| 2 | `u16` | `sequence` | Numéro de séquence du paquet actuel. |
| 2 | `u16` | `ack` | Dernier numéro de séquence reçu en ordre. |
| 4 | `u32` | `ack_bitfield` | Historique des 32 derniers paquets reçus (1 = reçu). |
| 2 | `u16` | `sender_id` | ID réseau du joueur (rempli par le serveur). |
| 1 | `u8` | `action` | Code d'action global (ex: `0x06` pour `ACTION_CODE_GAME_DATA`). |

#### Détail de l'En-tête TLV (`GameTLVHeader_St`)
| Taille | Type | Champ | Description |
| :--- | :--- | :--- | :--- |
| 1 | `u8` | `game_id` | ID unique du mini-jeu (0 = Lobby). |
| 1 | `u8` | `action` | Action spécifique au mini-jeu (ex: `PLAY_CARD`). |
| 2 | `u16` | `length` | Taille des données qui suivent (Payload). |

---

## 2. Interface Serveur (`GameServerInterface_St`)

Définie dans `networkInterface.h`, cette interface permet au serveur de piloter votre jeu sans connaître sa logique interne.

### 🛠️ Fonctions à implémenter :

*   **`create_instance()`** : Initialise l'état mémoire du jeu. Retourne un `void*` que le serveur passera aux autres fonctions.
*   **`on_action(state, player_id, action, payload, len, broadcast)`** :
    *   Appelée quand un client envoie une action.
    *   `action` est le code extrait du TLV.
    *   `broadcast` est le pointeur de fonction pour répondre.
*   **`on_tick(state)`** : Appelée 60 fois par seconde. Idéal pour la logique des bots ou les timers.
*   **`on_player_leave(state, player_id)`** : Gère la déconnexion brutale d'un joueur.
*   **`destroy_instance(state)`** : Libère toute la mémoire allouée.

### 💡 Le mécanisme de Diffusion (`Broadcast`)
La fonction `broadcast` fournie par le serveur est puissante :
*   `room_id = 0, exclude_id = -1` : Envoie à **tout le monde** dans la salle.
*   `room_id = 0, exclude_id = X` : Envoie à tout le monde **sauf** au joueur X.
*   `room_id = -1, exclude_id = X` : Envoie **uniquement** au joueur X (**Unicast**).

---

## 3. Interface Client (`GameClientInterface_St`)

Le client gère l'affichage (`Raylib`) et la capture des entrées.

### ⚠️ Règle d'or du Rendu
Vos fonctions `update()` et `draw()` **NE DOIVENT PAS** appeler `BeginDrawing()`, `EndDrawing()` ou `ClearBackground()`. Ces appels sont centralisés dans le `main.c` du lobby pour éviter les scintillements et les chutes de performance.

### 🛠️ Fonctions à implémenter :

*   **`init()`** : Charge les textures, sons et polices.
*   **`on_data(player_id, action, data, len)`** : Reçoit les mises à jour du serveur (souvent un `GameSyncPayload`).
*   **`update(dt)`** : Traite les clics souris/touches et envoie les ordres au serveur via le socket global `networkSocket`.
*   **`draw()`** : Dessine l'état actuel du jeu.

### 📤 Exemple : Envoi d'une action client
```c
// Préparation du message
ActionPlayPayload_St play = { .card_index = 5 };
GameTLVHeader_St tlv = { .game_id = 1, .action = KFF_PLAY_CARD, .length = sizeof(play) };
RUDPHeader_St rudp;
rudpGenerateHeader(&serverConnection, ACTION_CODE_GAME_DATA, &rudp);

// Envoi (Formatage binaire)
u8 buffer[1024];
memcpy(buffer, &rudp, sizeof(rudp));
memcpy(buffer + sizeof(rudp), &tlv, sizeof(tlv));
memcpy(buffer + sizeof(rudp) + sizeof(tlv), &play, sizeof(play));

send(networkSocket, buffer, sizeof(rudp) + sizeof(tlv) + sizeof(play), 0);
```

---

## 4. Intégration dans le Lobby

1.  **Enregistrement** : Déclarez votre module dans `lobby/src/main.c` :
    ```c
    extern GameClientInterface_St MonSuperJeuModule;
    register_minigame(&MonSuperJeuModule);
    ```
2.  **Activation** : Le switch vers votre jeu est déclenché par une action `ACTION_CODE_LOBBY_SWITCH_GAME` envoyée par le client déclencheur au serveur.

---

## 5. Codes d'Action Standard (`BaseActionCodes_e`)

Utilisez ces enums de `networkInterface.h` pour éviter les collisions :
*   `ACTION_CODE_JOIN_GAME` (0x02)
*   `ACTION_CODE_SYNC_GAME` (0x04) : Synchronisation complète périodique.
*   `ACTION_CODE_QUIT_GAME` (0x05) : Sortie propre.
*   `ACTION_CODE_GAME_DATA` (0x06) : **Obligatoire** pour tout le trafic interne aux mini-jeux.

---
*Dernière mise à jour : 31 Mars 2026 - Révision Correction Performance & RUDP.*
