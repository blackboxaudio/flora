#!/bin/bash
#
# Build every patch. Each patch is an independent Cargo crate that depends on the
# published bbx_daisy crate; the ARM target and toolchain come from the repo's
# .cargo/config.toml and rust-toolchain.toml.

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
cd "$REPO_ROOT" || exit 1

# Ensure the ARM Cortex-M target is installed (rust-toolchain.toml also requests it).
if command -v rustup &>/dev/null; then
    if ! rustup target list --installed 2>/dev/null | grep -q "thumbv7em-none-eabihf"; then
        printf "Installing ARM Cortex-M target (thumbv7em-none-eabihf)...\n"
        rustup target add thumbv7em-none-eabihf
    fi
fi

status=0
for cargo_toml in patches/*/*/Cargo.toml; do
    patch_dir="$(dirname "$cargo_toml")"
    printf "\nBuilding %s...\n" "$patch_dir"
    if ! (cd "$patch_dir" && cargo build --release); then
        printf "Failed to build %s\n" "$patch_dir"
        status=1
    fi
done

if [ $status -eq 0 ]; then
    printf "\nAll patches built successfully.\n"
else
    printf "\nSome patches failed to build.\n"
fi

exit $status
