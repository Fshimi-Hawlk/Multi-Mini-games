@page roadmap Project Roadmap

@ref index "Back to Home"

**Last checked against project structure:** March 16, 2026  

This page outlines where we are right now and the rough path we agreed on to keep things manageable.

## Current Status (March 2026)

- `main` -> early skeleton + some docs, but completely outdated now. We basically stopped touching it months ago.
- Active development branches:
  - Individual game branches (e.g. `tetris`, `snake-classic`, `block-blast`, `memory`, etc.) -> where single-player games get built and polished standalone
  - `docs-setup` -> shared documentation, code style rules, setup guides, small refactors that touch multiple parts
  - `sub-games-integration-test` (we call it `sgit`) -> the current main integration branch. We merge finished-enough single-player games here to hook them into the lobby, test them together, fix lobby bugs, add shared features

Right now: several games work standalone, a couple are already playable inside the lobby via `sgit`. The lobby itself has basic movement, collision triggers and UI, but it's still rough around the edges.

## Planned Longer-Term Structure

To avoid big conflicts when we start adding multiplayer (network code, sync problems, different game logic), we agreed on this phased approach:

1. **Single-player phase** (current / next few weeks-months)
   - Finish and polish single-player versions on their own branches
   - Integrate them one by one into the lobby through `sgit`
   - Once we have 4–5+ games working well together -> maybe rename `sgit` to something clearer like `singleplayer-games-integration-test`

2. **Multiplayer phase** (next major step after single-player feels solid)
   - Create a new branch: `multiplayer-games-integration-test` (short: `mgit`)
   - Prototype multiplayer versions there (networking, turn-based or real-time sync, UI differences)
   - Hook MP versions into the lobby separately -> keep single-player code untouched

3. **Combine phase** (when both SP and MP look ready)
   - Create `combine-games-integration-test` (short: `cgit`)
   - Merge single-player and multiplayer branches there
   - Resolve conflicts (local vs networked modes, UI toggles, performance issues, etc.)
   - Test the full lobby with a mix of SP + MP games

4. **Stable release to main**
   - When `cgit` is playable and reasonably stable -> merge everything to `main`
   - Tag a version (e.g. v0.1-singleplayer or v1.0 if ambitious)
   - Maybe create GitHub release with builds, screenshots, short description

This is our plan to keep branches testable and reduce merge hell.  
In practice:
- If something is small and conflict-free, we might skip steps and merge earlier
- If multiplayer takes longer than expected, we can ship a single-player-only version to `main` first
- Everything is flexible — we'll discuss in group chat and adjust whenever needed

## Quick To-Do / Ideas List

- Get at least 4–5 single-player games fully integrated and playable in `sgit`
- Improve lobby: nicer game selection (maybe signs or doors instead of just hitboxes), basic scores display, pause/resume, skin persistence
- Start multiplayer prototyping: pick 1–2 simple games (e.g. turn-based like memory or block-blast) and experiment with networking on a feature branch
- Shared improvements: better audio handling, unified input config, improved error logging/recovery
- Documentation: add screenshots or GIFs of the lobby + running games, finish setup guides

If any of this sounds interesting — feel free to jump in! Open issues, start Draft PRs, ask questions in chat. Mistakes are expected, we're all learning.

**Created:** March 16, 2026  
**Last updated:** March 16, 2026  
**Author:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"