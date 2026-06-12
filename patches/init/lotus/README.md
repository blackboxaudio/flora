# Lotus (Patch.Init)

A binaural drone — a `leader` sine (left) and a detuned `follower` sine (right), both
pitch-modulated by a shared LFO. With B8 off the oscillators free-run and beat; with it on the
follower is hard-synced to the leader.

- **CV_1** (tune) — leader frequency (65.4–261.6 Hz, C2–C4, logarithmic)
- **CV_2** (detune) — follower detune (±20 Hz)
- **CV_3** (lfo rate) — LFO frequency (0.1–1000 Hz)
- **CV_4** (lfo depth) — LFO pitch-modulation depth
- **B8** — follower hard-sync (on) vs free-running (off)

> Requires the bbx_daisy Patch.Init control support (CV inputs + B8 switch on PB9).

```bash
./scripts/flash.sh init lotus
```
