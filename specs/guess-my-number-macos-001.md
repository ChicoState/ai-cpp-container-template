# Feature: Guess My Number terminal game

## Objective

Implement a macOS C++ terminal game for one player. Each round chooses a random integer from 0 through 100; the player guesses until correct, receiving higher/lower feedback. The game supports replay and saves the best (fewest valid guesses) across launches in `high_score.txt` beside the executable.

## Intended Users

A person running the repository's C++ application locally on macOS who wants a simple terminal number-guessing game with an enduring personal best score.

## Measurable Success Criteria

- The game selects only targets in the inclusive range 0 through 100.
- Only whole-number guesses in that range count as attempts.
- Leading and trailing ASCII whitespace and an optional leading `+` are accepted; malformed and out-of-range input is rejected without increasing the attempt count.
- A correct guess reports the valid-guess count and offers replay.
- The lowest valid-guess count from a completed round persists between application launches.
- A missing score file is created after a completed game; malformed or unreadable score files produce a warning and are never overwritten.
- EOF during guessing or at the replay prompt exits cleanly.

## Current Tech Stack and Commands

- Language: C++
- Compiler already used by the repository: `g++`
- Build/run script: Bash, `test_runner.sh`
- Dependencies: none; use only the C++17 standard library and macOS system headers already available to the compiler.
- Package manager, test framework, linter, development server, CI, and deployment configuration: none found.

Current baseline build-and-run command:

```bash
./test_runner.sh
```

Direct development command after implementation:

```bash
g++ -std=c++17 main.cpp guess_my_number.cpp -o app && ./app
```

## Project Structure and Affected Files

```text
main.cpp                       Application entry point; currently empty.
test_runner.sh                 Compiles every root-level C++ source and runs `app`.
tests/                         Test folder; currently contains only README.md.
specs/                         Implementation specifications.
guess_my_number.hpp            New declarations/types for reusable game logic.
guess_my_number.cpp            New parsing, scoring, persistence, and game-loop logic.
tests/guess_my_number_test.cpp New unit and persistence test executable.
tests/guess_my_number_e2e.sh   New scripted terminal/process-level test.
.gitignore                     New only if approved during implementation; ignore runtime score file.
```

The runtime-created `high_score.txt` belongs beside the compiled executable and must not be committed. Preserve existing unrelated modifications and untracked files.

## Code Style

The only repository source example is a minimal native C++ entry point:

```cpp
int main() {
  return 0;
}
```

Follow this style with small, descriptive C++ functions and standard library facilities. Use C++17, avoid `using namespace std`, keep parsing and persistence separate from terminal I/O, and propagate errors through explicit return values/statuses and user-facing warning text.

## Public Behavior and Data Flow

The user-visible interfaces are stdin, stdout/stderr, and the plaintext `high_score.txt` file. No network, schema, API, authentication, or migration work is involved.

`main` seeds a `std::mt19937` once, supplies 0--100 targets through an internal `TargetProvider`, resolves the score path, and calls an interaction seam such as:

```cpp
int runGame(std::istream& input, std::ostream& output,
            TargetProvider targetProvider,
            const std::filesystem::path& scorePath);
```

The reusable logic must include:

- Parsing that trims surrounding ASCII whitespace, permits one leading `+` or `-`, requires all remaining characters to be digits, parses safely, and then enforces the 0--100 range.
- Guess evaluation returning too-low, too-high, or correct.
- A structured score-file result, rather than `optional<int>` alone:

```cpp
enum class HighScoreStatus { missing, valid, malformed, unreadable };
```

`missing` permits saving the first completed score. `valid` permits writing only a strictly lower score. `malformed` and `unreadable` produce a warning and disable all score-file writes for the invocation; this preserves a bad/unavailable file rather than silently replacing it.

On macOS, resolve the executable with `_NSGetExecutablePath` from `<mach-o/dyld.h>`, correctly resize its buffer as needed, and use the executable's parent directory for `high_score.txt`. If resolution fails, warn and use the current working directory as a documented fallback.

## Failure Modes and Risks

- Seed the random engine once, rather than per round, to avoid repetitive targets.
- Do not let invalid input wedge the input stream or count as an attempt.
- Treat EOF while guessing or choosing replay as a normal exit.
- A missing score is not an error; a malformed/unreadable score is a nonfatal warning that disables score writes.
- A score write failure is nonfatal: report it without losing the completed-round result or crashing.
- Resolve score-file placement independently from the process working directory; a process launched via another directory must still use the executable directory.
- Do not depend on the existing untracked Linux-only `app` binary when validating locally.

## Technical Plan

### Components and Dependencies

1. **Reusable game logic** (`guess_my_number.hpp`, `guess_my_number.cpp`) defines input parsing, guess evaluation, structured score loading/saving, and the stream-based `runGame` interaction seam. It has no dependency on macOS APIs and is the foundation for tests and the executable.
2. **Application entry point** (`main.cpp`) depends on the reusable logic. It supplies the random target provider and uses the macOS-only executable-path API to choose the runtime score-file path.
3. **Unit/persistence test executable** (`tests/guess_my_number_test.cpp`) depends on the reusable logic but not `main.cpp`, allowing deterministic checks without a second program entry point.
4. **E2E shell test** (`tests/guess_my_number_e2e.sh`) depends on both the reusable logic and app entry point. It verifies terminal transcripts and the executable-relative storage contract in temporary directories.

### Implementation Sequence

The reusable logic and its tests must be completed before `main.cpp`, because the entry point is intentionally thin. The E2E script follows the entry-point work because it launches the compiled program. `.gitignore` is independent but remains approval-gated and can be added only after that approval.

The unit test work and header declarations can proceed together. The E2E work is sequential after the application entry point; it should not be started against probabilistic behavior because the compile-time test target seam provides deterministic input.

### Verification Checkpoints

1. **Logic checkpoint:** the unit test executable compiles and verifies parsing, comparison, and score-state transitions.
2. **Application checkpoint:** the standard `test_runner.sh` build succeeds and the executable runs interactively.
3. **Behavior checkpoint:** the E2E script verifies replay, EOF, persistence, malformed-file preservation, and executable-relative storage.
4. **Handoff checkpoint:** all validation commands pass with no whitespace errors.

## Ordered Implementation Tasks

1. Before adding `.gitignore`, obtain approval to create it and add only `high_score.txt`; preserve all existing worktree changes.
2. Add `guess_my_number.hpp` plus `tests/guess_my_number_test.cpp` with failing tests for parsing, comparisons, high-score statuses, save/update rules, and file failures.
3. Implement `guess_my_number.cpp` with the C++17 parsing, comparison, high-score load/save, and `runGame` seam. Ensure malformed and unreadable statuses prevent writes.
4. Update `main.cpp` to create the seeded random target provider, calculate the macOS executable-relative score path, run the terminal game loop, offer replay, and handle EOF.
5. Add `tests/guess_my_number_e2e.sh`.
   - Exercise deterministic transcripts, invalid inputs, replay, and both EOF paths through `runGame`.
   - Exercise malformed-score preservation and failed-write warnings through temporary paths.
   - Compile and launch the actual app from a different working directory; verify that `high_score.txt` is created beside the temporary binary rather than in the launch directory, then verify persistence on a second process launch.
6. Allow deterministic targets only in a test build using `-DGUESS_MY_NUMBER_TESTING`; normal production builds must expose no test command-line option or test environment behavior.
7. Execute all validation commands and correct failures without deleting tests.

## Implementation Task Checklist

- [ ] **Task: Define the reusable game interface and failing unit tests.**
  - Acceptance: the header declares the stream-based game seam, target provider, parsing/evaluation interfaces, and structured high-score result; tests cover the specified valid and invalid values plus all score statuses.
  - Verify: `g++ -std=c++17 -Wall -Wextra -pedantic tests/guess_my_number_test.cpp guess_my_number.cpp -o /private/tmp/guess_my_number_test && /private/tmp/guess_my_number_test`
  - Files: `guess_my_number.hpp`, `tests/guess_my_number_test.cpp`

- [ ] **Task: Implement parsing, evaluation, and persistence.**
  - Acceptance: parser and comparison behavior meet the stated bounds/input policy; missing/valid/malformed/unreadable states are distinguishable; malformed/unreadable states never trigger a write.
  - Verify: run the unit test executable from the prior task.
  - Files: `guess_my_number.cpp`, `guess_my_number.hpp`, `tests/guess_my_number_test.cpp`

- [ ] **Task: Implement the macOS terminal entry point.**
  - Acceptance: one seeded random engine supplies every round; valid attempts are counted; replay and both EOF paths exit correctly; `_NSGetExecutablePath` determines the score directory with documented fallback behavior.
  - Verify: `./test_runner.sh`, followed by a manual terminal round.
  - Files: `main.cpp`, `guess_my_number.cpp`, `guess_my_number.hpp`

- [ ] **Task: Add deterministic end-to-end coverage.**
  - Acceptance: scripted tests prove feedback, invalid-input handling, replay, EOF, persistence, failed write warnings, malformed-file preservation, and macOS executable-relative score storage from a different working directory.
  - Verify: `bash tests/guess_my_number_e2e.sh`
  - Files: `tests/guess_my_number_e2e.sh`, `tests/guess_my_number_test.cpp`, `guess_my_number.cpp`, `guess_my_number.hpp`, `main.cpp`

- [ ] **Task: Ignore runtime score data after explicit approval.**
  - Acceptance: only `high_score.txt` is added to ignore rules; no existing user file is removed or altered.
  - Verify: `git status --short` confirms the runtime score file is not newly listed after a test run.
  - Files: `.gitignore`

- [ ] **Task: Complete validation and handoff.**
  - Acceptance: all build, unit, E2E, and whitespace checks pass without deleting or weakening tests.
  - Verify: run every command in [Validation Commands](#validation-commands).
  - Files: implementation and test files above, only if corrections are required.

## Testing Strategy

### Unit Tests

- Parse `0`, `100`, surrounding whitespace, and an optional `+` successfully.
- Reject blank, partial-number, nonnumeric, negative, `-1`, and `101` inputs.
- Verify too-low, too-high, and correct evaluations.
- Verify missing score, valid score, malformed score, and unreadable score outcomes.
- Verify first-score saving, strictly better-score replacement, and equal/worse-score preservation.

### Integration and E2E Tests

- Script terminal input with invalid values before a correct guess and verify invalid attempts do not count.
- Verify replay begins another round and user exit ends normally.
- Verify EOF during a guess and during replay ends normally.
- Verify cross-process persistence of the best score.
- Verify malformed score files remain byte-for-byte unmodified and produce warnings.
- Verify a deliberately invalid/non-writable test path emits a nonfatal warning.
- Verify actual `main` uses the macOS executable directory when the process starts in another directory.

## Validation Commands

```bash
./test_runner.sh
g++ -std=c++17 -Wall -Wextra -pedantic \
  tests/guess_my_number_test.cpp guess_my_number.cpp \
  -o /private/tmp/guess_my_number_test && /private/tmp/guess_my_number_test
bash tests/guess_my_number_e2e.sh
git diff --check
```

## Boundaries

- **Always:** use C++17 standard-library logic, preserve user changes, validate all input and failure paths, and run the listed checks.
- **Ask first:** add dependencies; alter score storage format or location; create/modify CI; alter security or data-retention behavior; or make destructive changes.
- **Never:** overwrite malformed scores; add production test controls; commit runtime data or secrets; delete tests to make checks pass; or change unrelated files.

## Approvals and Compatibility

No dependency, schema, CI, deployment, security, migration, or public API change is planned. Creating `.gitignore` is explicitly approval-gated. This feature supports macOS only because it uses the macOS executable-path API; Linux portability is out of scope.
