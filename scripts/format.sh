#!/bin/bash
#
# Format every patch with rustfmt.

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
cd "$REPO_ROOT" || exit 1

printf "Formatting Rust code...\n"
find patches -name 'Cargo.toml' -exec dirname {} \; | while read -r dir; do
    (cd "$dir" && cargo fmt)
done

printf "Done.\n"
