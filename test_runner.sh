#!/bin/bash

set -euo pipefail

build_dir="$(mktemp -d "${TMPDIR:-/tmp}/codequest-tests.XXXXXX")"
trap 'rm -rf "$build_dir"' EXIT

g++ -std=c++17 -Wall -Wextra -pedantic main.cpp game.cpp -o "$build_dir/app"
g++ -std=c++17 -Wall -Wextra -pedantic -I. tests/game_tests.cpp game.cpp \
    -o "$build_dir/game_tests"

"$build_dir/game_tests"
