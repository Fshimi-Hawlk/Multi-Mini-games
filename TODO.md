# TODO.md — Multi Mini-Games Monorepo

High-priority / medium-priority tasks for the root project (mostly things that affect everyone or prepare the final game collection).

Items are roughly ordered from "we should do this quite soon" to "nice to have later".

## Right now – most urgent

- [x] Create one global Makefile at root level  
  -> Done. Supports lazy library builds, API header copying, incremental & force-rebuild targets.

- [x] Create global error system (`GameError_Et`)  
  -> Done. Located in `firstparty/APIs/gameError.h`

- [x] Create global configuration system (`GameConfig_St`)  
  -> Done. Located in `firstparty/APIs/gameConfig.h` with audio/video sub-structures and default macros.

- [x] Establish mini-game API pattern  
  -> Done. Standard interface: `*_initGame(config)`, `*_gameLoop(game)`, `*_freeGame(game)`, `*_isRunning(game)`

- [ ] Create root `docs/` folder  
  - Put a main Doxyfile there for lobby + shared code  
  - Later: make one big documentation site linking all games + lobby

## When the first game is merged into main

- [x] ~~Write down clear rules for how a game should be structured so the lobby can use it~~  
  -> Done. See API pattern above. Add new game to `MiniGame_St` array in `lobby/src/main.c`.

- [ ] Decide how to save scores / progress / settings  
  → Simple files in assets/game-name/saves/ ? JSON? Something else?  
  → Create shared code in firstparty/ for loading & saving

- [ ] Think about leaderboards format (local files for now — no online)

- [ ] Add key bindings configuration  
  → Allow players to customize controls per game

## Games Status

| Game | Status | Author | Notes |
|------|--------|--------|-------|
| Solitaire | ✅ Playable | Maxime CHAUVEAU | Complete with drag-drop, scoring, timer |
| Tetris | ❌ Not started | - | API stub exists |

## Later / nice to have

- [ ] Add a "Roadmap" section in README.md with visual progress

- [ ] Create a shared audio system  
  → Background music, sound effects management

- [ ] Add game tutorials / help screens  
  → Accessible from lobby or in-game pause menu

Feel free to add new items or move priorities — discuss big changes in the group chat first.
