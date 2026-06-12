//! # Audio Test - VOS0 + 480MHz with PLL1 Iterative Strategy
//!
//! ## Blink patterns (all post-freeze via HAL):
//! - 1 blink = GPIO split worked
//! - 2 blinks = delay configured
//! - Slow blink = fully running at 480MHz

#![no_std]
#![no_main]

use bbx_daisy::__internal::panic_halt as _;
use cortex_m_rt::entry;
use stm32h7xx_hal::{pac, prelude::*, rcc::PllConfigStrategy};

#[entry]
fn main() -> ! {
    // Take peripherals
    let dp = pac::Peripherals::take().unwrap();
    let cp = cortex_m::Peripherals::take().unwrap();

    // VOS0 (high performance)
    let pwr = dp.PWR.constrain().vos0(&dp.SYSCFG).freeze();

    // HSE + PLL1 Iterative strategy for 480MHz (VOS0 allows this)
    let rcc = dp.RCC.constrain();
    let ccdr = rcc
        .use_hse(16.MHz())
        .pll1_strategy(PllConfigStrategy::Iterative) // Required for 480MHz
        .sys_ck(480.MHz())
        .freeze(pwr, &dp.SYSCFG);

    // Split GPIO (exactly like Board::init)
    let gpioc = dp.GPIOC.split(ccdr.peripheral.GPIOC);

    // If we get here, GPIO split worked! Blink once
    let mut led = gpioc.pc7.into_push_pull_output();
    let mut delay = cp.SYST.delay(ccdr.clocks);

    // 1 blink = success
    led.set_high();
    delay.delay_ms(500_u32);
    led.set_low();
    delay.delay_ms(500_u32);
    delay.delay_ms(1500_u32);

    // 2 blinks = delay works
    for _ in 0..2 {
        led.set_high();
        delay.delay_ms(500_u32);
        led.set_low();
        delay.delay_ms(500_u32);
    }
    delay.delay_ms(1500_u32);

    // Slow blink = running
    loop {
        led.set_high();
        delay.delay_ms(1000_u32);
        led.set_low();
        delay.delay_ms(1000_u32);
    }
}
