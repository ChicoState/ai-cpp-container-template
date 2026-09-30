# Spec: Codequest MVP

## Objective

Create a small, deterministic, text-based C++ console adventure called
**Codequest**. A single player explores a compact world, collects two clues,
solves one puzzle, and reaches a final goal.

The game is successful when a player can complete the following route through
terminal input without external dependencies:

1. Begin at Camp.
2. Visit Archive and collect the Star Chart.
3. Visit Workshop and collect the Decoder Wheel.
4. Visit Observatory and answer the final puzzle with `ORION`.
5. See a completion message and exit cleanly.

The intended user is someone running the program in an interactive terminal.

## Game Contract

### Locations and progression

| Location | Purpose | Result |
| --- | --- | --- |
| Camp | Starting hub | Choose a destination or use a global command. |
| Archive | First clue location | Collect the Star Chart once. |
| Workshop | Second clue location | Collect the Decoder Wheel once. |
| Observatory | Final location | Requires both items before the puzzle can be solved. |

The Archive and Workshop can be visited in either order. Revisiting either
location must report that its item was already collected and must not create a
duplicate item.

### Menus, clues, and puzzle text

The exact numbered actions are:

| Location | Actions |
| --- | --- |
| Camp | `1` Archive; `2` Workshop; `3` Observatory |
| Archive | `1` Search the shelves; `2` Return to Camp |
| Workshop | `1` Search the workbench; `2` Return to Camp |
| Observatory without both items | `1` Return to Camp |
| Observatory with both items | `1` Attempt the riddle; `2` Return to Camp |

The first Archive search awards the Star Chart and displays: "The chart shows a
constellation named after a famous hunter. Its name begins with O." The first
Workshop search awards the Decoder Wheel and displays: "The decoder reveals
the Hunter's constellation has five letters and ends with N."

When the player attempts the Observatory riddle, display this prompt:

> What is the name of the Hunter constellation? It starts with O, has five
> letters, and ends with N.

The required answer is `ORION`.

At the Observatory, the player cannot solve the puzzle until both items have
been collected. Once both are present, the game presents a final riddle whose
answer is `ORION`. Answer comparison ignores leading/trailing whitespace and
ASCII letter case. A correct answer prints the victory text, marks the game as
won, and ends the game loop. An incorrect answer provides feedback and leaves
the session active.

### Commands and input

Each location presents a numbered menu appropriate to that location. Every
menu also accepts these global text commands:

- `help` — display available commands.
- `inventory` — display collected items.
- `quit` — end the game cleanly.

Input is read as complete lines. Blank or unrecognized input must print short,
non-technical feedback, preserve the current game state, and return to the
current menu. End-of-file is treated as a normal exit, not an error.

## Tech Stack

- C++17
- C++ standard library only
- A POSIX-like shell for the existing `test_runner.sh`
- No third-party packages, test frameworks, build systems, persistence, GUI,
  combat, timers, randomness, or networking

## Commands

After implementation, the documented commands are:

```bash
# Build the interactive game.
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp game.cpp -o app

# Run the interactive game.
./app

# Build and run automated tests without starting an interactive session.
./test_runner.sh
```

`test_runner.sh` must compile the application and the tests as separate
executables. It must run the test executable only; it must not launch `./app`
without scripted input.

## Project Structure

```text
main.cpp                 application entry point only
game.hpp                 Game public interface and small domain enums
game.cpp                 game loop, rules, input handling, and text output
tests/game_tests.cpp     standalone assertion-based automated tests
test_runner.sh           non-interactive build-and-test script
specs/codequest-mvp.md   this MVP source-of-truth specification
README.md                contributor build, run, test, and player guidance
```

The existing `specs/README.md` and `tests/README.md` are not required to
change for this MVP unless their content later becomes misleading.

## Architecture and Code Style

Use one `Game` class and two small enums. Keep all game rules and mutable game
state inside `Game`; `main.cpp` only wires standard input and output to it.

```cpp
enum class Location { Camp, Archive, Workshop, Observatory };
enum class Item { StarChart, DecoderWheel };

int main() {
    Game game(std::cin, std::cout);
    game.run();
    return 0;
}
```

`Game` accepts `std::istream&` and `std::ostream&` in its constructor so the
same game loop works interactively and with string streams in tests. It owns:

- the current `Location`;
- two clearly named item flags;
- running and won state;
- menu rendering, command dispatch, navigation, item collection, inventory,
  and final-puzzle behavior.

Use `std::getline` for all player input. A single private normalization helper
must trim surrounding whitespace and lowercase ASCII letters before command or
puzzle comparisons. Prefer direct, descriptive functions such as
`visitArchive`, `visitWorkshop`, `visitObservatory`, `showInventory`, and
`tryPuzzle` over a hierarchy of location classes, callback maps, or command
objects.

Use conventional C++ formatting, descriptive `camelCase` function names,
trailing underscores for private data members, and no global mutable state.

## Testing Strategy

Use a small, dependency-free assertion-based executable in
`tests/game_tests.cpp`. It has its own `main()` and is compiled separately
from the interactive application entry point.

Tests use `std::istringstream` for scripted commands and
`std::ostringstream` for deterministic output checks. They must cover:

- the complete successful route;
- either order of item collection;
- one-time collection and inventory output;
- Observatory prerequisite messages with zero or one item;
- incorrect-answer retry behavior;
- case- and whitespace-insensitive `ORION` acceptance;
- blank and invalid command feedback without state corruption;
- `quit` and end-of-file termination without a hang.

The test runner compiles with `-std=c++17 -Wall -Wextra -pedantic` and builds
the game and test executable separately, preventing duplicate `main`
definitions. Tests assert player-visible behavior instead of exposing mutable
state solely for testing.

## Boundaries

### Always

- Use C++17 and the standard library only.
- Validate every player input line and handle EOF.
- Keep game rules in `Game` and the entry point thin.
- Run `./test_runner.sh` before considering the MVP complete.
- Keep gameplay deterministic and output suitable for scripted tests.

### Ask first

- Add a dependency, test framework, build system, or CI configuration.
- Add locations, items, additional puzzles, scoring, persistence, or gameplay
  systems beyond this specification.
- Change the fixed final answer or the stated game progression.

### Never

- Start the game interactively from the test runner without scripted input.
- Parse normal player mistakes through exception-driven numeric conversion.
- Remove or weaken tests to make a failing implementation appear correct.
- Commit secrets, generated binaries, or unrelated repository changes.

## Success Criteria

- [ ] The program builds with the documented C++17 command and warning flags.
- [ ] A player can collect the Star Chart and Decoder Wheel in either order.
- [ ] Items cannot be duplicated by revisiting their locations.
- [ ] The Observatory cannot be won without both required items.
- [ ] `ORION`, regardless of ASCII case and surrounding whitespace, wins once
      both items have been collected.
- [ ] Invalid input, `quit`, and EOF are handled without a crash or infinite
      prompt loop.
- [ ] Automated tests cover the specified game flow and edge cases.
- [ ] `./test_runner.sh` completes without requiring interactive input.

## Open Questions

None for the MVP.
