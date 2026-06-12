//! # Daisy (Seed)
//!
//! A fixed 110 Hz sine oscillator on the Daisy Seed, written to both channels.
//!
//! Rust port of the original C++ `seed/daisy` patch.

#![no_std]
#![no_main]

use bbx_daisy::{bbx_daisy_audio, prelude::*};

const FREQUENCY: f32 = 110.0;
const AMPLITUDE: f32 = 0.5;

struct Daisy {
    phase: f32,
    phase_inc: f32,
}

impl Daisy {
    fn new() -> Self {
        Self {
            phase: 0.0,
            phase_inc: FREQUENCY / DEFAULT_SAMPLE_RATE,
        }
    }
}

impl AudioProcessor for Daisy {
    fn process(
        &mut self,
        _input: &FrameBuffer<BLOCK_SIZE>,
        output: &mut FrameBuffer<BLOCK_SIZE>,
        _controls: &Controls,
    ) {
        for i in 0..BLOCK_SIZE {
            let sample = sinf(self.phase * 2.0 * PI) * AMPLITUDE;
            output.set_frame(i, sample, sample);

            self.phase += self.phase_inc;
            if self.phase >= 1.0 {
                self.phase -= 1.0;
            }
        }
    }
}

bbx_daisy_audio!(Daisy, Daisy::new());
