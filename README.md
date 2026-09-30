# Codequest

Codequest is a small, deterministic text adventure written in C++17. Explore
the Camp, Archive, Workshop, and Observatory; collect two clues; then solve the
final riddle to open the vault.

## Build and run

```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp game.cpp -o app
./app
```

## Play

Start at Camp and use the numbered menu choices to travel. The Archive contains
the Star Chart and the Workshop contains the Decoder Wheel. Once both are in
your inventory, visit the Observatory and solve its riddle.

At every location, these commands are available:

- `help` — show command guidance.
- `inventory` — show collected clues.
- `quit` — exit the game.

Input is case-insensitive for commands and the final answer. Whitespace around
input is ignored.

## Test

```bash
./test_runner.sh
```

The test runner builds the game and a separate assertion-based test executable,
then runs the tests without opening an interactive session.

## Project structure

- `main.cpp` — application entry point.
- `game.hpp`, `game.cpp` — game state, menus, input handling, and rules.
- `tests/game_tests.cpp` — scripted gameplay tests.
- `specs/codequest-mvp.md` — approved MVP specification.
