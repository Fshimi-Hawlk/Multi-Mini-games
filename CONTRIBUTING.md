# CONTRIBUTING.md

This project uses a monorepo with branch-per-game workflow. Contributions are welcome via issues or pull requests.

## How to Contribute

### 1. Branch Workflow
- Work on feature branches off `main` (e.g., `git checkout -b feature/new-shape-system`).
- For new mini-games: create a dedicated branch (`git checkout -b new-game-name`).
- Keep branches focused — one feature or game at a time.
- When ready: open PR to `main` (for merges) or relevant branch.

### 2. Code Style & Conventions
- See the markdown file: [code style & conventions](./CodeStyleAndConventions.md)

### 3. Commit Messages
- Conventional Commits style:
  - `feat: add custom board support`
  - `fix: resolve placement overlap bug`
  - `docs: update README with build instructions`
  - `refactor: extract shape generation logic`

### 4. Pull Requests
- Title: Clear summary.
- Description: What/why, reference issues if any.
- Keep PRs small and reviewable.
- Ensure builds pass (`make MODE=clang-debug`, `make run-tests`).

### 5. Issues
- Use GitHub Issues for bugs, features, or ideas.
- Label appropriately (bug, enhancement, documentation).

### 6. Testing
- Add tests in `tests/` when possible.
- Run `make run-tests` before PR.

Questions? Open an issue or discuss in PR comments.

Happy coding!