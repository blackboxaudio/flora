//! # Kudzu (Patch.Init)
//!
//! A distortion effect: the input is shaped by either a tanh **saturator** or a sine
//! **wavefolder** (selected by the B8 toggle), passed through a post low-pass filter, and
//! crossfaded back against the dry signal.
//!
//! - **CV_1** (drive)    — distortion intensity
//! - **CV_2** (tone)     — post low-pass cutoff (1 kHz – 18 kHz, logarithmic)
//! - **CV_3** (symmetry) — asymmetry of the shaping curve
//! - **CV_4** (mix)      — dry/wet blend
//! - **B8** (switch)     — saturator (on) vs wavefolder (off)
//!
//! Rust port of the original C++ `init/kudzu` patch. The saturator/wavefolder/filter are
//! reimplemented inline (the original used the neuron DSP library); behavior is faithful,
//! not bit-exact. The CV-out write from the original is omitted.

#![no_std]
#![no_main]

use bbx_daisy::{bbx_daisy_audio_with_controls, prelude::*};

struct Kudzu {
    /// Post low-pass filter state (one-pole).
    lp_state: f32,
    /// DC blocker history (removes offset introduced by asymmetric shaping).
    dc_x1: f32,
    dc_y1: f32,
}

impl Kudzu {
    fn new() -> Self {
        Self {
            lp_state: 0.0,
            dc_x1: 0.0,
            dc_y1: 0.0,
        }
    }

    /// Logarithmic mapping from 0.0-1.0 to a frequency range.
    #[inline]
    fn map_log(value: f32, min: f32, max: f32) -> f32 {
        let log_min = logf(min);
        let log_max = logf(max);
        expf(log_min + value * (log_max - log_min))
    }

    /// tanh soft-clip saturator. `saturation` ~1..25; `symmetry` 1.0 = symmetric.
    #[inline]
    fn saturate(x: f32, saturation: f32, symmetry: f32) -> f32 {
        let bias = (1.0 - symmetry) * 0.3;
        tanhf((x + bias) * saturation) / tanhf(saturation)
    }

    /// Sine wavefolder. `gain` ~1..7; `symmetry` 1.0 = symmetric.
    #[inline]
    fn fold(x: f32, gain: f32, symmetry: f32) -> f32 {
        let bias = (1.0 - symmetry) * 0.5;
        sinf((x * gain + bias) * 0.5 * PI)
    }
}

impl AudioProcessor for Kudzu {
    fn process(
        &mut self,
        input: &FrameBuffer<BLOCK_SIZE>,
        output: &mut FrameBuffer<BLOCK_SIZE>,
        controls: &Controls,
    ) {
        let drive = controls.cv[0];
        let tone = controls.cv[1];
        let symmetry_knob = controls.cv[2];
        let mix = controls.cv[3];
        let use_saturator = controls.switch;

        // C++: SetSymmetry(1.0 - symmetryKnob)
        let symmetry = 1.0 - symmetry_knob;
        let saturation = drive * 24.0 + 1.0; // 1..25
        let fold_gain = drive * 6.0 + 1.0; // 1..7

        // Post low-pass cutoff, log-mapped from 1 kHz (matches the C++ tone control).
        let cutoff = Self::map_log(tone, 1000.0, 18000.0);
        let omega = 2.0 * PI * cutoff / DEFAULT_SAMPLE_RATE;
        let alpha = omega / (omega + 1.0);

        for i in 0..BLOCK_SIZE {
            // Use the left input channel as the mono source.
            let dry = input.frame(i)[0];

            // Distortion stage (saturator or wavefolder, selected by B8).
            let distorted = if use_saturator {
                Self::saturate(dry, saturation, symmetry)
            } else {
                Self::fold(dry, fold_gain, symmetry)
            };

            // Post low-pass filter.
            self.lp_state += alpha * (distorted - self.lp_state);
            let filtered = self.lp_state;

            // DC blocker: y[n] = x[n] - x[n-1] + R * y[n-1].
            let wet = filtered - self.dc_x1 + 0.995 * self.dc_y1;
            self.dc_x1 = filtered;
            self.dc_y1 = wet;

            // Dry/wet mix. The original C++ mix line was a no-op (`out*(1-mix) + out*mix`);
            // this implements the intended crossfade between the dry input and processed signal.
            let out = dry * (1.0 - mix) + wet * mix;

            output.set_frame(i, out, out);
        }
    }
}

bbx_daisy_audio_with_controls!(Kudzu, Kudzu::new());
