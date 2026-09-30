#!/usr/bin/env bash

set -euo pipefail

g++ -std=c++17 -Wall -Wextra -Werror main.cpp -o app
./tests/calculator_cli_test.sh ./app
