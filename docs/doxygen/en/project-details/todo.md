@page todo TODO

@ref index "Back to Home"

**Last checked against project structure:** March 16, 2026

This page lists the main open tasks for the whole monorepo.  
Most of these are things that impact the lobby, shared code, or the overall collection of mini-games.  

Tasks are sorted roughly from "do this soon" to "would be cool later".  
We update this file as we go - feel free to add items or change priorities after group discussion.

## High priority - do these quite soon

- [x] Create one global Makefile at root level  
  -> Done. It now handles lazy library builds, copies API headers to firstparty/APIs/, supports incremental builds, force-rebuilds, debug modes, sanitizers, etc.

- [ ] Finish setting up root `docs/` folder properly  
  -> Already created the folder and moved several guides there (`API_Conversion.md`, `CodeStyleAndConventions.md`, `makefile.md`, etc.) to keep root clean.  
  - Main Doxyfile is in `docs/` and documents lobby + shared code  
  - Next: expand to include documentation for each game module once more are integrated

## After the first game is fully merged into main

- [x] Write clear rules for turning a standalone game into a lobby-compatible API  
  -> Done. See @ref api_conversion "API Conversion & Integration"

- [ ] Decide on save / progress / settings system  
  -> Options so far: plain text files in `assets/<game-name>/saves/`, simple JSON, or something custom?  
  -> Plan: add shared loading/saving helpers in `firstparty/` so every game can use the same code

- [ ] Define local leaderboard format  
  -> Keep it simple for now (local files only, no online).  
  -> Maybe JSON or binary in `assets/leaderboards/`?

## Nice to have / longer term

- [x] Add a visible roadmap  
  -> Done. See the [Gantt chart on GitHub Projects](https://github.com/users/Fshimi-Hawlk/projects/1)  
  -> Additionally, refer to @ref roadmap "Roadmap"  

- [ ] Add basic audio settings (volume sliders, mute toggles) in lobby  
- [ ] Add persistent skin unlocks / cosmetics across games  
- [ ] Experiment with very basic local multiplayer (split-screen or hotseat)  
- [ ] Add simple error recovery in lobby (reload game on crash instead of exit)

Big changes should be discussed in the group chat first so everyone agrees.

**Created:** March 02, 2025  
**Last updated:** March 16, 2026  
**Author:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"