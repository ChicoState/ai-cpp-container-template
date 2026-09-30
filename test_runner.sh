#!/bin/bash

set -euo pipefail

g++ -std=c++17 -Wall -Wextra -pedantic main.cpp blackjack.cpp -o app
g++ -std=c++17 -Wall -Wextra -pedantic tests/blackjack_tests.cpp blackjack.cpp \
  -o blackjack_tests
./blackjack_tests
