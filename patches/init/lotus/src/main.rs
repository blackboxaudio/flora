//! # Lotus (Patch.Init)
//!
//! A binaural drone: a `leader` sine oscillator (left) and a detuned `follower` sine
//! oscillator (right), both pitch-modulated by a shared LFO. With the B8 toggle off the
//! oscillators free-run (binaural beating); with it on the follower is hard-synced to the
//! leader.
//!
//! - **CV_1** (tune)      — leader frequency (65.4–261.6 Hz, C2–C4, logarithmic)
//! - **CV_2** (detune)    — follower detune (±20 Hz around the leader)
//! - **CV_3** (lfo rate)  — LFO frequency (0.1–1000 Hz)
//! - **CV_4** (lfo depth) — LFO pitch-modulation depth
//! - **B8** (switch)      — follower hard-sync (on) vs free-running (off)
//!
//! Rust port of the original C++ `init/lotus` patch (the neuron oscillators/LFO are
//! reimplemented inline). The original's doubled leader-frequency modulation is cleaned up so
//! both oscillators share the same LFO pitch modulation; the CV-out write is omitted.

#![no_std]
#![no_main]

use bbx_daisy::{bbx_daisy_audio_with_controls, prelude::*};

// Leader frequency range (C2 to C4).
const OSC_MIN_FREQ: f32 = 65.406;
const OSC_MAX_FREQ: f32 = 261.626;

// Maximum follower detune (Hz) and LFO pitch-modulation depth (Hz).
const MAX_DETUNE: f32 = 20.0;
const LFO_PITCH_DEPTH: f32 = 20.0;

// Maximum LFO rate (Hz).
const LFO_MAX_RATE: f32 = 1000.0;

const AMPLITUDE: f32 = 0.5;

struct Lotus {
    leader_phase: f32,
    follower_phase: f32,
    lfo_phase: f32,
}

impl Lotus {
    fn new() -> Self {
        Self {
            leader_phase: 0.0,
            follower_phase: 0.0,
            lfo_phase: 0.0,
        }
    }

    /// Logarithmic mapping from 0.0-1.0 to a frequency range.
    #[inline]
    fn map_log(value: f32, min: f32, max: f32) -> f32 {
        let log_min = logf(min);
        let log_max = logf(max);
        expf(log_min + value * (log_max - log_min))
    }
}

impl AudioProcessor for Lotus {
    fn process(
        &mut self,
        _input: &FrameBuffer<BLOCK_SIZE>,
        output: &mut FrameBuffer<BLOCK_SIZE>,
        controls: &Controls,
    ) {
        let tune = controls.cv[0];
        let detune = controls.cv[1];
        let lfo_rate_knob = controls.cv[2];
        let lfo_depth = controls.cv[3];
        let sync = controls.switch;

        let leader_freq = Self::map_log(tune, OSC_MIN_FREQ, OSC_MAX_FREQ);
        let follower_detune = MAX_DETUNE * (detune * 2.0 - 1.0); // -20..+20 Hz
        let lfo_rate = lfo_rate_knob * LFO_MAX_RATE + 0.1; // 0.1..1000.1 Hz
        let lfo_phase_inc = lfo_rate / DEFAULT_SAMPLE_RATE;

        for i in 0..BLOCK_SIZE {
            // Shared LFO (bipolar sine), scaled by depth.
            let lfo = sinf(self.lfo_phase * 2.0 * PI) * lfo_depth;

            // Pitch modulation applied to both oscillators.
            let modulated = leader_freq + LFO_PITCH_DEPTH * lfo;
            let leader_f = modulated;
            let follower_f = modulated - follower_detune;

            // Generate oscillators: leader -> left, follower -> right (binaural).
            let leader_sample = sinf(self.leader_phase * 2.0 * PI) * AMPLITUDE;
            let follower_sample = sinf(self.follower_phase * 2.0 * PI) * AMPLITUDE;
            output.set_frame(i, leader_sample, follower_sample);

            // Advance the leader and note when it completes a cycle.
            self.leader_phase += leader_f / DEFAULT_SAMPLE_RATE;
            let leader_wrapped = self.leader_phase >= 1.0;
            if leader_wrapped {
                self.leader_phase -= 1.0;
            }

            // Advance the follower.
            self.follower_phase += follower_f / DEFAULT_SAMPLE_RATE;
            if self.follower_phase >= 1.0 {
                self.follower_phase -= 1.0;
            }

            // Hard sync: reset the follower's phase when the leader wraps (approximates the
            // original's AttachFollower). Off -> the oscillators free-run and beat.
            if sync && leader_wrapped {
                self.follower_phase = self.leader_phase;
            }

            self.lfo_phase += lfo_phase_inc;
            if self.lfo_phase >= 1.0 {
                self.lfo_phase -= 1.0;
            }
        }
    }
}

bbx_daisy_audio_with_controls!(Lotus, Lotus::new());
