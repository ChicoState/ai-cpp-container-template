# Maze Escape

A compact C++17 terminal adventure game. Navigate the maze, collect the key,
avoid the moving hazard, and reach the exit as fast as you can.

## Play

Build with a C++17 compiler:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror main.cpp game.cpp -o build/maze_game
./build/maze_game
```

Controls:

- `W`, `A`, `S`, `D` — move one tile
- `Q` — end the current run

Symbols:

- `P` — player
- `K` — key; collect it to unlock the exit
- `E` — exit
- `H` — moving hazard
- `#` — wall

The timer starts on your first successful movement, remains running while you
play, and freezes when the game ends. The hazard follows a deterministic route,
so faster runs reward learning the maze and timing your path.

## Test

```bash
./test_runner.sh
```

The test runner performs deterministic game-rule tests, a scripted winning
playthrough, and the rule tests under AddressSanitizer and UndefinedBehavior
Sanitizer.

## Project Structure

- `main.cpp` — terminal input/output and game loop
- `game.h`, `game.cpp` — game state, rules, rendering, and timer logic
- `tests/` — unit tests and the scripted terminal playthrough
- `specs/` — MVP specification and implementation plan
