@page contributing Contributing Guidelines

# CONTRIBUTING

Guidelines for working together on the Multi Mini-Games project

We're all figuring out collaborative Git workflows at the same time - mistakes will happen and that's okay.  
The most important rule is: communicate.

@section basic_workflow Basic Workflow (very simplified)

1. Everything stable lives on branch @c main
2. Each mini-game gets its own branch (example: @c block-blast, @c snake-classic, @c memory-game)
3. You work only on your branch
4. When your game is playable and reasonably clean -> you create a Pull Request (PR) to merge it into @c main
5. After review + any small fixes -> merge -> the game becomes part of the main project

@section git_commands Git commands you will use most

```bash
git pull origin main               # always start
git checkout -b my-game-name       # create your branch
# ... work + commit frequently ...
git push origin my-game-name
# then go to GitHub -> create Pull Request from your branch to main
```

@section commit_messages Commit messages

We try to follow "Conventional Commits" style.  
The prefix tells everyone at a glance what kind of change it is.  
Most important ones for us right now:

- @c feat:      new feature / visible functionality
- @c fix:       bug fix (something was wrong and now works correctly)
- @c docs:      documentation, comments, README, etc.
- @c style:     formatting, whitespace, indentation, naming (no logic change)
- @c refactor:  improve internal code structure without changing observable behavior
- @c test:      adding or changing tests
- @c chore:     maintenance tasks, small cleanups, tooling updates that don't fit anywhere else
- @c build:     changes to build system (Makefile, compiler flags, etc.)

For very tiny things (fixing one typo in a variable name, removing trailing whitespace) you have two options:

- make a small @c style: or @c chore: commit
- fold it into the next meaningful commit (totally fine)

We are not being super strict about this yet - the goal is just to make the history more readable when reviewing PRs or looking back later.

If you are unsure which prefix to pick, just choose the one that feels closest and we can tweak it during review. Or ask in chat.

@section prs Pull Requests (PRs)

- Title: short sentence describing the change
- Description: explain the why (especially for bigger changes)
- Before opening PR: run @c make MODE=clang-debug run-tests

@section code_style Code Style

See @ref code_style

Even partial adherence helps - big style differences make everyone's life harder when reading/reviewing.

@section reviews Reviews & questions

- Ping the group chat when you open a PR
- It's normal to ask "is this okay?" or "what do you think about this approach?"
- Merge conflicts happen - ask for help when they do, there's no shame

Thanks for working together - take your time, ask questions, and enjoy making the games!

**Created:** January 15, 2026  
**Last updated:** March 02, 2026  
**Author:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)