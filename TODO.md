# TODO.md — Multi Mini-Games Monorepo

High-priority / medium-priority tasks for the root project (mostly things that affect everyone or prepare the final game collection).

Items are roughly ordered from "we should do this quite soon" to "nice to have later".

## Right now – most urgent

- [x] Create one global Makefile at root level  
  -> Done. Supports lazy library builds, API header copying, incremental & force-rebuild targets.

- [ ] Create root `docs/` folder  
  - Put a main Doxyfile there for lobby + shared code  
  - Later: make one big documentation site linking all games + lobby

## When the first game is merged into main

- [ ] Write down clear rules for how a game should be structured so the lobby can use it  
  Example: game must provide init/update/draw/cleanup functions with a common prefix

- [ ] Decide how to save scores / progress / settings  
  → Simple files in assets/game-name/saves/ ? JSON? Something else?  
  → Create shared code in firstparty/ for loading & saving

- [ ] Think about leaderboards format (local files for now — no online)

## Later / nice to have

- [ ] Add a "Roadmap" or "Games status" section in README.md  
  Example table: which games are started / in progress / finished / merged

Feel free to add new items or move priorities — discuss big changes in the group chat first.