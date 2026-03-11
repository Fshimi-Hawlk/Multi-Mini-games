## Architecture & Concept

Le serveur fonctionne sur un modèle **serveur autoritaire**. Cela signifie que :
1. Les clients envoient des **intentions d'actions**.
2. Le serveur valide la logique métier (ex: "Le joueur a-t-il le droit de poser cette carte ?").
3. Le serveur diffuse l'état résultant à tous les participants.



### Points Clés :
* **Modulaire** : Via `GameInterface`, le serveur peut charger `king-for-four`, un lobby, ou tout autre jeu sans modification du code réseau.
* **Tickrate** : Stabilisé à **60 FPS** (cycle de **16.6ms**) via `select()`.
* **Multi-joueurs** : Gestion native de 8 slots clients avec détection de timeout automatique (5s).

---

## 🛰️ Protocole RUDP (Reliable UDP)

Pour pallier le manque de fiabilité de l'UDP standard tout en évitant la latence du TCP, nous utilisons un en-tête personnalisé compact.

### Structure de l'En-tête (11 octets)
L'alignement mémoire est forcé à 1 octet via `#pragma pack(push, 1)` pour garantir la compatibilité entre architectures.

| Offset | Type | Champ | Description |
| :--- | :--- | :--- | :--- |
| **0** | `uint16_t` | `sequence` | ID unique du paquet (incrémental). |
| **2** | `uint16_t` | `ack` | ID du dernier paquet reçu avec succès. |
| **4** | `uint32_t` | `ack_bitfield` | Masque de bits représentant l'historique des 32 derniers ACKs. |
| **8** | `uint16_t` | `sender_id` | Identifiant du client (assigné par le serveur). |
| **10** | `uint8_t` | **`action`** | Code de l'action métier (Join, Move, Query...). |



---

## 🛠️ Compilation & Utilisation

Le module réseau doit être lié aux bibliothèques statiques des jeux présentes dans `build/lib/`.

### Commandes principales (depuis la racine) :
```bash
# Compiler le serveur et ses modules
make server

# Lancer le serveur
make run-server