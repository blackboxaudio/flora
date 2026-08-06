# audio-test

A bare-HAL clock/power bring-up diagnostic for the Daisy Pod — **not a music
patch**. It bypasses `bbx_daisy`'s board init and drives `stm32h7xx-hal`
directly to verify the VOS0 + 480MHz PLL1 (Iterative strategy) configuration,
reporting progress through LED blink patterns:

- **1 blink** — GPIO split worked
- **2 blinks** — delay configured
- **slow blink** — fully running at 480MHz

Useful when a board appears dead: if this patch blinks but an audio patch
does not, the fault is in the audio path (SAI/DMA/codec), not the core
clocks.

## Build & Flash

```bash
cd patches/pod/audio-test
cargo build --release
../../../scripts/flash.sh target/thumbv7em-none-eabihf/release/audio-test
```
