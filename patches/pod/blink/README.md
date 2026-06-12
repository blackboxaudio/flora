# Blink (Rust)

A minimal LED blink patch for Daisy Pod, written in Rust.

## Description

Blinks LED 2 Green (PA1) at 1 Hz to verify basic GPIO functionality. This is a simple non-audio patch demonstrating the `bbx_daisy_run!` macro.

## Controls

No controls - automatic LED blinking.

## Building & Flashing

From the repo root:
```bash
./scripts/flash.sh pod blink
```

Put the Daisy Pod in DFU mode first:
1. Hold BOOT button
2. Press RESET button
3. Release RESET
4. Release BOOT

## Hardware Requirements

- Daisy Pod
