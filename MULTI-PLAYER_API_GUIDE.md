# Guide d'Utilisation de l'API Multi-Joueurs (Multi-Mini-Games)

Ce document explique comment transformer un mini-jeu Raylib solo en un module multi-joueurs intégré au Lobby.

## 1. Architecture Globale

Le système repose sur deux interfaces principales :
- **Serveur (`GameInterface`)** : Gère la logique faisant autorité.
- **Client (`MiniGameModule`)** : Gère le rendu et les entrées utilisateur.

Le protocole utilise des paquets **RUDP** (Reliable UDP) contenant un en-tête **TLV** (Type-Length-Value).

## 2. Implémentation Côté Serveur

Le serveur doit implémenter l'interface `GameInterface` définie dans `firstparty/game_interface.h`.

### Fonctions Clés :
- `create_instance()` : Alloue l'état de la partie (`GameState`).
- `on_action(state, player_id, action, payload, len, broadcast)` : 
    - Reçoit les actions des clients.
    - `player_id` permet d'identifier quel joueur a envoyé l'action.
    - Utilise `broadcast` pour envoyer l'état aux clients.

### Astuce : Unicast vs Broadcast
Pour envoyer des données privées (ex: une main de cartes), nous avons étendu `server_broadcast` :
- `room_id = 0` : Envoie à tous sauf `exclude_id`.
- `room_id = -1` : Envoie **uniquement** à `exclude_id` (mode Unicast).

```c
// Exemple : Synchroniser la main d'un joueur spécifique
broadcast(-1, target_player_id, ACTION_SYNC_HAND, cards, len);
```

## 3. Implémentation Côté Client

Le client doit implémenter `MiniGameModule` défini dans `firstparty/APIs/module_interface.h`.

### Fonctions Clés :
- `init()` : Charge les textures/sons.
- `on_data(player_id, action, data, len)` : Reçoit les mises à jour du serveur et met à jour un `GameState` local (fictif).
- `update(dt)` : Détecte les entrées (clics) et envoie des actions au serveur via `send_to_server`.
- `draw()` : Affiche le `GameState` local.

### Envoi d'une action au serveur :
```c
GameTLVHeader tlv = { .game_id = MY_GAME_ID, .action = MY_ACTION, .length = payload_len };
// ... Génération header RUDP ...
// ... Envoi via socket ...
```

## 4. Intégration au Lobby

1. **Enregistrement** : Dans `lobby/src/main.c`, ajoutez votre module :
   ```c
   extern MiniGameModule MonSuperJeuModule;
   register_minigame(&MonSuperJeuModule);
   ```

2. **Bascule (Switch)** : Pour lancer le jeu, envoyez l'action `LOBBY_SWITCH_GAME` (0x20) au serveur avec l'ID de votre jeu.

## 5. Résumé du Protocole

| Couche | Structure | Rôle |
| :--- | :--- | :--- |
| **Réseau** | `RUDP_Header` | Séquencement, ACKs, ID Expéditeur |
| **Routage** | `GameTLVHeader` | ID du Jeu, Type d'Action, Taille |
| **Métier** | `Payload` | Données spécifiques (ex: index de carte) |

---
*Document généré automatiquement suite à l'intégration de King-for-Four.*
