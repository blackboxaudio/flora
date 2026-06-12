//! # Lily - Binaural Oscillator Patch (Rust)
//!
//! A recreation of the C++ Lily patch with full knob control.
//!
//! - Knob 1 (Tune): Oscillator frequency (65.4Hz - 261.6Hz, logarithmic)
//! - Knob 2 (Detune): LFO frequency (0.01Hz - 24Hz, logarithmic)
//!
//! The LFO modulates the oscillator frequency to create subtle binaural
//! beating effects when the detune amount is small.

#![no_std]
#![no_main]

use bbx_daisy::{bbx_daisy_audio_with_controls, prelude::*};

// Oscillator frequency range (C2 to C4)
const OSC_MIN_FREQ: f32 = 65.406;
const OSC_MAX_FREQ: f32 = 261.626;

// LFO frequency range
const LFO_MIN_FREQ: f32 = 0.01;
const LFO_MAX_FREQ: f32 = 24.0;

// LFO modulation depth (Hz)
const LFO_DEPTH: f32 = 1.0;

struct Lily {
    // Oscillator state
    osc_phase: f32,

    // LFO state
    lfo_phase: f32,

    // Parameter smoothing (simple one-pole filters)
    osc_freq_smooth: f32,
    lfo_freq_smooth: f32,
}

impl Lily {
    fn new() -> Self {
        Self {
            osc_phase: 0.0,
            lfo_phase: 0.0,
            osc_freq_smooth: 130.813, // C3 default
            lfo_freq_smooth: 2.0,
        }
    }

    /// Logarithmic mapping from 0.0-1.0 to frequency range.
    ///
    /// This gives a more musical feel for frequency control, as human
    /// pitch perception is logarithmic.
    #[inline]
    fn map_log(value: f32, min: f32, max: f32) -> f32 {
        let log_min = logf(min);
        let log_max = logf(max);
        expf(log_min + value * (log_max - log_min))
    }
}

impl AudioProcessor for Lily {
    fn process(
        &mut self,
        _input: &FrameBuffer<BLOCK_SIZE>,
        output: &mut FrameBuffer<BLOCK_SIZE>,
        controls: &Controls,
    ) {
        // Map knobs to frequency ranges (logarithmic, like C++ version)
        let target_osc_freq = Self::map_log(controls.knob1, OSC_MIN_FREQ, OSC_MAX_FREQ);
        let target_lfo_freq = Self::map_log(controls.knob2, LFO_MIN_FREQ, LFO_MAX_FREQ);

        // Smooth parameter changes (simple one-pole filter)
        // Coefficient of 0.99 gives ~100ms smoothing time at 48kHz
        const SMOOTH: f32 = 0.99;
        self.osc_freq_smooth = SMOOTH * self.osc_freq_smooth + (1.0 - SMOOTH) * target_osc_freq;
        self.lfo_freq_smooth = SMOOTH * self.lfo_freq_smooth + (1.0 - SMOOTH) * target_lfo_freq;

        // Pre-calculate phase increments
        let lfo_phase_inc = self.lfo_freq_smooth / DEFAULT_SAMPLE_RATE;

        for i in 0..BLOCK_SIZE {
            // Generate LFO (bipolar sine, -1 to +1)
            let lfo_value = sinf(self.lfo_phase * 2.0 * PI);

            // Modulate oscillator frequency
            // LFO depth of 1.0 means ±1Hz modulation per unit LFO output
            let modulated_freq = self.osc_freq_smooth + (lfo_value * LFO_DEPTH);

            // Compute phase increment for this sample
            let osc_phase_inc = modulated_freq / DEFAULT_SAMPLE_RATE;

            // Generate oscillator (sine wave)
            let sample = sinf(self.osc_phase * 2.0 * PI) * 0.5;

            // Output stereo
            output.set_frame(i, sample, sample);

            // Advance phases
            self.osc_phase += osc_phase_inc;
            if self.osc_phase >= 1.0 {
                self.osc_phase -= 1.0;
            }

            self.lfo_phase += lfo_phase_inc;
            if self.lfo_phase >= 1.0 {
                self.lfo_phase -= 1.0;
            }
        }
    }
}

// Use bbx_daisy_audio_with_controls! to enable ADC knob reading on Pod hardware
bbx_daisy_audio_with_controls!(Lily, Lily::new());
