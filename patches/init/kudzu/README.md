# Kudzu (Patch.Init)

A distortion effect — the input is shaped by a tanh **saturator** or a sine **wavefolder**
(selected by the B8 toggle), low-pass filtered, then crossfaded against the dry signal.

- **CV_1** (drive) — distortion intensity
- **CV_2** (tone) — post low-pass cutoff (1 kHz – 18 kHz, logarithmic)
- **CV_3** (symmetry) — asymmetry of the shaping curve
- **CV_4** (mix) — dry/wet blend
- **B8** — saturator (on) vs wavefolder (off)

> Requires the bbx_daisy Patch.Init control support (CV inputs + B8 switch on PB9).

```bash
./scripts/flash.sh init kudzu
```
