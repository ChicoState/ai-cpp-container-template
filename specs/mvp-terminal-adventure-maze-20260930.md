# Feature: Minimum Viable Terminal Adventure Maze Game

## Feature Description

Create a small, self-contained C++ terminal game in which a player explores one
handcrafted maze, collects a required key (or treasure), avoids a simple moving
hazard, and reaches a locked exit to win as quickly as possible. A visible
completion timer turns the fixed level into a replayable challenge. The game is
deliberately limited to one level and standard console input/output so it can
be completed, tested, and demonstrated without a graphics engine or
third-party libraries.

## User Story

As a player
I want to navigate a maze, find the key, avoid danger, and escape against a
running timer
So that I can improve my completion time on a short but demanding challenge.

## Problem Statement

The repository contains only an empty C++ entry point. It needs a complete but
appropriately small gameplay loop that demonstrates movement, maze collision,
an objective, a competitive time challenge, and an ending state. It must also
serve as a portfolio-quality C++ foundation rather than a one-off script.

## Solution Statement

Model the maze as a fixed grid of tiles and keep all game state in a `Game`
object. Render that state as ASCII, accept one command per turn, reject invalid
moves without changing the state, then advance the hazard and evaluate win/loss
conditions. Isolating the grid rules from terminal I/O makes the core mechanics
easy to unit-test without external libraries.

The terrain grid is immutable; the player, hazard, key-collected flag, and
status are dynamic overlays. This prevents rendering or movement from erasing a
wall, exit, or key tile after an actor leaves it.

The game starts its timer on the first successful player movement and stops it
on victory, defeat, quit, or input EOF. The core receives a caller-supplied
`std::chrono::steady_clock::time_point` when it processes a command; `main.cpp`
supplies the real clock and tests supply fixed timestamps. That creates
deterministic timing tests without adding an unnecessary clock abstraction.

## Relevant Files

- `main.cpp` — current empty entry point; will become the application bootstrap
  and interactive game loop.
- `README.md` — will document how to build, run, and play the game.
- `test_runner.sh` — existing compile-and-run helper; should be updated only if
  needed to compile the new test executable(s).
- `tests/README.md` — existing test-folder documentation; can be updated with
  the chosen test command and layout.

### New Files

- `game.h` — public game-state types and the testable `Game` interface.
- `game.cpp` — maze setup, movement rules, hazard movement, and outcome logic.
- `tests/game_tests.cpp` — assertion-based unit tests for game rules.
- `tests/playthrough_test.sh` — non-interactive terminal integration test that
  feeds a winning command sequence to the compiled program and checks its
  result.
- `tests/timer_tests.cpp` — focused, deterministic timer checks; it may be
  folded into `tests/game_tests.cpp` if that keeps the suite simpler.

## Architecture Decisions

- Use C++17 and only the standard library; this avoids installation and runtime
  dependencies in the provided C++ container.
- Use `std::chrono::steady_clock`, never wall-clock time, for elapsed time so
  clock adjustments cannot alter a run.
- Store the single level as a fixed rectangular grid. Tile constants distinguish
  walls, floor, key, exit, player, and hazard during rendering.
- Use WASD for movement and `q` to quit. Commands are line based, which works
  consistently in ordinary terminals and in scripted tests.
- Make the exit remain locked until the key is collected. A collision with the
  hazard ends the game immediately.
- Move the hazard on a deterministic, documented route after each valid player
  turn. Determinism keeps the challenge understandable and makes tests stable.
- Resolve each command in this exact order: `q` quits; blank, unknown, blocked,
  and out-of-bounds commands change neither actor; a legal player move that
  reaches the hazard loses immediately; otherwise collect a key, then win
  immediately at an unlocked exit; otherwise move the hazard once and lose if
  it reaches the player. A completed game accepts no further state changes.
- Start timing on the first successful move—not when instructions render—so a
  player can read the screen before competing. Show elapsed time while playing
  and a final `mm:ss.cc` completion time on victory. Best-time persistence is
  intentionally deferred beyond the MVP.
- Keep public interfaces small and value-oriented: no global mutable state, no
  `using namespace` in headers, const-correct query methods, and a single game
  state owner.

## Implementation Plan

### Phase 1: Foundation

Define the tile model, position type, game result states, timer state, and
`Game` API. Create a small rectangular maze whose spawn, key, exit, and hazard
positions are all reachable and whose intended winning route can be documented.

### Phase 2: Core Implementation

Implement rendering, the visible timer, and the turn sequence: read a command,
attempt player movement, collect the key when entered, move the hazard, and
evaluate loss or win conditions. Print concise instructions and clear status
messages.

### Phase 3: Integration

Connect `main.cpp` to the game object, add automated unit and scripted
playthrough coverage, document commands, and run the project validation suite.

## Step by Step Tasks

### 1. Define the game contract and winning level

- Add `Position`, tile representation, `GameStatus`, and a small `Game` API.
- Separate immutable terrain from the dynamic player and hazard positions.
- Define timer query methods and pass timestamps into command processing so
  game rules never call a clock directly.
- Write down the coordinate system, hazard route, and intended winning command
  sequence as comments or tests.
- Keep the maze data private to the game implementation.

Acceptance:

- The starting player, key, exit, and hazard positions are unique and valid.
- A route exists from the spawn to the key and then to the exit.
- The documented winning route does not intersect the hazard at its scheduled
  positions.
- Headers expose no mutable global state and compile independently.

Verification:

- Compile the game core with
  `g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -c game.cpp`.

Dependencies: None.

Likely files: `game.h`, `game.cpp`.

### 2. Implement and test player movement and walls

- Add conversion from `w`, `a`, `s`, and `d` to a movement direction.
- Move the player only into in-bounds, non-wall cells.
- Leave state unchanged for wall, boundary, empty, or unknown commands.
- Add independent unit tests for each direction and blocked movement.

Acceptance:

- Valid movement changes the player position by exactly one tile.
- A player cannot leave the maze or pass through a wall.
- Invalid, blocked, and empty input do not start the timer before its first
  legal move; after the timer starts, elapsed real time continues normally.

Verification:

- Compile and run `tests/game_tests.cpp` against the game core.

Dependencies: Task 1.

Likely files: `game.h`, `game.cpp`, `tests/game_tests.cpp`.

### 3. Implement the competitive timer and testable time rules

- Start elapsed time only when a legal player move succeeds.
- Calculate elapsed time from supplied `steady_clock` timestamps and expose a
  display-ready duration without formatting terminal output in the game core.
- Freeze the final duration on win, loss, quit, and EOF.
- Render live elapsed time in `main.cpp` and display the frozen completion time
  only for a victory.
- Add deterministic tests using fixed timestamps; do not use sleeps in tests.

Acceptance:

- Reading instructions or entering invalid input does not affect a run time.
- A successful move starts the timer exactly once.
- Terminal states freeze elapsed time permanently.
- The final victory time is formatted consistently as `mm:ss.cc`.

Verification:

- Run timing tests without waiting for real time to pass.

Dependencies: Task 2.

Likely files: `game.h`, `game.cpp`, `main.cpp`, `tests/game_tests.cpp`.

### 4. Implement the key, locked exit, and hazard turn

- Mark the key as collected when the player enters its tile.
- Prevent completion at the exit before the key is held, with a clear message.
- Move the hazard one deterministic step after each valid player turn.
- Apply the documented turn order so a player collision is checked before key
  or exit resolution and an unlocked exit wins before a subsequent hazard move.
- End the game on either player-into-hazard or hazard-into-player collision.
- Add unit tests for collection, locked-exit behavior, hazard movement, and both
  collision directions.

Acceptance:

- The exit wins the game only after collection of the key.
- Either collision ends the game as a loss.
- The hazard route is repeatable across runs.
- A blocked or invalid command never advances the hazard.

Verification:

- Run the expanded game-rule test executable with a zero exit status.

Dependencies: Task 3.

Likely files: `game.h`, `game.cpp`, `tests/game_tests.cpp`.

### Checkpoint: Core Rules

- All unit tests pass.
- A reviewed route reaches the key and then unlocks the exit.
- A reviewed route demonstrates a hazard loss.

### 5. Build the interactive terminal loop and renderer

- Render the maze before each turn using legible ASCII symbols.
- Render a concise elapsed-time display and a polished final result message.
- Show key status, movement controls, and meaningful feedback for invalid
  commands and the locked exit.
- Read one line per command, handle EOF safely, and let `q` exit cleanly.
- Print a distinct final message for victory, defeat, and voluntary quit.

Acceptance:

- A player can understand the goal and controls from the initial screen.
- The game never crashes on blank, invalid, or EOF input.
- Terminal output always reflects the current player, key, and game status.
- A victory screen reports the same elapsed time that the game core froze.

Verification:

- Compile and manually play one win, one loss, and one quit session.

Dependencies: Task 4.

Likely files: `main.cpp`, `game.h`, `game.cpp`.

### 6. Add automated end-to-end terminal coverage and quality gates

- Add an assertion-based C++ test executable for pure game rules.
- Add `tests/playthrough_test.sh`, which compiles the game, pipes a known
  winning command sequence to it, and asserts that the victory message occurs.
- Add a second scripted input that verifies the locked-exit message or hazard
  defeat path.
- Update `test_runner.sh` to use `set -euo pipefail`, compile test binaries
  into `build/`, and run all automated checks without prompting for input.
- Compile with `-Wall -Wextra -Wpedantic -Werror`; add a separate
  AddressSanitizer and UndefinedBehaviorSanitizer test pass.

Acceptance:

- Tests run without interactive input.
- A successful playthrough proves the key-to-exit loop works end-to-end.
- An adverse route proves the game does not report a false win.
- Automated checks fail on compiler warnings, memory errors, or undefined
  behavior.

Verification:

- Run `./test_runner.sh` from the repository root.

Dependencies: Task 5.

Likely files: `test_runner.sh`, `tests/game_tests.cpp`,
`tests/playthrough_test.sh`, `tests/README.md`.

### 7. Document and validate the MVP

- Replace the template README content with build, run, controls, objective, and
  test instructions.
- Include the maze symbols and clarify the deterministic hazard behavior.
- Explain the first-successful-move timer rule and the intentionally deferred
  best-time persistence/graphics work.
- Run every validation command below and fix warnings or test failures.

Acceptance:

- A new contributor can build, play, and test the game using README commands.
- No compiler warning is introduced by the chosen warning flags.

Verification:

- Run every command in the Validation Commands section.

Dependencies: Task 6.

Likely files: `README.md`, `tests/README.md`.

### 8. Run final validation

- Execute all unit, integration, build, and manual smoke-test commands.
- Confirm the working tree contains only the intended MVP implementation and
  documentation changes.

Acceptance:

- Every validation command finishes successfully.

Verification:

- Use the complete Validation Commands list below.

Dependencies: Tasks 1–7.

Likely files: no new files required.

## Testing Strategy

### Unit Tests

- Direction parsing for valid and invalid commands.
- Moves in all four directions on open tiles.
- Boundary and wall rejection without state changes.
- Key collection and persistent inventory state.
- Locked-exit rejection and unlocked-exit victory.
- Deterministic hazard step sequence.
- Collision detection when either actor enters the other’s tile.
- Terminal/finished game states reject further movement.
- Timer remains stopped until a legal first move, advances from supplied times,
  and freezes on every terminal status.
- Duration formatting at zero, centisecond rollover, minute rollover, and a
  representative multi-minute completion.

### Integration Tests

- Pipe a known route into the executable and assert its final output contains
  the victory message.
- Pipe an invalid command and `q` to confirm graceful recovery and quitting.
- Pipe a route that reaches a locked exit or hazard and assert it cannot report
  victory.
- Pipe a winning route and assert that the final output includes a completion
  time as well as the victory result.

### Edge Cases

- Blank lines, uppercase movement input (either explicitly supported or
  consistently rejected), unknown characters, and EOF.
- Attempting to quit after a win/loss and moving after a terminal state.
- Key, exit, or hazard positions adjacent to a wall or maze boundary.
- A hazard movement that would enter the player tile.
- Time points equal to the start, a timestamp immediately before a terminal
  event, and elapsed-time queries after the game has finished.

## Acceptance Criteria

- The game compiles with C++17, `-Wall -Wextra -Wpedantic -Werror`, and no
  external dependencies.
- The first screen explains the objective and the `W`, `A`, `S`, `D`, and `Q`
  controls.
- The player cannot cross maze walls or boundaries.
- The player must collect the key before winning through the exit.
- The hazard can cause a clear loss state, and game state stops changing after
  a win or loss.
- Invalid or blocked input does not advance the hazard.
- The timer begins only on a legal first move, remains visible during a run,
  freezes at the terminal state, and displays a final victory time.
- A scripted, non-interactive winning playthrough passes.
- All unit and integration tests pass through `./test_runner.sh`.
- The automated suite passes under AddressSanitizer and UndefinedBehavior
  Sanitizer with no reported errors.
- README instructions accurately build, run, and test the game.

## Validation Commands

Run from the repository root:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror main.cpp game.cpp -o app
./test_runner.sh
printf 'q\n' | ./app
```

After adding the scripted test, `./test_runner.sh` must compile and run the
unit suite and `tests/playthrough_test.sh`. Perform one manual terminal smoke
test with the documented winning route and one with an intentional hazard loss.

## Risks and Mitigations

| Risk | Impact | Mitigation |
| --- | --- | --- |
| A maze route is impossible because of map or hazard timing | High | Prove a fixed winning route in a unit/integration test before polishing output. |
| Timer behavior becomes flaky or unfair | High | Start on the first legal move, use `steady_clock`, inject timestamps into game rules, and never sleep in tests. |
| Input handling blocks or fails in scripts | Medium | Use line-based `std::getline`, handle EOF, and test piped input. |
| Game logic becomes coupled to console output | Medium | Keep rules and state transitions in `Game`; restrict I/O to `main.cpp`. |
| Scope expands into a graphics or multi-level project | Medium | Treat all additions beyond one fixed ASCII maze as post-MVP work. |

## Notes

- Post-MVP candidates: multiple levels, random maze generation, best-time
  persistence, multiple hazard types, colorized output, and a graphical UI.
- No new library is required for the MVP.
- The plan intentionally fixes timing semantics above so the implementation,
  documentation, unit tests, and scripted playthrough all agree.
