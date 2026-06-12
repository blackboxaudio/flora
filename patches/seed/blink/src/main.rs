//! # Blink (Seed)
//!
//! Minimal GPIO test: toggles the Daisy Seed onboard LED (PC7) at ~2 Hz.
//!
//! Rust port of the original C++ `seed/blink` patch.

#![no_std]
#![no_main]

use bbx_daisy::{bbx_daisy_run, prelude::*};

fn blink(mut board: Board) -> ! {
    let mut led = Led::new(board.gpioc.pc7.into_push_pull_output());
    loop {
        led.toggle();
        board.delay.delay_ms(250_u32);
    }
}

bbx_daisy_run!(blink);
