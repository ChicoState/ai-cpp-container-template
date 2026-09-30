#!/usr/bin/env bash

set -uo pipefail

project_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
test_directory="$(mktemp -d)"
calculator="$test_directory/calculator"
trap 'rm -rf "$test_directory"' EXIT

g++ "$project_root/main.cpp" -o "$calculator"

failures=0

expect_success() {
  local input="$1"
  local expected_output="$2"
  local output

  output="$(printf '%s\n' "$input" | "$calculator" 2>&1)"
  local status=$?

  if [[ $status -ne 0 || "$output" != "$expected_output" ]]; then
    printf 'FAIL: %s\nExpected success output: %s\nActual status/output: %s / %s\n' \
      "$input" "$expected_output" "$status" "$output" >&2
    failures=$((failures + 1))
  fi
}

expect_failure() {
  local input="$1"
  local output

  output="$(printf '%s\n' "$input" | "$calculator" 2>&1)"
  local status=$?

  if [[ $status -eq 0 || -z "$output" ]]; then
    printf 'FAIL: %s\nExpected an error and non-zero status; got %s / %s\n' \
      "$input" "$status" "$output" >&2
    failures=$((failures + 1))
  fi
}

expect_success '8 + 2' '10'
expect_success '8 - 2' '6'
expect_success '8 * 2' '16'
expect_success '8 / 2' '4'
expect_success '5 / 2' '2.5'
expect_success '-5 * 2' '-10'
expect_failure '8 / 0'
expect_failure '8 % 2'
expect_failure '12abc + 2'
expect_failure 'nan + 2'
expect_failure '8 + 2 extra'

if "$calculator" </dev/null >/dev/null 2>&1; then
  printf 'FAIL: EOF should return a non-zero status\n' >&2
  failures=$((failures + 1))
fi

if [[ $failures -ne 0 ]]; then
  exit 1
fi

printf 'All calculator tests passed.\n'
