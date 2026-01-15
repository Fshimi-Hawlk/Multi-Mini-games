# CONTRIBUTING.md

Internal guidelines for the Multi Mini-Games project

This is a [monorepo](https://wellarchitected.github.com/library/scenarios/monorepos/) with branch-per-game workflow.

## Workflow

### Branches
- Develop features or fixes on branches off `main` (e.g., `git checkout -b feature/custom-boards`).
- New mini-games: dedicated branch from `main` (`git checkout -b new-game-name`).
- Keep branches focused and short-lived.
- When ready:
  - Open [PR](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/proposing-changes-to-your-work-with-pull-requests/about-pull-requests) to `main` (for merges/shared changes) or target branch.
  - Discuss big changes in team chat/meeting first.

### Merging Sub-Games
- Once a game branch is stable: PR to merge into `main` as a folder.
- Post-merge: integrate as lib/module for lobby scene switching.

## Code Style & Conventions
- See separate file: [Code Style & Conventions](CodeStyleAndConventions.md)
- You certainly have your own style and convention, and that's fine. But try to respect
some of it to be consistent. If you disagree with some of the points, propose a change 
and we'll discuss about it.

## Commit Messages
Use Conventional Commits:
- `feat: add durability tiles` (feature)
- `fix: correct streak scoring`
- `docs: update build instructions` (documentation)
- `refactor: split shape logic`
- `test: add placement tests`

## Pull Requests
- Title: concise summary.
- Description: explain what/why.
- Keep PRs small for quick review.
- Run checks locally: `make MODE=clang-debug`, `make run-tests`.

## Testing
- Add unit/integration tests in `sub-project/tests/` for new logic.
- Always run `make run-tests` (prefer `MODE=clang-debug` or `valgrind-debug`) before committing/PR.

## General
- Use raylib.
- [WIP] Prefix game-specific functions (e.g., `subProject_` for any sub-project).
- Update Doxygen comments for public API.
- Ping team on PRs for review.