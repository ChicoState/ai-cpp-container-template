#!/usr/bin/env bash

set -euo pipefail

calculator="${1:?usage: calculator_cli_test.sh <calculator-path>}"

actual_output="$(printf '%s\n' \
  'add 2 3' \
  'add -2 3' \
  'sub 9 4' \
  'sub 2 5' \
  '' \
  'multiply 2 3' \
  'add 2' \
  'sub x 3' \
  'add 1 2 3' \
  'exit now' \
  'add 10 5' \
  'exit' | "$calculator")"

expected_output="$(cat <<'EOF'
5
1
5
-3
Error: expected 'add <integer> <integer>', 'sub <integer> <integer>', or 'exit'.
Error: expected 'add <integer> <integer>', 'sub <integer> <integer>', or 'exit'.
Error: expected 'add <integer> <integer>', 'sub <integer> <integer>', or 'exit'.
Error: expected 'add <integer> <integer>', 'sub <integer> <integer>', or 'exit'.
Error: expected 'add <integer> <integer>', 'sub <integer> <integer>', or 'exit'.
Error: expected 'add <integer> <integer>', 'sub <integer> <integer>', or 'exit'.
15
EOF
)"

if [[ "$actual_output" != "$expected_output" ]]; then
  printf '%s\n' 'Calculator CLI output did not match the expected transcript.' >&2
  printf '%s\n' 'Expected:' >&2
  printf '%s\n' "$expected_output" >&2
  printf '%s\n' 'Actual:' >&2
  printf '%s\n' "$actual_output" >&2
  exit 1
fi

printf '%s\n' 'Calculator CLI tests passed.'
