#include "cortex.h"
#include "daisy_patch_sm.h"

using namespace daisy;
using namespace patch_sm;

DaisyPatchSM hardware;
cortex::Oscillator oscillator(cortex::DEFAULT_CONTEXT, 220.0f);

void AudioCallback(AudioHandle::InterleavingInputBuffer in,
    AudioHandle::InterleavingOutputBuffer out,
    size_t size)
{
    hardware.ProcessAllControls();

    float coarse_knob = hardware.GetAdcValue(CV_1);
    float coarse = cortex::map(coarse_knob, 24.0f, 96.0f);

    float voct_cv = hardware.GetAdcValue(CV_5);
    float voct = cortex::map(voct_cv, 0.0f, 60.0f);

    float midi_nn = cortex::clamp(coarse + voct, 0.0f, 127.0f);
    float freq = cortex::midi_to_frequency(midi_nn);

    oscillator.SetFrequency(freq);

    for (size_t idx = 0; idx < size; idx += 2) {
        auto sample = (float)oscillator.Generate();
        out[idx] = sample;
        out[idx + 1] = sample;
    }
}

int main(void)
{
    hardware.Init();
    hardware.StartAudio(AudioCallback);

    while (1) { }
}
