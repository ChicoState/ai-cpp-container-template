# Spec and Approval Plan: C++ CLI Calculator MVP

## Status

**Complete — verified locally; awaiting review and commit.**

## Objective

Build a small interactive command-line calculator for a person working in a
terminal. The MVP supports addition and subtraction only, so it establishes a
clear command format and validation behavior without adding parsing complexity
for arbitrary mathematical expressions.

### Assumptions to approve

- Input is interactive, one command per line; this is not an expression parser.
- Commands use integer operands: `add <a> <b>` and `sub <a> <b>`.
- A user leaves the program with `exit`.
- Invalid input reports an error and keeps the calculator running.
- Results and errors are written to standard output; no files, persistence, or
  external dependencies are involved.

### Success criteria

- `add 2 3` prints `5`.
- `sub 9 4` prints `5`.
- Negative integer operands work (for example, `add -2 3` prints `1`).
- A blank, unknown, missing-operand, or non-integer command prints a clear
  error and the next valid command still works.
- `exit` ends the program with a zero exit status.
- The documented build and automated test commands pass from a clean checkout.

## Tech Stack

- C++17, compiled with `g++`; no third-party libraries.
- POSIX shell integration tests, retaining the repository's existing
  `test_runner.sh` entry point.

## Commands

```bash
# Build the executable
g++ -std=c++17 -Wall -Wextra -Werror main.cpp -o app

# Run it interactively
./app

# Run the automated CLI checks
./test_runner.sh
```

## Project Structure

```text
main.cpp                        # CLI loop, parsing, validation, calculations
tests/calculator_cli_test.sh     # Black-box scripted input/output checks
test_runner.sh                   # Compiles the app and invokes the test script
specs/calculator-mvp.md          # This MVP specification and implementation plan
README.md                        # Usage and supported-command documentation
```

## Code Style

- Use C++17 standard library facilities only; no global mutable state.
- Name functions and local variables in `snake_case`; use `const` when values
  do not change.
- Keep parsing separate from the calculation operation so future commands can
  be added without reshaping the input loop.
- Print one result or one actionable error per submitted command.

```cpp
int add(const int left, const int right) {
  return left + right;
}

// Input:  "sub 9 4"
// Output: "5\n"
```

## Testing Strategy

The MVP uses black-box integration tests because its public surface is terminal
input and output. `tests/calculator_cli_test.sh` will compile through the test
runner, pipe a sequence of commands to the executable, and compare the complete
output with the expected transcript.

Required cases:

- addition with positive and negative integer operands;
- subtraction with positive and negative integer operands;
- blank, unknown, incomplete, and non-integer commands;
- recovery after an invalid command;
- clean termination on `exit`.

No percentage coverage target is proposed for this small executable; every
supported command and listed invalid-input category must have a test.

## Boundaries

- **Always:** validate command arity and integer conversion; run compiler
  warnings and the automated tests before committing; keep the feature branch
  short-lived and merge through review.
- **Ask first:** adding dependencies or a test framework; changing CI or
  container configuration; adding multiplication, division, floating point,
  expression parsing, command-line arguments, history, or persistence.
- **Never:** commit binaries, secrets, generated build artifacts, or edits to
  vendored/container tooling solely to implement this MVP; remove failing tests
  to make a build pass.

## Branching and Commit Strategy

- This work is isolated on `feature/calculator-mvp`, branched from the clean
  `main` baseline.
- Keep `main` untouched and deployable. Make small, tested commits, proposed as:
  1. `docs: add calculator mvp specification`
  2. `test: define calculator cli behavior`
  3. `feat: implement addition and subtraction commands`
  4. `docs: document calculator usage`
- Open a PR from `feature/calculator-mvp` to `main`; merge only after review
  and a passing test run, then delete the feature branch.

## Implementation Plan (After Approval)

1. **Establish the behavior contract with tests.** Add a scripted CLI test and
   improve `test_runner.sh` so it compiles with C++17 warnings and executes the
   test. This captures expected results, invalid-input recovery, and `exit`
   before production code changes.
2. **Implement the calculator loop.** Replace the placeholder `main.cpp` with
   line-based command parsing, strict integer validation, `add` and `sub`
   operations, consistent output, and an `exit` path.
3. **Document usage.** Update `README.md` with the compile/run/test commands,
   supported syntax, and a short terminal example.
4. **Verify and prepare review.** Run the full test command and a manual
   transcript; inspect the staged diff for scope and generated binaries before
   committing the implementation slices and opening a PR.

### Dependencies and sequencing

Steps 1 and 2 are sequential: the test contract must exist before the command
loop is implemented. Step 3 can follow Step 2, and Step 4 depends on all prior
steps. This scope is small enough that parallel implementation is unnecessary.

### Risks and mitigations

| Risk | Mitigation |
| --- | --- |
| Ambiguous input syntax grows into an expression parser | Restrict MVP input to the two documented commands and seek approval for expansion. |
| `std::stoi` accepts partial numeric text | Confirm that the full operand token is consumed during parsing. |
| Tests accidentally validate prompts instead of behavior | Keep prompts minimal and assert results/errors plus termination behavior. |
| Compiled `app` appears in Git status | Add or confirm an ignore rule before the first build if needed; do not commit binaries. |

## Implementation Tasks (After Approval)

- [x] Task: Add executable-level behavior tests and make `test_runner.sh` run them.
  - Acceptance: The command captures all required valid, invalid, recovery, and exit cases.
  - Verify: `./test_runner.sh` fails against the placeholder program and passes once behavior is implemented.
  - Files: `tests/calculator_cli_test.sh`, `test_runner.sh`.

- [x] Task: Implement the interactive `add`/`sub` calculator.
  - Acceptance: Every success criterion under Objective is met with deterministic output and no third-party dependency.
  - Verify: `g++ -std=c++17 -Wall -Wextra -Werror main.cpp -o app` and `./test_runner.sh` succeed; manually run `printf 'add 2 3\nexit\n' | ./app`.
  - Files: `main.cpp`.

- [x] Task: Publish the user-facing usage instructions.
  - Acceptance: A new user can build, run, test, and use all MVP commands from `README.md`.
  - Verify: Follow the documented commands in a clean shell and confirm the transcript matches the spec.
  - Files: `README.md`.

## Open Questions

Approval of the stated assumptions resolves the MVP. If you prefer another
interface—such as `2 + 3`, non-interactive command-line arguments, floating
point numbers, a different quit command, or a required greeting/prompt—please
specify it before implementation so the test contract can reflect it.
