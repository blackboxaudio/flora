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

    float coarseKnob = hardware.GetAdcValue(CV_1);
    float coarseTuning = cortex::map(coarseKnob, 24.0f, 96.0f);

    float vOctCv = hardware.GetAdcValue(CV_5);
    float vOct = cortex::map(vOctCv, 0.0f, 60.0f);

    float midiNote = cortex::clamp(coarseTuning + vOct, 0.0f, 127.0f);
    float frequency = cortex::midi_to_frequency(midiNote);

    oscillator.SetFrequency(frequency);

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
