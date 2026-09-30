# Feature: Terminal Rock–Paper–Scissors MVP

## Feature Description

Create a self-contained C++ terminal game in which one local player plays rock–paper–scissors against a computer opponent. The game uses a first-to-two-wins match (best of three decisive rounds), presents simple labeled ASCII-art move cards side-by-side after each valid round, and lets the player start another match by entering `y` after the result.

The feature gives the currently empty C++ template a complete, approachable interactive program while keeping the implementation limited to standard C++ and terminal output.

## User Story

As a terminal user
I want to play a short rock–paper–scissors match against the computer
So that I can see an immediately playable C++ program with clear text-based visual feedback.

## Tech Stack

- Language: ISO C++17.
- Runtime: a standard terminal using `std::istream` and `std::ostream`.
- Build: `g++` invoked by `test_runner.sh` with `-std=c++17 -Wall -Wextra -Wpedantic`.
- Libraries: the C++ standard library only (`<random>`, streams, strings, and standard containers/utilities as needed).
- Test support: repository-local explicit `CHECK` helpers; no third-party framework or package is needed for this MVP.

## Commands

Run these commands from the repository root after implementation:

```sh
# Build the production executable, run unit tests, integration tests, and an EOF smoke test.
./test_runner.sh

# Run the game interactively after the test runner has built it.
./build/app

# Run the same verification workflow in the documented C++ container.
docker run --rm -v "$(pwd)":/usr/src -w /usr/src cpp-container ./test_runner.sh
```

The runner must compile `main.cpp` and `rps_game.cpp` into `build/app`, then compile each test target against `rps_game.cpp` with `-I.` and the C++17 warning flags.

## Project Structure

```text
main.cpp                 Production entry point; wires standard streams and random computer moves.
rps_game.hpp            Public game model, rendering, parsing, and testable game-loop declarations.
rps_game.cpp            Rules, input normalization, rendering, and stream-based game-loop implementation.
test_runner.sh           Location-independent build/test runner producing ignored build artifacts.
tests/rps_game_test.cpp  Unit tests for rules, normalization, score handling, and renderer invariants.
tests/rps_cli_test.cpp   Deterministic integration tests for the complete terminal interaction.
specs/                   Product specification and implementation plan.
build/                   Generated executables; already ignored by Git.
```

## Code Style

- Use `enum class` for moves and outcomes; do not represent game state with magic integers or raw characters outside parsing.
- Keep comparison, score, normalization, and rendering functions small and deterministic.
- Pass streams and the computer-move provider into the game loop; do not use global mutable state.
- Use clear lower-camel-case function names and PascalCase types.
- Keep production randomness in `main.cpp`; tests must use fixed move providers.

Example expected style:

```cpp
std::optional<Move> parseMove(const std::string& input) {
  const std::string normalized = normalizeInput(input);
  if (normalized == "r") return Move::Rock;
  if (normalized == "p") return Move::Paper;
  if (normalized == "s") return Move::Scissors;
  return std::nullopt;
}
```

## Boundaries

- Always: normalize and validate input; compile with the specified C++17 warnings; run `./test_runner.sh`; keep rule and CLI tests deterministic; preserve the confirmed game behavior.
- Ask first: adding any dependency, changing the C++ standard, changing the test/build workflow beyond this feature, adding color/sound/windowing, or extending gameplay rules.
- Never: commit generated `build/` artifacts, rely on random results in automated tests, introduce global mutable match state, remove tests to make a build pass, or add networking/persistence to this MVP.

## Problem Statement

The repository has only an empty `main` function, so it does not demonstrate an interactive application. A small game must provide a complete input, game-state, output, and replay loop without adding GUI, network, persistence, or third-party-library complexity.

## Solution Statement

Implement the game as small deterministic rules, rendering functions, and a testable terminal-loop function. Normalize move and replay input by trimming surrounding whitespace and converting it to lower case; accept `r`, `p`, `s`, and `y` after normalization. In production, choose computer moves randomly; in tests, inject a fixed move sequence. Render same-size ASCII cards under `Player` (left) and `Computer` (right) headings. Always emit the side-by-side layout; narrow terminals may wrap naturally. Track decisive round wins until either side reaches two. A tie is displayed but neither changes the score nor consumes a decisive round. At match end, normalized `y` starts a clean new match and any other response exits.

## Relevant Files

Use these files to implement the feature:

- `main.cpp` — Replace the empty program with the terminal application loop and startup/exit behavior.
- `test_runner.sh` — Replace the current interactive compile-and-run-only harness with explicit production, unit-test, and deterministic integration-test targets built under `build/`.
- `README.md` — Document the local/container build-and-run command and the controls (`r`, `p`, `s`, and replay `y`).
- `tests/README.md` — Replace the placeholder with the test location and execution instructions if the repository convention is to keep test documentation there.

### New Files

- `rps_game.hpp` — Declare the move, outcome, score, normalization/parsing, rendering, and injectable terminal-loop interfaces shared by the executable and tests.
- `rps_game.cpp` — Implement normalization/parsing, winner calculation, score updates, fixed-width labeled side-by-side ASCII rendering, and the terminal loop parameterized by a computer-move provider.
- `tests/rps_game_test.cpp` — Provide non-interactive explicit-check unit tests for game rules, normalization, and ASCII layout.
- `tests/rps_cli_test.cpp` — Provide deterministic stream-based integration tests for prompts, match flow, replay, quit, and end-of-file behavior.

## Implementation Plan

### Phase 1: Foundation

Define a small domain model (`Move`, round outcome, and match score) and pure functions for normalization/parsing, comparison, score updates, and rendering. Define a terminal-loop function that accepts input/output streams and a computer-move provider, so tests do not depend on random outcomes. Establish explicit app, unit-test, and integration-test builds in an ignored `build/` directory before adding interactive behavior.

### Phase 2: Core Implementation

Build the round loop: normalize and validate the user move, obtain the computer move from the provider, render both selected moves side-by-side under headings, announce the result, and update/display the score. Continue tied rounds without advancing either player’s score or consuming a decisive round, and stop a match as soon as one side has two wins.

### Phase 3: Integration

Wrap the match loop in a replay loop, using normalized `y` to begin a new reset match and treating every other response as exit. Keep production randomness in `main.cpp`; supply a fixed move sequence in the integration tests. Document the controls and run the full build and test sequence in both host and container-compatible workflows.

## Step by Step Tasks

IMPORTANT: Execute every step in order, top to bottom.

### 1. Define the testable game contract

- Add `rps_game.hpp` with a strongly typed representation for rock, paper, and scissors; a round-result type; score state; and declarations for input normalization/parsing, comparison, score update, rendering, and the stream-based game loop.
- Specify that a round is tied only when both moves match, and that rock beats scissors, scissors beats paper, and paper beats rock.
- Specify a renderer that returns equal-height, fixed-width lines with `Player` content first (left), `Computer` content second (right), and an explicit separator. The renderer must always emit this layout; it does not inspect or reject narrow terminal widths.
- Define the game loop to receive `std::istream`, `std::ostream`, and a computer-move provider. Keep random-engine ownership in `main.cpp`, not in rules or tests.

### 2. Add automated rules and renderer tests

- Create `tests/rps_game_test.cpp` and `tests/rps_cli_test.cpp` using a small explicit `CHECK` helper that reports failures and returns nonzero; do not use assertions that could be compiled out and do not add a testing dependency for this MVP.
- Test valid lower- and upper-case inputs (`r`, `p`, `s`, `R`, `P`, `S`) with surrounding whitespace, and invalid normalized input values.
- Test all nine player/computer combinations, including all three ties, to prove winner calculation.
- Test that ties do not alter scores and each decisive result increments only the winning side.
- Test that generated ASCII-art lines include `Player` and `Computer` headings, place player content before computer content on every row, and maintain equal row counts, fixed widths, and a fixed separator for every move pair.
- In `tests/rps_cli_test.cpp`, pass `std::istringstream` input and a fixed computer-move sequence to prove invalid-input re-prompting, a tie at `0–0`, two decisive wins, the winner message, replay with normalized `Y`, reset scores, non-`y` quit, and clean EOF exit.

### 3. Implement game rules and ASCII move cards

- Implement `rps_game.cpp` to satisfy the tests with no global mutable match state.
- Trim surrounding whitespace and convert input to lower case before validating one-character moves or replay responses.
- Create one deliberately simple, legible ASCII card for each move; all cards must have the same number of lines and width so the output does not shift between rounds. Print `Player` and `Computer` headings above the cards and permit natural terminal wrapping when the output is wider than the viewport.
- Return user-readable labels and round-result text from helpers where it keeps `main.cpp` focused on wiring production streams and randomness.

### 4. Update the terminal application loop

- Replace `main.cpp`’s empty implementation by constructing a `std::mt19937` seeded from `std::random_device`, mapping a `std::uniform_int_distribution<int>{0, 2}` to legal moves, and passing that provider with `std::cin`/`std::cout` to the game loop.
- For each valid round, normalize the player input, obtain a computer move, render the player on the left and computer on the right below their headings, display the round result, and print the score.
- Continue until either score reaches two; ties are shown but do not advance the best-of-three match.
- After the winner message, prompt `Play again? (y to continue):`; after trimming and case-normalizing, start with zeroed scores only for `y`, otherwise print an exit message and terminate successfully.
- Handle end-of-file on standard input as a clean exit instead of looping indefinitely.

### 5. Make the test harness build deterministic targets

- Update `test_runner.sh` to resolve its own directory, enable `set -euo pipefail`, create the already-ignored `build/` directory, and compile from explicit source lists.
- Compile production `main.cpp` plus `rps_game.cpp` to `build/app`; compile each test separately with `rps_game.cpp`, `-I.`, `-std=c++17`, `-Wall`, `-Wextra`, and `-Wpedantic`.
- Run `build/rps_game_test`, `build/rps_cli_test`, and an EOF smoke test such as `printf '' | build/app`; never launch an unbounded interactive app from the test runner.

### 6. Document the MVP and validate it end-to-end

- Update `README.md` with commands to run the supplied harness and launch the game inside the documented container environment.
- Document controls, input normalization, first-to-two-decisions semantics, tie behavior, replay behavior, labeled side-by-side ASCII layout, natural wrapping on narrow terminals, and the terminal-only scope.
- Run every command in **Validation Commands**, including deterministic automated CLI coverage of invalid input, a tie, a completed match, ASCII cards on both sides, replay, and quit. Perform one short manual interactive smoke test for production randomness and presentation.

## Testing Strategy

### Unit Tests

- Normalization/parsing tests verify `r`, `p`, and `s` are accepted after case conversion and whitespace trimming, while empty, multi-character, and unrelated values are rejected.
- Outcome tests cover the complete 3×3 move matrix.
- Score tests verify ties leave scores unchanged and decisive rounds increment exactly one score.
- Renderer tests verify `Player`/`Computer` headings, equal-height and equal-width cards, a fixed separator, and player-left/computer-right ordering for every move pair.
- Stream-based integration tests use a fixed computer-move provider to test the complete output and state transitions without relying on production randomness.

### Edge Cases

- Empty input, whitespace-only input, multi-character input, and arbitrary text are rejected and re-prompted; upper-case valid controls and surrounding whitespace are accepted.
- A tie in the final potential decisive round does not incorrectly end the match.
- The score resets before a replay match and does not carry over.
- Any replay answer other than `y` after trimming and case normalization exits.
- Input closes during a move or replay prompt without a crash or endless prompt.
- Random selection always maps to exactly one legal computer move.

## Success Criteria

- The program compiles as C++17 with warnings enabled and runs in a terminal without third-party dependencies.
- A player can enter `r`, `p`, or `s` in either case with optional surrounding whitespace; invalid input is explained and the move prompt repeats.
- Each valid round shows simple, aligned ASCII art below `Player` (left) and `Computer` (right) headings. A narrow terminal may wrap this fixed layout naturally.
- Rock defeats scissors, scissors defeats paper, paper defeats rock, and identical moves are ties.
- The first side to two decisive wins is declared the match winner; ties neither award a point nor end the match.
- At match end, `y` in either case with optional surrounding whitespace starts a zero-score new match; all other input ends the program cleanly.
- Automated tests cover normalization/parsing, all round outcomes, scoring, renderer layout, and deterministic full CLI flow; all pass through `test_runner.sh` without launching an unbounded interactive session.

## Validation Commands

Execute every command to validate the feature works correctly with zero regressions.

```sh
./test_runner.sh
```

```sh
docker run --rm -v "$(pwd)":/usr/src -w /usr/src cpp-container ./test_runner.sh
```

```sh
./build/app
```

Manual verification for the interactive session:

- Enter ` R ` and confirm it is accepted; enter invalid text and confirm the move prompt repeats.
- Confirm every valid round prints `Player` art on the left and `Computer` art on the right. Resize the terminal narrower if desired and confirm natural wrapping is acceptable.
- Confirm a tie leaves the score unchanged, a match winner appears after two decisive wins, and ` Y ` starts a new `0–0` match.
- At the next replay prompt, enter a non-`y` response and confirm a clean exit.

## Notes

- No new library is required; use the C++ standard library only.
- The exact appearance of each ASCII card is intentionally not prescribed, provided it is simple, readable, fixed-size, labeled, and rendered side-by-side; it may wrap naturally in narrow terminals.
- The current `test_runner.sh` compiles every root `*.cpp` into an interactive app. The revised harness must use explicit target lists and `build/` outputs, because a bare `app` executable is not ignored by the existing `.gitignore` rule.
- This plan intentionally leaves out cumulative scores, custom game lengths, colors, sound, graphical windows, networking, and persistence.

## Open Questions

None. The match, tie, input-normalization, replay, ASCII-heading, and narrow-terminal behaviors have all been explicitly decided.
