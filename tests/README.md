This folder contains automated tests for the terminal game.

- `rps_game_test.cpp` verifies parsing, round rules, score updates, and ASCII
  card layout.
- `rps_cli_test.cpp` uses deterministic computer moves to verify the complete
  terminal flow, including replay and end-of-file handling.

Run both through `./test_runner.sh` from the repository root.
