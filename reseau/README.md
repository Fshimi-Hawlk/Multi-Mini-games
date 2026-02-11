# Module Reseau

**Core TCP Server, TLV structure Protocol, and Client API for Multi-Mini-Games.**

## Overview

The `reseau` module is the backbone of the multiplayer architecture. It provides:
1.  **The Central Server:** A standalone TCP server that manages lobby state, rooms, and player connections.
2.  **The Client API:** A static library (`libreseau.a`) used by mini-games to communicate easily with the server.
3.  **The Protocol:** A strict binary definition for data exchange.

### Core Mechanics

The module does not have "gameplay" in itself but orchestrates the game flow:
- **Handshake & Auth:** Handles client connection and identification.
- **Lobby Management:** creation/joining of rooms, chat dispatching.
- **Packet Routing:** Acts as a relay to tunnel game-specific data between clients sharing a room.

[See Protocol Definition](./include/protocol.h)

## Project Goals

- **Reliability:** Ensure stable TCP connections handling disconnects gracefully.
- **Ease of Use:** Provide a simple C API (`Reseau_Connect`, `Reseau_Send`) so game developers don't deal with raw sockets.
- **Scalability:** Capable of handling multiple rooms and games simultaneously via `epoll` or `select`.

## Tech / Structure Notes
- Language: C
- Folders:
  - `src/`: Server logic (`server.c`) and Lobby logic (`lobby.c`).
  - `include/`: Public API (`reseauAPI.h`) and Protocol (`protocol.h`).
  - `build/`: Output binaries and libraries.
- Documentation: Generated via Doxygen → see [`docs/html/index.html`](./docs/html/index.html) (after running `doxygen`)