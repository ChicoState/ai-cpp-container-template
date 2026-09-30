#!/usr/bin/env bash

set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir"

mkdir -p build

compiler="${CXX:-g++}"
flags=(-std=c++17 -Wall -Wextra -Wpedantic -I.)

"$compiler" "${flags[@]}" main.cpp rps_game.cpp -o build/app
"$compiler" "${flags[@]}" rps_game.cpp tests/rps_game_test.cpp \
  -o build/rps_game_test
"$compiler" "${flags[@]}" rps_game.cpp tests/rps_cli_test.cpp \
  -o build/rps_cli_test

build/rps_game_test
build/rps_cli_test
printf '' | build/app > /dev/null

echo "All tests passed."
