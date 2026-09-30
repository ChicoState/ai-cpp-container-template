# Spec: Terminal Spinning Donut

## Objective

Build a dependency-free C++ terminal program that displays a continuously rotating, shaded ASCII torus in an ANSI-capable terminal. It makes the repository demonstrably runnable while providing a deterministic, finite execution mode for tests.

### User story

As a developer running this repository, I can launch a spinning ASCII donut and stop it with Ctrl-C. As an automated test, I can request a fixed number of frames and receive a successful, bounded result.

### Success definition

- The default invocation displays successive animated torus frames until the terminal interrupts it.
- `--frames 2 --delay-ms 0` produces exactly two frames and exits with status 0.
- No third-party libraries, package managers, or framework changes are introduced.
- Automated unit and terminal smoke tests finish without manual input or timeouts.

## Tech Stack

- Language: C++17.
- Compiler: `g++`, already used by `test_runner.sh`.
- Runtime: ANSI-capable terminal, using text output and ANSI clear/cursor-home sequences only.
- Dependencies: C++ standard library only (`<chrono>`, `<thread>`, streams, containers, math, and exceptions as needed).

## Commands

The following are the intended post-implementation commands. They build into a disposable temporary directory so generated executables do not appear as untracked repository files.

```bash
build_dir="$(mktemp -d)"; trap 'rm -rf "$build_dir"' EXIT
g++ -std=c++17 -Wall -Wextra -Wpedantic main.cpp donut.cpp -o "$build_dir/app"
"$build_dir/app" --help
"$build_dir/app" --frames 2 --delay-ms 0
g++ -std=c++17 -Wall -Wextra -Wpedantic -I. tests/donut_tests.cpp donut.cpp -o "$build_dir/donut_tests"
"$build_dir/donut_tests"
SPINNING_DONUT_APP="$build_dir/app" bash tests/e2e_spinning_donut.sh
bash test_runner.sh
```

Interactive run after building:

```bash
"$build_dir/app"
```

Press Ctrl-C to return to the prompt. No application-level signal handler is required because the app does not hide the cursor or change terminal modes.

## Project Structure

```text
main.cpp                       -> CLI parsing, animation loop, terminal output
donut.hpp                      -> RenderConfig and renderer interface
donut.cpp                      -> Torus projection, depth buffer, shading, plain-text frame generation
tests/donut_tests.cpp          -> Dependency-free renderer unit tests
tests/e2e_spinning_donut.sh    -> Bounded CLI and ANSI-output smoke test
test_runner.sh                 -> Temporary build and full automated test entry point
README.md                      -> Build, run, options, and test documentation
specs/                         -> Feature plan and this specification
```

## Code Style

- Use C++17 and four-space indentation.
- Use `snake_case` for functions and variables, `PascalCase` for types, and `constexpr` for fixed rendering constants.
- Keep all terminal escape sequences and timing in `main.cpp`; renderer output must be plain text so unit tests do not need a terminal.
- Prefer direct data flow and value types over global mutable state.
- Validate externally supplied numeric values before narrowing or using them in allocation/indexing calculations.

```cpp
RenderConfig config{80, 22};
const std::string frame = render_frame(config, angle_a, angle_b);

std::cout << "\x1b[H" << frame << std::flush;
```

## Functional Requirements

### Renderer

- `render_frame` accepts a valid positive-width, positive-height configuration and two rotation angles.
- It returns exactly the requested number of newline-terminated rows, with the requested display width before each newline.
- It uses a per-frame depth buffer so nearer torus samples occlude farther samples.
- It uses a fixed ASCII luminance palette such as `.,-~:;=!*#$@`; blank cells are spaces.
- It rejects invalid or overflowed dimensions before allocation or indexing.

### CLI and terminal behavior

- Supported flags are `--help`, `--frames N`, and `--delay-ms N`.
- With no `--frames`, the program loops until the terminal interrupts it.
- `--frames 0` exits successfully without rendering a frame; a positive value renders exactly that many frames.
- `--delay-ms 0` is valid and skips sleeping; a positive value sleeps between frames.
- Missing values, malformed values, negative values, integer overflow, and unknown options emit an error and return non-zero.
- The program emits one clear-screen sequence, then a cursor-home sequence before each rendered frame, and flushes each frame.
- Finite normal exit emits a final newline. The program must not install a signal handler that writes through C++ streams.

## Testing Strategy

### Unit tests: `tests/donut_tests.cpp`

- A valid frame has the configured row count, column count, valid palette characters, spaces, and newlines only.
- A baseline configuration contains at least one non-space glyph.
- Invalid dimensions fail predictably.
- Different rotation inputs change representative frame content but not its dimensions.

### End-to-end test: `tests/e2e_spinning_donut.sh`

- Resolve the repository root from the script location.
- Receive the executable path through `SPINNING_DONUT_APP`.
- Use `mktemp` plus a cleanup trap for captured output.
- Assert that `--frames 2 --delay-ms 0` succeeds and emits exactly one clear-screen sequence, two cursor-home sequences, and visible luminance glyphs.
- Assert that `--help` and zero-frame mode succeed, and at least one malformed or negative option fails.

### Test constraints

- `test_runner.sh` compiles application and unit-test targets separately in a `mktemp -d` directory, with `-I.` for the test header include.
- Tests must never invoke the unbounded default mode.
- Tests are deterministic and require no installed test framework.

## Boundaries

- Always: compile with `-std=c++17 -Wall -Wextra -Wpedantic`; run unit, E2E, and `test_runner.sh` verification; validate all CLI numbers; keep generated binaries temporary.
- Ask first: add a dependency; change the C++ standard; add terminal-size, color, resize, or Windows-console support; change CI or container configuration.
- Never: commit generated binaries or secrets; use a signal handler for C++ stream output; make automated checks depend on manual Ctrl-C; remove failing tests to get a passing run.

## Success Criteria

- Default launch visibly animates a shaded ASCII torus in an ANSI-capable terminal.
- Bounded launch with `--frames 2 --delay-ms 0` exits successfully and is verifiably two frames.
- Unit tests cover renderer shape, glyph validity, visible content, invalid configuration, and rotation change.
- E2E tests cover bounded animation output, help, zero frames, and invalid CLI input.
- `bash test_runner.sh` completes successfully without invoking an endless loop.
- README accurately documents building, running, stopping, flags, and testing.

## Open Questions

- None for the planned minimal terminal implementation. Terminal-size detection, color, resize handling, and Windows console support are intentionally excluded and require a separate scope decision.

## Approval Gate

This specification documents the reviewed plan. Implementation must not begin until the human approves this specification and the associated task plan.
