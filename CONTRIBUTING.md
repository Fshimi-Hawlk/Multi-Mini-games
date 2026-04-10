# CONTRIBUTING.md

Guidelines for working together on the Multi Mini-Games project

We're all figuring out collaborative Git workflows at the same time - mistakes will happen and that's okay.  
The most important rule is: communicate.

## Basic Workflow (how we work right now)

Basic Workflow (how we work right now - March 2026)

Most of the action is **not** on `main` anymore (it's paused and outdated). We focus on these active branches instead:

- Your individual game branches (e.g. `snake-classic`, `block-blast`, `tetris`, etc.) -> build and polish single-player games here
- `docs-setup` -> shared docs, code style, setup guides, conventions
- `sub-games-integration-test` (we call it `sgit`) -> integrating single-player games into the lobby, fixing shared bugs, improving the lobby itself

Usual flow for most of us:
1. `git fetch origin` and pull the branch you're working on
2. Do your work (on a game branch or directly on `sgit`/`docs-setup` if it's shared stuff)
3. Commit with good messages (see Commit messages section below)
4. Push your branch
5. Open a PR on GitHub (target usually `sgit` for game integrations, or the relevant shared branch)
6. Ping the chat for reviews — we review, test in the lobby, fix things
7. Once a game works well in `sgit`, it's considered "done" for single-player phase

`main` will get everything later when single + multiplayer are both solid and combined.

For the current status, planned branches (like multiplayer integration, final combine step), and why we're doing it this way -> check [ROADMAP.md](./docs/ROADMAP.md)

## Git commands you will use most

```
git pull origin main               # always start
git checkout -b my-game-name       # create your branch
# ... work + commit frequently ...
git push origin my-game-name
# then go to GitHub -> create Pull Request from your branch to main
```

## Commit messages

We try to follow "Conventional Commits" style because it makes the git log and PR reviews way easier to scan.  
The prefix tells everyone at a glance what kind of change it is.

The prefixes listed below (and in the table) are the **most common ones** we expect to see — they're our main set.  
But they are **not strict rules**. If something doesn't fit nicely into one of these, feel free to use (or even invent) another prefix that makes sense for that commit.  
Examples we've already used or might use:

- `fetch` / `merge` — when you bring in commits / files from another branch (cherry-pick, manual merge, etc.)
- `undo` — when you added something in a previous commit and now want to clearly say "I’m removing it again because I changed my mind"
- others that might appear later: `wip`, `temp`, `experiment`, `revert`, etc.

So: use the common ones when they fit, but don't force it if it feels wrong — clarity > perfect prefix.

Also — something I've started doing in recent commits and I think is nice:  
put a **very short one-line summary** at the very top of the commit message (before the list of prefixed lines).  
It helps a lot when scrolling through `git log --oneline` or GitHub commit lists.

Good example (recent one):

```text
Docs update/less globals/temporary pause menu removal/fixes/new font
- docs: Updated the creation date and last updated dates
- chore: Removed unused windowRect and appFont off globals
- refactor: Moved games globals to BallSystem_St fields
- undo: Removed pause menu draft. I decided to move it's implementation when the lobby will be back, as it would make more sense with the lobby.
- fix: Font warning font size wasn't correctly displaying
- fix: Can now place daub right when the ball's text appear without penalizing te player.
- chore: Changed font to NotoSansMono-Bold.ttf
```

Compared to older style (still okay, but harder to grasp quickly):

```text
- refactor: Moved all the lobby initialization, gameloop and freeing logic into dedicated lobbyAPI .c and .h file to factorize the code and comply with every other game on the structure.
- fix: Corrected the variable name of GameScene_Et
- refactor: Added PlayerTexture_Et to correctly name what texture id correspond to which texture
...
```

The top summary line is optional but recommended — especially when a commit touches many things.

Most important common prefixes for us right now:

Quick reference table:

| Type       | Purpose / When to use it                                                                 | Changes the produced game binary? | Semantic Versioning impact | Example commit message (our project context)                     |
|------------|------------------------------------------------------------------------------------------|-----------------------------------|----------------------------|-------------------------------------------------------------------|
| `feat`     | Introduces new functionality, like a new game mechanic or UI element.                   | Yes                               | Minor                      | feat: Added hold piece mechanic in Tetris                          |
| `fix`      | Corrects a defect in existing functionality, such as a crash or incorrect scoring.     | Yes                               | Patch                      | fix: Resolved collision detection issue in lobby platformer        |
| `docs`     | Changes that affect only documentation, comments, READMEs, or similar.                 | No                                | None                       | docs: Updated API conversion guide with new error handling section  |
| `style`    | Cosmetic changes: formatting, indentation, renaming without logic impact.              | No                                | None                       | style: Reformatted lobbyAPI.c to use consistent camelCase          |
| `refactor` | Internal code cleanup / restructuring that improves code but doesn't change behavior. Usually no binary change, but can happen if lots of moving around causes different compilation order or inlines. | Usually No                        | None                       | refactor: Extracted common player update logic to shared utils.h   |
| `test`     | Adding, updating, or fixing tests.                                                     | No                                | None                       | test: Added basic score overflow check for snake-classic            |
| `chore`    | Housekeeping: .gitignore, unused files, version bumps, small tooling tweaks, cleanups   | Almost never                      | None                       | chore: Ignored compile_commands.json and .cache/ in .gitignore     |
| `build`    | Makefile changes, new compiler flags, debug/release modes, linker options, etc.         | No (build only)                   | None                       | build: Added -O2 optimization to release build in Makefile          |

For very tiny changes (one typo fix, trailing whitespace) you can:

- make a small `style:` / `chore:` commit
- or just fold it into your next "real" commit (totally fine — we do this a lot)

We're not super strict — the goal is readable history + helping each other during reviews.  
If you're unsure about a prefix, pick the closest one or ask in chat. We can always adjust the message during PR review if needed.

## Pull Requests (PRs)

Pull Requests (PRs) are the standard way we review, discuss, and safely integrate changes.  
We distinguish two main cases depending on the **target branch**:

1. PRs targeting a **feature/game branch** (e.g. `tetris`, `block-blast`, `snake`)
2. PRs targeting a **shared/global branch** (e.g. `main`, `docs-setup`, `sub-games-integration-test`)

### Branch Naming Convention (both cases)

All branches follow this pattern:

`<target-branch-name>-PR<sequential-number-of-the-day>-DD/MM`

Examples:
- `tetris-PR1-01/03`     -> first PR of the day targeting `tetris` on March 1st
- `tetris-PR2-01/03`     -> second PR of the day to `tetris`
- `main-PR3-02/03`       -> third PR of the day targeting `main` on March 2nd
- `docs-setup-PR1-05/03` -> first PR of the day to `docs-setup`

This makes it easy to see at a glance what the branch is for, its order in the day, and the date.

### Case 1: PR targeting a Feature/Game Branch (e.g. `tetris`, `block-blast`)

These are PRs where someone is helping improve or fix an existing feature branch.

**Steps to create the branch and open the PR**

Example for `tetris`:

```bash
# Fetch latest state
git fetch origin

# Make sure you are up-to-date on the target branch
git checkout tetris
git pull origin tetris

# Create your new branch using the naming convention
git checkout -b tetris-PR1-01/03

# Do your work, commit with conventional commit messages
# ...

# Push
git push origin tetris-PR1-01/03
```

**Opening and reviewing the PR**
- Go to GitHub -> "Pull requests" -> "New pull request"
- Base: `tetris`   Compare: `tetris-PR1-01/03`
- Title: short sentence (e.g. "Improve UI")
- Description: explain what was changed and why, link any related issues/docs
- In the sidebar: add the branch "owner" (the person mainly responsible for `tetris`) as reviewer
- Ping group chat: "Opened PR #X to tetris – please review"

**Conditions for merging**
- Requires **at the very least the approval from the branch owner**
- By team convention: **only the branch owner** (or the person they delegate to) should click "Merge pull request"
- Use **"Create a merge commit"** (default) to preserve full authorship and granular history

**After merge**
- Click the green **"Delete branch"** button on the PR page
- The branch disappears from the Branches list, but all commits remain fully visible and traceable in `tetris` via the merge commit
- The branch owner pulls the updated branch: `git checkout tetris && git pull origin tetris`

### Case 2: PR targeting a Shared/Global Branch (e.g. `main`, `docs-setup`, `sub-games-integration-test`)

These are PRs that bring completed work (new game, big refactor, doc update, etc.) into a shared branch everyone relies on.

**Steps to create the branch and open the PR**

```bash
# Always start from latest docs-setup (or target shared branch)
git fetch origin
git checkout docs-setup
git pull origin docs-setup

# Create your branch using the naming convention
git checkout -b docs-setup-PR1-01/03

# Do your work, commit...
# ...

# Push
git push origin docs-setup-PR1-01/03
```

**Opening and reviewing the PR**
- GitHub -> "New pull request"
- Base: `docs-setup`   Compare: `docs-setup-PR1-01/03`
- Title: clear and concise (e.g. "Rework web site documentation")
- Description: explain purpose, key changes, link to relevant docs, note any trade-offs
- In sidebar: add **all other team members** as reviewers
- Ping group chat: "PR #X ready for review"

**Conditions for merging**
- Requires **approval from all other team members** (3 approvals for our team of 4)
- Stale approvals are automatically dismissed on new commits (safety feature enabled on protected branches)
- **Only the PR author** merges their own PR once all approvals are in – do not merge someone else's PR unless explicitly asked
- Use **"Create a merge commit"** (default button) – preserves full commit history and authorship, which is important for tracing work and debugging
- Do **not** use "Squash and merge" or "Rebase and merge" unless the team explicitly agrees for that specific PR

**After merge**
- Click **"Delete branch"** on the PR page to keep the Branches tab clean
- Commit history from your branch remains fully accessible in `docs-setup` via the merge commit (even after branch deletion)
- Pull latest: `git checkout docs-setup && git pull origin docs-setup`
- If post-merge issues appear, open a new PR with fixes

### General PR Tips (both cases)
- Open early as Draft PR if you want early feedback (mark as Draft in GitHub)
- Use `@username` in comments to notify specific people
- Resolve merge conflicts yourself (usually `git pull --rebase origin <target>`)
- Run `make MODE=clang-debug run-tests` before opening and before final merge
- If someone is unavailable >3 days, team can agree to lower required approvals temporarily (note it in PR comments)

This process keeps ownership clear, preserves authorship, maintains clean UI, and ensures everyone has visibility without creating bottlenecks.

## Code Style

So, to start things up, see the linked [CodeStyleAndConventions.md](./CodeStyleAndConventions.md)

-> It's mostly how I've been writing so far.  
-> Goal => have some baseline consistency, I'm not expecting perfect uniformity.  
-> If anything feels inconvenient, overly strict/pedantic, or just wrong - bring it up (chat or PR that edits the file). We can adopt parts, ignore parts, or write something new together.  

Even partial adherence helps - big style differences make everyone's life harder when reading/reviewing.

## Reviews & questions

- Ping the group chat when you open a PR
- It's normal to ask "is this okay?" or "what do you think about this approach?"
- Merge conflicts happen - ask for help when they do, there's no shame

Thanks for working together - take your time, ask questions, and enjoy making the games!

## Credits

**Created: January 15, 2026**  
**Last updated: March 17, 2026**  
**Authors:**
- [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)