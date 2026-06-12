//! # Blink - LED Test for Daisy Pod
//!
//! Minimal test using bbx_daisy abstractions.

#![no_std]
#![no_main]

use bbx_daisy::{bbx_daisy_run, prelude::*};

fn blink(mut board: Board) -> ! {
    // Pod LED 2 Green is on PA1, active-low (common anode)
    let mut pod_led = Led::new_active_low(board.gpioa.pa1.into_push_pull_output());
    // Seed onboard LED is on PC7, active-high
    let mut seed_led = Led::new(board.gpioc.pc7.into_push_pull_output());

    loop {
        pod_led.on();
        seed_led.on();
        board.delay.delay_ms(500_u32);
        pod_led.off();
        seed_led.off();
        board.delay.delay_ms(500_u32);
    }
}

bbx_daisy_run!(blink);
