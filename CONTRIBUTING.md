# CONTRIBUTING.md

Guidelines for working together on the Multi Mini-Games project

We're all figuring out collaborative Git workflows at the same time. - mistakes will happen and that's okay.  
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
| `feat`     | Introduces new gameplay, mechanic, screen, scoring rule, etc.                            | Yes                               | Minor ↑                    | feat: Added block-blast piece rotation with X key                   |
| `fix`      | Corrects broken / incorrect behavior (crashes, wrong scoring, visual glitches, etc.)    | Yes                               | Patch ↑                    | fix: Snake no longer wraps around when wall collision is off      |
| `refactor` | Reworks code structure, extracts helpers, removes duplication, improves readability     | Usually yes                       | None                       | refactor: Moved repeated DrawRectangle calls into `ui/game.c`     |
| `docs`     | Changes to comments, README, INSTALL, doxygen-style docs, game instructions              | No                                | None                       | docs: Documented memory-game power-up spawn probability             |
| `style`    | Pure formatting: indentation, line breaks, trailing whitespace, brace placement         | No                                | None                       | style: Trimed some white space                  |
| `test`     | Adding, fixing or improving test code (even if tests are very basic right now)          | No (tests only)                   | None                       | test: Added basic score overflow check for snake-classic            |
| `chore`    | Housekeeping: .gitignore, unused files, version bumps, small tooling tweaks, cleanups   | Almost never                      | None                       | chore: Ignored compile_commands.json and .cache/ in .gitignore     |
| `build`    | Makefile changes, new compiler flags, debug/release modes, linker options, etc.         | No (build only)                   | None                       | build: Added -O2 optimization to release build in Makefile          |

For very tiny things (fixing one typo in a variable name, removing trailing whitespace) you have two options:

- make a small `style:` or `chore:` commit
- fold it into the next meaningful commit (totally fine)

We are not being super strict about this yet - the goal is just to make the history more readable when reviewing PRs or looking back later.

If you are unsure which prefix to pick, just choose the one that feels closest and we can tweak it during review. Or ask in chat.

## Pull Requests (PRs)

- Title: short sentence describing the change
- Description: explain the why (especially for bigger changes)
- Before opening PR: run `make MODE=clang-debug run-tests`

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