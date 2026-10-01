# Tests

- `game_tests.cpp` checks game rules, movement, key/exit states, hazard
  collisions, and deterministic timer behavior.
- `playthrough_test.sh` compiles the terminal program and verifies a scripted
  winning run prints both victory and a final completion time.

Run every check from the repository root with:

```bash
./test_runner.sh
```
