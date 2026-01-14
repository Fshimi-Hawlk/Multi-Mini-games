# CONTRIBUTING.md

Internal guidelines for the Multi Mini-Games project

This is a monorepo with branch-per-game workflow. All team members coordinate via GitHub issues/PRs or direct discussion.

## Workflow

### Branches
- Develop features or fixes on branches off `main` (e.g., `git checkout -b feature/custom-boards`).
- New mini-games: dedicated branch from `main` (`git checkout -b new-game-name`).
- Keep branches focused and short-lived.
- When ready:
  - Open PR to `main` (for merges/shared changes) or target branch.
  - Discuss big changes in team chat/meeting first.

### Merging Sub-Games
- Once a game branch is stable: PR to merge into `main` as a folder.
- Post-merge: integrate as lib/module for lobby scene switching.

## Code Style & Conventions
- See separate file: [Code Style & Conventions](./CodeStyleAndConventions.md)

## Commit Messages
Use Conventional Commits:
- `feat: add durability tiles`
- `fix: correct streak scoring`
- `docs: update build instructions`
- `refactor: split shape logic`
- `test: add placement tests`

## Pull Requests
- Title: concise summary.
- Description: explain what/why, link related issues.
- Keep PRs small for quick review.
- Run checks locally: `make MODE=clang-debug`, `make run-tests`.

## Issues
- Create GitHub issues for bugs, features, or discussions.
- Use labels: bug, enhancement, documentation, etc.

## Testing
- Add unit/integration tests in `tests/` for new logic.
- Always run `make run-tests` (prefer `MODE=clang-debug` or `valgrind-debug`) before committing/PR.

## General
- Use raylib only (no external windowing changes).
- Prefix game-specific functions (e.g., `bb_` for Block Blast).
- Update Doxygen comments for public API.
- Ping team on PRs for review.

Coordinate on larger changes to avoid conflicts.