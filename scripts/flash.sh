#!/bin/bash
#
# Build and flash a patch to a Daisy over DFU.
#
# Usage: ./scripts/flash.sh <platform> <patch>   e.g. ./scripts/flash.sh pod lily
#
# Put the Daisy in DFU mode first: hold BOOT, tap RESET, release BOOT.

PLATFORM=${1:-seed}
PATCH=${2:-blink}

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
PATCH_DIR="$REPO_ROOT/patches/$PLATFORM/$PATCH"

if [ ! -f "$PATCH_DIR/Cargo.toml" ]; then
    printf "Cannot find patch \"%s/%s\" (no Cargo.toml at %s)\n" "$PLATFORM" "$PATCH" "$PATCH_DIR"
    exit 1
fi

cd "$PATCH_DIR" || exit 1

printf "Building \"%s\" for Daisy %s...\n" "$PATCH" "$PLATFORM"
if ! cargo build --release; then
    printf "Failed to build patch\n"
    exit 1
fi

BINARY_NAME=$(grep -m1 'name = ' Cargo.toml | sed 's/.*"\(.*\)".*/\1/')
BINARY="target/thumbv7em-none-eabihf/release/$BINARY_NAME"
if [ ! -f "$BINARY" ]; then
    printf "Cannot find built binary at %s\n" "$BINARY"
    exit 1
fi

printf "Flashing via DFU...\n"
BIN_FILE="${BINARY}.bin"

if command -v arm-none-eabi-objcopy &>/dev/null; then
    arm-none-eabi-objcopy -O binary "$BINARY" "$BIN_FILE"
elif command -v llvm-objcopy &>/dev/null; then
    llvm-objcopy -O binary "$BINARY" "$BIN_FILE"
elif command -v rust-objcopy &>/dev/null; then
    rust-objcopy -O binary "$BINARY" "$BIN_FILE"
else
    printf "No objcopy tool found (tried arm-none-eabi-objcopy, llvm-objcopy, rust-objcopy)\n"
    exit 1
fi

dfu-util -a 0 -s 0x08000000:leave -D "$BIN_FILE"

printf "\nDone.\n"
