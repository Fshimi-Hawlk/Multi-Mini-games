# CONTRIBUTING.md

Guidelines for working together on the Multi Mini-Games project

We're all figuring out collaborative Git workflows at the same time - mistakes will happen and that's okay.  
The most important rule is: communicate.

## Basic Workflow (very simplified)

1. Everything stable lives on branch `main`
2. Each mini-game gets its own branch (example: `block-blast`, `snake-classic`, `memory-game`)
3. You work only on your branch
4. When your game is playable and reasonably clean -> you create a Pull Request (PR) to merge it into `main`
5. After review + any small fixes -> merge -> the game becomes part of the main project

## Git commands you will use most

```
git pull origin main               # always start
git checkout -b my-game-name       # create your branch
# ... work + commit frequently ...
git push origin my-game-name
# then go to GitHub -> create Pull Request from your branch to main
```

## Commit messages

We try to follow "Conventional Commits" style.  
The prefix tells everyone at a glance what kind of change it is.  
Most important ones for us right now:

- `feat:`      new feature / visible functionality
- `fix:`       bug fix (something was wrong and now works correctly)
- `docs:`      documentation, comments, README, etc.
- `style:`     formatting, whitespace, indentation, naming (no logic change)
- `refactor:`  improve internal code structure without changing observable behavior
- `test:`      adding or changing tests
- `chore:`     maintenance tasks, small cleanups, tooling updates that don't fit anywhere else
- `build:`     changes to build system (Makefile, compiler flags, dependencies, scripts)

Quick reference table:

| Type       | Purpose / When to use it                                                                 | Changes the produced game binary? | Semantic Versioning impact | Example commit message (our project context)                     |
|------------|------------------------------------------------------------------------------------------|-----------------------------------|----------------------------|-------------------------------------------------------------------|
| `feat`     | Introduces new functionality, like a new game mechanic or UI element.                   | Yes                               | Minor                      | feat: Added hold piece mechanic in Tetris                          |
| `fix`      | Corrects a defect in existing functionality, such as a crash or incorrect scoring.     | Yes                               | Patch                      | fix: Resolved collision detection issue in lobby platformer        |
| `docs`     | Changes that affect only documentation, comments, READMEs, or similar.                 | No                                | None                       | docs: Updated API conversion guide with new error handling section  |
| `style`    | Cosmetic changes: formatting, indentation, renaming without logic impact.              | No                                | None                       | style: Reformatted lobbyAPI.c to use consistent camelCase          |
| `refactor` | Internal code cleanup that improves structure but doesn't change external behavior.    | No                                | None                       | refactor: Extracted common player update logic to shared utils.h   |
| `test`     | Adding, updating, or fixing tests.                                                     | No                                | None                       | test: Added basic score overflow check for snake-classic            |
| `chore`    | Housekeeping: .gitignore, unused files, version bumps, small tooling tweaks, cleanups   | Almost never                      | None                       | chore: Ignored compile_commands.json and .cache/ in .gitignore     |
| `build`    | Makefile changes, new compiler flags, debug/release modes, linker options, etc.         | No (build only)                   | None                       | build: Added -O2 optimization to release build in Makefile          |

For very tiny things (fixing one typo in a variable name, removing trailing whitespace) you have two options:

- make a small `style:` or `chore:` commit
- fold it into the next meaningful commit (totally fine)

We are not being super strict about this yet - the goal is just to make the history more readable when reviewing PRs or looking back later.

If you are unsure which prefix to pick, just choose the one that feels closest and we can tweak it during review. Or ask in chat.

## Pull Requests (PRs)

Pull Requests (PRs) are the standard way we review, discuss, and safely integrate changes.  
We distinguish two main cases depending on the **target branch**:

1. PRs targeting a **feature/game branch** (e.g. `tetris`, `block-blast`, `snake`)
2. PRs targeting a **shared/global branch** (e.g. `main`, `docs-setup`, `sub-games-integration-test`)

### Branch Naming Convention (both cases)

All branches follow this pattern:

`<target-branch-name>-PR<sequential-number-of-the-day>-DD/MM`

Examples:
- `tetris-PR1-01/03`     → first PR of the day targeting `tetris` on March 1st
- `tetris-PR2-01/03`     → second PR of the day to `tetris`
- `main-PR3-02/03`       → third PR of the day targeting `main` on March 2nd
- `docs-setup-PR1-05/03` → first PR of the day to `docs-setup`

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
- Go to GitHub → "Pull requests" → "New pull request"
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
- GitHub → "New pull request"
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
**Last updated: March 01, 2026**  
**Author: [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)**