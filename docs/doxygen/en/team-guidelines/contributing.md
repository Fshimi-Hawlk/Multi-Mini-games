@page contributing Contributing Guidelines

@ref index "Back to Home"

**Last checked against project structure:** March 16, 2026  

We're a small team of four students learning Git collaboration as we go.  
This page describes our current workflow for commits, branches, pull requests and reviews (as of March 2026).  
Even though the project is still WIP, we already have defined basic rules to keep things organized and traceable.

The most important principle stays the same: communicate openly in the group chat when something is unclear.

## Current Active Branches & Workflow Overview

`main` is currently paused and mostly outdated.  
Active development happens on these branches:

- Individual game branches (e.g. `tetris`, `snake-classic`, `block-blast`, etc.) -> single-player game development and polish
- `docs-setup` -> documentation, code style, conventions, setup guides
- `sub-games-integration-test` (we usually call it `sgit`) -> lobby improvements, game integration, shared bug fixes

Typical flow for a team member:
1. Pull latest from the branch you're working on
2. Work on your feature / game / fix
3. Commit with clear messages (see below)
4. Push your branch
5. Open a Pull Request (usually targeting `sgit` for integrations, or the relevant shared branch)
6. Ask for reviews in group chat
7. Fix feedback -> once approved and tested in lobby -> merge

Once games are solid in single-player on `sgit`, we'll later merge everything into `main` (including future multiplayer parts).

For planned steps, current status and long-term vision see @ref roadmap "Roadmap"

## Commit Messages

We follow a simplified **Conventional Commits** style.  
It helps everyone understand changes quickly during reviews and in git log.

Common prefixes we use most often:

- `feat:`     new feature / visible change
- `fix:`      bug fix
- `docs:`     documentation, comments, guides
- `style:`    formatting, whitespace, naming (no logic change)
- `refactor:` code cleanup / better structure (no behavior change)
- `test:`     tests added or fixed
- `chore:`    maintenance, tooling, .gitignore, small cleanups
- `build:`    Makefile, build flags, compiler options

We also sometimes use:
- `fetch` / `merge` -> bringing in changes from another branch
- `undo` -> clearly removing something added earlier

Examples from our recent commits:
- `feat: added hold piece mechanic in Tetris`
- `fix: resolved player getting stuck on lobby platforms`
- `docs: updated API conversion guide with new error codes`
- `refactor: moved player globals into BallSystem_St`
- `undo: removed temporary pause menu draft`

For very small changes (typo, trailing space) we either make a tiny `style:`/`chore:` commit or fold it into the next real commit — both are fine.

Optional but recommended: add a **one-line summary** at the very top of longer commit messages.  
It makes `git log --oneline` and GitHub commit lists much easier to scan.

Example of good recent commit message:

```text
Docs update / less globals / temporary pause menu removal / fixes / new font
- docs: Updated creation date and last updated dates
- chore: Removed unused windowRect and appFont from globals
- refactor: Moved game globals to BallSystem_St fields
- undo: Removed pause menu draft - will reimplement later in lobby context
- fix: Fixed font size warning display
- fix: Can now place daub immediately when ball text appears
- chore: Switched font to NotoSansMono-Bold.ttf
```

We're not strict about prefixes — clarity and helpful history matter more than perfect categorization.  
If unsure, pick the closest one or ask in chat.

## Branch Naming for Pull Requests

We use this pattern for PR branches:

`<target-branch>-PR<day-sequence-number>-DD/MM`

Examples:
- `tetris-PR1-16/03`
- `sgit-PR3-16/03`
- `docs-setup-PR2-17/03`

This makes it obvious what the branch targets, when it was created, and its order that day.

## Pull Requests

PRs are our main way to review, discuss and integrate changes safely.

### Before opening a PR
- Run tests: `make MODE=clang-debug run-tests` (or at least `make tests`)
- Make sure it builds without warnings/errors

### Creating the PR branch (two common cases)

**Case 1: Helping on a game/feature branch** (e.g. improving `tetris`)

```bash
git fetch origin
git checkout tetris
git pull origin tetris
git checkout -b tetris-PR1-16/03
# work -> commit -> ...
git push origin tetris-PR1-16/03
```

**Case 2: Integrating into shared branch** (e.g. `sgit`, `docs-setup`)

```bash
git fetch origin
git checkout sub-games-integration-test
git pull origin sub-games-integration-test
git checkout -b sgit-PR1-16/03
# work -> commit -> ...
git push origin sgit-PR1-16/03
```

### Opening & reviewing
- Title: short and clear (e.g. "Fix collision in lobby platforms")
- Description: explain what + why, link related docs/issues if any
- Add reviewers:
  - Game branches -> mainly the branch owner
  - Shared branches -> all other team members
- Ping group chat: "opened PR #X for review"

We often open as Draft PR early to get feedback before it's finished.

### Merging rules (team convention)
- Game/feature branches -> needs at least approval from the branch owner
  -> only the branch owner (or delegate) should merge
- Shared branches (`sgit`, `docs-setup`, `main`, etc.) -> needs approval from all other team members
  -> only the PR author merges once approvals are in (unless asked otherwise)
- Use **"Create a merge commit"** (default) -> keeps full authorship and granular history
- Do not squash or rebase unless the team explicitly agrees for that PR

### After merge
- Delete the PR branch from GitHub (keeps Branches tab clean)
- Pull latest on target branch
- History remains fully traceable via merge commit

## Code Style

See @ref code_style_and_conventions "Code Style & Conventions" for naming, formatting, file layout, etc.

It's mostly based on how the project started — goal is baseline consistency, not perfect uniformity.  
If anything feels inconvenient or wrong, bring it up in chat or open a PR to improve the style guide.

## Reviews & Questions

- Ping the group chat when opening a PR or when you need input
- Asking "does this look okay?" or "what do you think about this approach?" is completely normal
- Merge conflicts happen — just ask for help, no problem at all

Thanks for working together — take your time, ask questions, enjoy the process!

**Created:** January 15, 2026  
**Last updated:** March 16, 2026  
**Author:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"