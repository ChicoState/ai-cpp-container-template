#!/bin/bash

set -euo pipefail

mkdir -p build

compile_flags=(-std=c++17 -Wall -Wextra -Wpedantic -Werror)

g++ "${compile_flags[@]}" tests/game_tests.cpp game.cpp -o build/game_tests
build/game_tests

bash tests/playthrough_test.sh

g++ "${compile_flags[@]}" -fsanitize=address,undefined \
  tests/game_tests.cpp game.cpp -o build/game_tests_sanitized
build/game_tests_sanitized
