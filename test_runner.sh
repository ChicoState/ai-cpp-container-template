#!/usr/bin/env bash

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
build_dir="$(mktemp -d)"
trap 'rm -rf "$build_dir"' EXIT

g++ -std=c++17 -Wall -Wextra -Wpedantic \
    "$repo_root/main.cpp" "$repo_root/donut.cpp" -o "$build_dir/app"
g++ -std=c++17 -Wall -Wextra -Wpedantic -I"$repo_root" \
    "$repo_root/tests/donut_tests.cpp" "$repo_root/donut.cpp" -o "$build_dir/donut_tests"

"$build_dir/donut_tests"
SPINNING_DONUT_APP="$build_dir/app" bash "$repo_root/tests/e2e_spinning_donut.sh"
