# Multi-Mini-Games Lobby

## Overview
The Lobby serves as the central hub for the Multi-Mini-Games collection. It is a 2D environment where players can move around, interact with each other, and enter different zones to launch various mini-games.

## Features
- **Central Hub**: Explore a persistent world and find entry points to all available mini-games.
- **Multiplayer Connectivity**: Connect to local or remote servers using a custom RUDP (Reliable UDP) implementation.
- **Server Discovery**: Automatically find active game servers on your local network.
- **Skin Customization**: Change your player's appearance through an interactive skin menu (press 'P').
- **Interactive Zones**: Walk into designated areas to trigger game selection and matchmaking.
- **Integrated Level Editor**: Create and modify lobby levels in real-time.
- **Progress Tracking**: Your achievements and unlocked skins are saved and persisted across sessions.

## Controls
- **A / D**: Move Left / Right
- **Space**: Jump (supports double jump and coyote time)
- **P**: Open Skin Selection Menu
- **Enter**: Enter a mini-game zone or interact with objects
- **Esc**: Return to Lobby (from a mini-game) or open the main menu

## Technical Architecture
- **Scene Management**: A generic dispatch system allows for easy registration of new mini-games.
- **Networking**: Built on top of a custom RUDP core for low-latency, reliable communication.
- **Rendering**: Powered by Raylib for efficient 2D/3D graphics and UI.

## Credits
**Last updated: April 14, 2026**  
**Author: Léandre BAUDET**
