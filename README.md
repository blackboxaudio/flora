# Flora

[![Build](https://github.com/blackboxaudio/flora/actions/workflows/ci.build.yml/badge.svg)](https://github.com/blackboxaudio/flora/actions/workflows/ci.build.yml)
[![License](https://img.shields.io/badge/License-MIT-yellow)](https://github.com/blackboxaudio/flora/blob/develop/LICENSE)

> Collection of Rust patches for the Electrosmith Daisy

## Overview

Welcome to Flora! This is a collection of Rust patches for the [Electrosmith Daisy](https://www.electro-smith.com/daisy) embedded audio platform.

It uses the [`bbx_daisy`](https://github.com/blackboxaudio/bbx_audio/tree/develop/bbx_daisy) crate from the [bbx_audio](https://github.com/blackboxaudio/bbx_audio) workspace for hardware abstraction and audio processing. The `bbx_audio` workspace provides a modular, real-time safe audio toolkit in Rust with support for DSP graphs, spatial audio, MIDI, and more.

## Supported Boards

| Board | Feature Flag | Status |
|-------|--------------|--------|
| Daisy Seed | `seed` | Ready |
| Daisy Pod | `pod` | Ready |
| Patch.Init() | `patch_sm` | Ready |

## Prerequisites

### Rust Toolchain

Install Rust using [rustup](https://rustup.rs/) if not already installed:

```bash
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
```

Add the ARM Cortex-M target for cross-compilation:

```bash
rustup target add thumbv7em-none-eabihf
```

> **Note:** The `bbx_audio` workspace uses Rust nightly. After cloning, `rustup` will automatically select the correct toolchain from `rust-toolchain.toml`.

### Flashing Tools

**Option A: Debug Probe (Recommended)**

Install [probe-rs](https://probe.rs/) for flashing and debugging:

```bash
cargo install probe-rs-tools
```

You'll need a debug probe (ST-Link, J-Link, or CMSIS-DAP compatible).

**Option B: DFU (No Debug Probe)**

Install dfu-util for USB bootloader flashing:

```bash
# macOS
brew install dfu-util

# Ubuntu/Debian
sudo apt install dfu-util
```

## Getting Started

Clone this repository:

```bash
git clone https://github.com/blackboxaudio/flora
cd flora/
```

## Building and Flashing

Each patch is an independent Cargo crate under `patches/<platform>/<patch>/`. The ARM target
(`thumbv7em-none-eabihf`) and the pinned nightly toolchain are configured at the repo root
(`.cargo/config.toml`, `rust-toolchain.toml`), so a plain `cargo build` cross-compiles.

### Build

```bash
# A single patch
cd patches/<platform>/<patch>   # e.g. patches/pod/lily
cargo build --release

# Or build every patch from the repo root
./scripts/build.sh
```

### Flash (DFU)

1. Enter DFU mode: hold **BOOT**, press **RESET**, release **RESET**, release **BOOT**.
2. Build and flash with the helper script (requires `dfu-util` and an `objcopy`):

```bash
./scripts/flash.sh <platform> <patch>   # e.g. ./scripts/flash.sh pod lily
```

The script cross-compiles the patch, converts the ELF to a raw binary, and flashes it to
`0x08000000` over DFU.

## Patches

Patches are organized by board under `patches/<platform>/`.

- **Seed** (`seed`)
  - [blink](./patches/seed/blink) — toggles the onboard LED (GPIO sanity check)
  - [daisy](./patches/seed/daisy) — a fixed 110 Hz sine oscillator
- **Pod** (`pod`)
  - [lily](./patches/pod/lily) — binaural oscillator with LFO frequency modulation (knobs 1 & 2)
  - [blink](./patches/pod/blink) — blinks the Pod and Seed LEDs (GPIO sanity check)
  - [audio-test](./patches/pod/audio-test) — low-level clock/power bring-up diagnostic
- **Init** (Patch.Init, `patch_sm`)
  - [kudzu](./patches/init/kudzu) — distortion: saturator/wavefolder + low-pass, 4 CV inputs + B8 toggle
  - [lotus](./patches/init/lotus) — binaural drone with LFO pitch modulation, 4 CV inputs + B8 toggle

For more end-to-end examples, see the [bbx_daisy examples](https://github.com/blackboxaudio/bbx_audio/tree/develop/bbx_daisy/examples).

## Writing Patches

### Dependencies

While `bbx_daisy` is unpublished, patches depend on it via a local relative path. Flora and
`bbx_audio` are expected to live side by side under the same parent directory:

```toml
[dependencies]
bbx_daisy = { path = "../../../../bbx_audio/bbx_daisy", default-features = false, features = ["seed"] }
```

Use the feature that matches your board — `seed`, `pod`, or `patch_sm` (Patch.Init).
`default-features = false` is required: `bbx_daisy` defaults to `seed`, and enabling a second
board feature trips its "exactly one product feature" check. Once `bbx_daisy` is published this
will become a versioned crates.io dependency.

### AudioProcessor Pattern

Implement the `AudioProcessor` trait and use the `bbx_daisy_audio!` macro:

```rust
#![no_std]
#![no_main]

use bbx_daisy::{bbx_daisy_audio, prelude::*};

struct SineOscillator {
    phase: f32,
    phase_inc: f32,
}

impl SineOscillator {
    fn new(frequency: f32) -> Self {
        Self {
            phase: 0.0,
            phase_inc: frequency / DEFAULT_SAMPLE_RATE,
        }
    }
}

impl AudioProcessor for SineOscillator {
    fn process(
        &mut self,
        _input: &FrameBuffer<BLOCK_SIZE>,
        output: &mut FrameBuffer<BLOCK_SIZE>,
        _controls: &Controls,
    ) {
        for i in 0..BLOCK_SIZE {
            let sample = sinf(self.phase * 2.0 * PI) * 0.5;
            output.set_frame(i, sample, sample);

            self.phase += self.phase_inc;
            if self.phase >= 1.0 {
                self.phase -= 1.0;
            }
        }
    }
}

bbx_daisy_audio!(SineOscillator, SineOscillator::new(440.0));
```

The `bbx_daisy_audio!` macro handles all hardware initialization, panic handling, and audio callback registration.

### GPIO Applications

For non-audio applications (LEDs, buttons, etc.), use `bbx_daisy_run!`:

```rust
#![no_std]
#![no_main]

use bbx_daisy::{bbx_daisy_run, prelude::*};

fn blink(mut board: Board) -> ! {
    let led_pin = board.gpioc.pc7.into_push_pull_output();
    let mut led = Led::new(led_pin);

    loop {
        led.toggle();
        board.delay.delay_ms(500u16);
    }
}

bbx_daisy_run!(blink);
```

## Troubleshooting

### "No probe found"

- Verify USB connection and that the probe is powered
- On Linux, add udev rules to `/etc/udev/rules.d/99-probe-rs.rules`:
  ```
  # ST-Link
  ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374*", MODE="0666"
  # J-Link
  ATTRS{idVendor}=="1366", ATTRS{idProduct}=="*", MODE="0666"
  ```
  Then run: `sudo udevadm control --reload-rules && sudo udevadm trigger`

### "Target not found" or "Chip not detected"

- Ensure the chip is configured as `STM32H750VBTx`
- Check SWD connections and that the Daisy is powered

### DFU device not detected

- Ensure you entered DFU mode correctly (LED should not blink)
- On Linux, add udev rule: `ATTRS{idVendor}=="0483", ATTRS{idProduct}=="df11", MODE="0666"`

## Documentation

- [bbx_audio Documentation](https://docs.bbx-audio.com/)
- [Embedded Development Guide](https://docs.bbx-audio.com/embedded.html)
- [bbx_daisy Crate README](https://github.com/blackboxaudio/bbx_audio/tree/develop/bbx_daisy)
- [Electrosmith Daisy Wiki](https://github.com/electro-smith/DaisyWiki/wiki)

## License

MIT License - see [LICENSE](./LICENSE) for details.
