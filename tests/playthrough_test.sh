#!/bin/bash

set -euo pipefail

mkdir -p build
g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror main.cpp game.cpp \
  -o build/maze_game

printf 'd\nd\nd\nd\nd\nd\ns\ns\ns\ns\ns\n' | build/maze_game \
  > build/winning-playthrough.txt

grep -q "You escaped the maze!" build/winning-playthrough.txt
grep -Eq "Final time: [0-9]{2}:[0-9]{2}\.[0-9]{2}" \
  build/winning-playthrough.txt
