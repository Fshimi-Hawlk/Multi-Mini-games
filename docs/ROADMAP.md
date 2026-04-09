# ROADMAP.md

Current project status and planned path (as of March 16, 2026)

We're a team of 4 students building a collection of mini-games in one repo, with a lobby to play them all. Still very much in progress — we're figuring out structure, multiplayer, integration as we go.

## Current Reality (March 2026)

- `main`: Has early structure + docs, but basically abandoned since the start of the project. Completly outdated compared to active branches.
- Active work happens here:
  - Individual game branches: Most single-player games get built/polished here (snake, tetris, block-blast, memory, etc.)
  - `docs-setup`: Shared docs, code conventions, setup guides, small refactors that affect everything
  - `sub-games-integration-test` (`sgit`): The main integration branch right now — we PR finished-ish single-player games here to connect them to the lobby, test together, fix lobby bugs, add shared features

So far: a few games are playable standalone, some are getting hooked into the lobby via `sgit`. Lobby basics exist but still rough.

## Planned Longer-Term Structure (our team agreement)

To avoid chaos when adding multiplayer (network code, sync issues, different UI/logic), we want to split integration phases:

1. **Single-player phase** (now / soon)
   - Finish/polish single-player games on their branches
   - Integrate them one by one into lobby via `sgit`
   - Rename `sgit` -> `singleplayer-games-integration-test` when it feels right (maybe after 4-5 games are in)

2. **Multiplayer phase** (next big step)
   - Create a new branch: `multiplayer-games-integration-test` (shorten to `mgit`)
   - Implement multiplayer versions there (networking, sync, turn-based/real-time logic)
   - Integrate MP games to lobby separately — no stepping on single-player code

3. **Final combine phase** (when both feel ready)
   - Create `combine-games-integration-test` (`cgit`)
   - Merge single-player + multiplayer branches there
   - Fix any conflicts (local vs networked modes, UI switches, performance, etc.)
   - Test the full lobby with mix of SP + MP games

4. **Stable release to main**
   - When `cgit` is playable and stable -> merge everything to `main`
   - Tag a version, maybe make a release with builds/screenshots

This is the theory to keep things testable and conflict-free. In practice:
- If conflicts are tiny, we might skip some steps and merge earlier
- If multiplayer takes forever, we could ship single-player only to `main` first
- We'll talk in chat and adjust as needed — nothing is set in stone

## Quick Status To-Do / Ideas

- More games: get at least 4-5 single-player ones fully in `sgit`
- Lobby polish: better menu, game selection, scores, pause/resume basics
- Multiplayer start: pick 1-2 games to prototype networking on a feature branch first
- Shared utils: audio manager, input handling, better error reporting
- Docs: finish setup guide, add screenshots/GIFs of lobby + games

If you want to help with any of this — jump in! Open issues, Draft PRs, ask in chat. Mistakes are fine, we're learning too.

## Credits

**Created: March 16, 2026**  
**Last updated: March 17, 2026**  
**Authors:**
- [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)
- Maxime CHAUVEAU
