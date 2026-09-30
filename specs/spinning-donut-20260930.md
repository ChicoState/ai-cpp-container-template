# Feature: Terminal Spinning Donut

## Feature Description

Create a self-contained C++ terminal application that renders the classic shaded ASCII torus ("spinning donut") as an ANSI animation. It gives this otherwise empty template a visible, interactive example while staying compatible with the documented `cpp-container` workflow and requiring no dependencies beyond a C++ compiler.

## User Story

As a developer running this repository in a terminal,
I want to launch an animated ASCII donut,
so that I can confirm the project builds and runs through an immediately recognizable terminal experience.

## Problem Statement

`main.cpp` currently exits without producing output, so the repository has no demonstrable application behavior. A terminal animation also needs a bounded execution mode; otherwise automated verification would hang on an intentionally continuous program.

## Solution Statement

Implement the renderer with standard-library C++ only. Keep rendering math and frame generation in a small testable module, and keep terminal I/O, timing, ANSI cursor control, and command-line parsing in the executable. The normal launch loops until the user sends an interrupt; a `--frames N` option renders exactly `N` frames and exits for repeatable automated checks. Do not install a signal handler: standard C++ does not make stream output safe in one, and the app does not hide the cursor or alter terminal modes that need asynchronous restoration.

## Relevant Files

- `main.cpp` — replace the empty entry point with CLI parsing and the animation loop.
- `test_runner.sh` — change the current unbounded run into build-and-test commands that terminate automatically.
- `README.md` — document how to build, run, quit, and use bounded mode.

### New Files

- `donut.hpp` — public, small rendering configuration and frame-generation interface.
- `donut.cpp` — torus sampling, z-buffering, luminance mapping, and ANSI-independent frame construction.
- `tests/donut_tests.cpp` — dependency-free executable tests for renderer invariants.
- `tests/e2e_spinning_donut.sh` — terminal end-to-end smoke test for the bounded CLI path and emitted ANSI output.

## Implementation Plan

### Phase 1: Foundation

Establish a deterministic renderer API with dimensions, rotation angles, and a character palette as explicit inputs. Validate dimensions and frame limits at the CLI boundary so invalid values cannot produce unsafe buffer calculations or endless test runs.

### Phase 2: Core Implementation

Use the standard parametric torus projection with two rotation angles. For each sample, calculate projected screen coordinates, retain the closest depth in a per-frame z-buffer, and map lighting to the ASCII palette. Clear and redraw the terminal with ANSI escape sequences between frames and advance the angles at a stable delay.

### Phase 3: Integration

Make the existing script compile application and test targets separately into a temporary directory, then run unit and end-to-end checks. Document the normal interactive invocation and the deterministic bounded invocation. This prevents generated binaries from appearing as untracked repository files.

## Step by Step Tasks

### 1. Define the renderer boundary

- Add `donut.hpp` and `donut.cpp` with a `RenderConfig` (positive width and height) and a `render_frame` function accepting rotation angles.
- Return a frame containing exactly one newline-terminated row per requested display row, with no ANSI codes in the renderer result.
- Choose the conventional luminance palette `.,-~:;=!*#$@` (or document any equivalent fixed palette) and make a blank background explicit.
- Reject zero, negative, and overflowed dimensions in the renderer rather than allowing size calculations or buffer allocations to proceed.

### 2. Add renderer tests before wiring the loop

- Create `tests/donut_tests.cpp` with a minimal assertion harness; do not add a test framework dependency.
- Test that a valid configuration produces the requested number of rows and columns, has only allowed frame characters/newlines, and contains at least one non-background glyph.
- Test invalid dimensions are rejected and that changing a rotation angle changes a representative rendered frame.

### 3. Build the application loop and bounded CLI

- Replace `main.cpp` with a small CLI supporting `--frames N`, `--delay-ms N`, and `--help`; normal invocation uses an infinite animation loop, while `--frames N` must exit successfully after exactly `N` rendered frames.
- Reject non-numeric values, negative frame counts, negative delays, integer overflow, missing flag values, and unknown options with an error and non-zero exit status. `--frames 0` and `--delay-ms 0` are valid deterministic test modes.
- Emit one ANSI clear-screen sequence before drawing, then exactly one ANSI cursor-home sequence immediately before each frame; flush every frame so the animation is visible in an interactive terminal.
- Sleep only when the configured delay is positive. On finite normal exit, emit a final newline so the prompt remains usable; rely on the terminal's ordinary Ctrl-C handling for the unbounded interactive mode.

### 4. Add an end-to-end terminal smoke test

- Create `tests/e2e_spinning_donut.sh` and make it executable. Resolve its repository root from its own location and accept the already-built executable path through `SPINNING_DONUT_APP` so it does not rely on the caller's directory.
- Run the executable with `--frames 2 --delay-ms 0`, capture stdout in a `mktemp` file with a cleanup trap, and assert status 0, exactly one clear-screen sequence, exactly two cursor-home sequences, and at least one luminance-palette glyph.
- Run `--help`, `--frames 0 --delay-ms 0`, and one malformed or negative option to assert documented usage succeeds, zero-frame mode terminates, and bad input fails promptly.

### 5. Update project commands and documentation

- Update `test_runner.sh` to create a `mktemp -d` build directory with a cleanup trap, compile the app and unit-test executable there without duplicate `main` definitions, then execute unit and E2E tests. It must use `-I.` for the test's `donut.hpp` include, pass the app path to the E2E script as `SPINNING_DONUT_APP`, and never call the unbounded default application mode.
- Update `README.md` with prerequisites, compile/run examples, Ctrl-C behavior, flags, and the test command.
- Keep the change limited to the application, its tests, runner, and documentation; do not introduce a package manager, UI framework, or external dependency.

### 6. Run the validation commands

- Execute every command listed below from a clean working directory state after deleting only generated, ignored binaries if necessary.
- Confirm the normal animation manually in an interactive terminal and confirm automated verification does not hang.

## Testing Strategy

### Unit Tests

- Frame geometry matches configured dimensions.
- Renderer output uses only background, luminance-palette glyphs, and line endings.
- A baseline frame contains visible torus content.
- Invalid renderer configuration fails predictably.
- Different rotation inputs produce different frame content without changing the frame shape.

### Edge Cases

- `--frames 0` exits cleanly without attempting an unbounded loop.
- Very small but valid display dimensions do not access buffers out of range.
- `--delay-ms 0` supports fast test execution and is explicitly accepted by the CLI.
- Unknown flags, missing flag values, malformed integers, and negative values return a non-zero status.
- Output redirected to a file still contains the frame and expected ANSI controls; full terminal capability detection is intentionally out of scope.

## Acceptance Criteria

- Running `./app` in an ANSI-capable terminal displays a continuously rotating shaded ASCII torus until interrupted with Ctrl-C.
- `./app --frames 2 --delay-ms 0` exits with status 0 after producing two animation frames.
- The application uses only the C++ standard library and the compiler already assumed by the repository.
- Unit tests and the terminal E2E script pass without manual interaction or timeouts.
- Documentation accurately describes run, quit, flags, and test commands.

## Validation Commands

Run these after implementation:

```bash
build_dir="$(mktemp -d)"; trap 'rm -rf "$build_dir"' EXIT
g++ -std=c++17 -Wall -Wextra -Wpedantic main.cpp donut.cpp -o "$build_dir/app"
"$build_dir/app" --help
"$build_dir/app" --frames 2 --delay-ms 0 > /tmp/spinning-donut.out
g++ -std=c++17 -Wall -Wextra -Wpedantic -I. tests/donut_tests.cpp donut.cpp -o "$build_dir/donut_tests"
"$build_dir/donut_tests"
SPINNING_DONUT_APP="$build_dir/app" bash tests/e2e_spinning_donut.sh
bash test_runner.sh
```

For the manual check, run `./app` inside the documented container or a local ANSI-capable terminal, observe smooth frame updates, then press Ctrl-C and verify the prompt resumes on a new line.

## Notes

- This plan deliberately does not add terminal-size detection, color output, resize handling, signal handlers, or Windows console compatibility; they are sensible follow-up enhancements but not needed for the requested app.
- The default target is C++17 for `std::chrono` and `std::thread`; if the container defaults to a newer standard, the implementation may retain C++17-compatible code.
