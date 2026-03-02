@page todo Project TODO

# TODO

High-priority / medium-priority tasks for the root project (mostly things that affect everyone or prepare the final game collection).

Items are roughly ordered from "we should do this quite soon" to "nice to have later".

@section urgent Right now – most urgent

- [x] Create one global Makefile at root level  
  Done. Supports lazy library builds, API header copying, incremental & force-rebuild targets.

- [X] Create root @c docs/ folder  
  Added @c docs folder to put in @c API_Conversion.md, @c CodeStyleAndConventions.md and @c makefile.md (root-level) in order to de-clutter the project folder. 
  - Put a main Doxyfile there for lobby + shared code  
  - Later: make one big documentation site linking all games + lobby

@section first_game When the first game is merged into main

- [X] Write down clear rules for how a game should be structured so the lobby can use it  
  See @ref api_conversion

- [ ] Decide how to save scores / progress / settings  
  Simple files in assets/game-name/saves/ ? JSON? Something else?  
  Create shared code in firstparty/ for loading & saving

- [ ] Think about leaderboards format (local files for now - no online)

@section later Later / nice to have

- [X] Add a "Roadmap"  
  See [Gantt chart](https://github.com/users/Fshimi-Hawlk/projects/1)

Feel free to add new items or move priorities - discuss big changes in the group chat first.

**Last updated:** March 02, 2026  
**Author:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)