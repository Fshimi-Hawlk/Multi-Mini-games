# CONTRIBUTING.md

Guidelines for working together on the Multi Mini-Games project

We're all figuring out collaborative Git workflows at the same time. — mistakes will happen and that's okay.  
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

## Commit messages — our simple rule

We try to follow "Conventional Commits" — it looks like this:

- `- feat: Add player movement with arrow keys`
- `- fix: Prevent crash when clicking too fast`
- `- doc: Improve README installation steps`
- `- style: Reformat code + fix indentation`
- `- refactor: Move repeated code into utilsDrawRectangle()`
- `- test: Add checks for score calculation`

For trivial changes (whitespace, better variable name, ...), either make a tiny commit (style: trim trailing whitespace) or fold it into the next meaningful commit — both are fine.

## Pull Requests (PRs)

- Title: short sentence describing the change
- Description: explain the why (especially for bigger changes)
- Before opening PR: run `make MODE=clang-debug run-tests`

## Code Style

So, to start things up, see the linked [CodeStyleAndConventions.md](./CodeStyleAndConventions.md)

-> It's mostly how I've been writing so far.  
-> Goal => have some baseline consistency, I'm not expecting perfect uniformity.  
-> If anything feels inconvenient, overly strict/pedantic, or just wrong — bring it up (chat or PR that edits the file). We can adopt parts, ignore parts, or write something new together.  

Even partial adherence helps — big style differences make everyone's life harder when reading/reviewing.

## Reviews & questions

- Ping the group chat when you open a PR
- It's normal to ask "is this okay?" or "what do you think about this approach?"
- Merge conflicts happen — ask for help when they do, there's no shame

Thanks for working together — take your time, ask questions, and enjoy making the games!