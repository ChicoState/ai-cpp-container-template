#!/usr/bin/env bash

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
app_path="${SPINNING_DONUT_APP:-$repo_root/app}"
output_file="$(mktemp)"
help_file="$(mktemp)"
trap 'rm -f "$output_file" "$help_file"' EXIT

if [[ ! -x "$app_path" ]]; then
    echo "Expected executable at: $app_path" >&2
    exit 1
fi

"$app_path" --help > "$help_file"
grep -q "Usage:" "$help_file"

"$app_path" --frames 0 --help > "$help_file"
grep -q "Usage:" "$help_file"

"$app_path" --frames 0 --delay-ms 0 > /dev/null

if "$app_path" --frames -1 > /dev/null 2>&1; then
    echo "Negative frame count unexpectedly succeeded" >&2
    exit 1
fi

"$app_path" --frames 2 --delay-ms 0 > "$output_file"

clear_count="$(LC_ALL=C grep -aoF $'\033[2J' "$output_file" | wc -l)"
home_count="$(LC_ALL=C grep -aoF $'\033[H' "$output_file" | wc -l)"

if [[ "$clear_count" -ne 1 ]]; then
    echo "Expected one clear-screen sequence, got $clear_count" >&2
    exit 1
fi

if [[ "$home_count" -ne 2 ]]; then
    echo "Expected two cursor-home sequences, got $home_count" >&2
    exit 1
fi

if ! LC_ALL=C grep -aq '[.,\-~:;=!*#$@]' "$output_file"; then
    echo "Expected at least one donut glyph" >&2
    exit 1
fi

echo "Spinning donut end-to-end test passed."
