#!/bin/sh

set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
temp_directory=$(mktemp -d "${TMPDIR:-/tmp}/guess-my-number-e2e.XXXXXX")
trap 'rm -rf "$temp_directory"' EXIT

binary_directory="$temp_directory/bin"
working_directory="$temp_directory/work"
mkdir -p "$binary_directory" "$working_directory"

g++ -std=c++17 -Wall -Wextra -pedantic -DGUESS_MY_NUMBER_TESTING \
  "$repo_root/main.cpp" "$repo_root/guess_my_number.cpp" \
  -o "$binary_directory/guess-my-number"

first_output=$(cd "$working_directory" && \
  GUESS_MY_NUMBER_TARGET=42 "$binary_directory/guess-my-number" <<'EOF'
oops
101
40
42
n
EOF
)

printf '%s\n' "$first_output" | grep -q "Invalid guess"
printf '%s\n' "$first_output" | grep -q "Higher"
printf '%s\n' "$first_output" | grep -q "2 valid guesses"
test -f "$binary_directory/high_score.txt"
test ! -e "$working_directory/high_score.txt"
test "$(tr -d '[:space:]' < "$binary_directory/high_score.txt")" = "2"

second_output=$(cd "$temp_directory" && \
  GUESS_MY_NUMBER_TARGET=42 "$binary_directory/guess-my-number" <<'EOF'
42
n
EOF
)

printf '%s\n' "$second_output" | grep -q "High score: 2 valid guesses"
test "$(tr -d '[:space:]' < "$binary_directory/high_score.txt")" = "1"

third_output=$(cd "$working_directory" && \
  GUESS_MY_NUMBER_TARGET=42 "$binary_directory/guess-my-number" <<'EOF'
40
42
n
EOF
)

printf '%s\n' "$third_output" | grep -q "High score: 1 valid guesses"
test "$(tr -d '[:space:]' < "$binary_directory/high_score.txt")" = "1"

printf 'not-a-score\n' > "$binary_directory/high_score.txt"
malformed_output=$(cd "$working_directory" && \
  GUESS_MY_NUMBER_TARGET=42 "$binary_directory/guess-my-number" <<'EOF'
42
n
EOF
)

printf '%s\n' "$malformed_output" | grep -q "Warning: high score file is malformed"
test "$(cat "$binary_directory/high_score.txt")" = "not-a-score"

echo "Guess My Number E2E tests passed"
