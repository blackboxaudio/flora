#include "cortex.h"
#include "daisy_patch_sm.h"

using namespace daisy;
using namespace patch_sm;

DaisyPatchSM hardware;
cortex::Saturator saturator;

void AudioCallback(AudioHandle::InterleavingInputBuffer in,
    AudioHandle::InterleavingOutputBuffer out,
    size_t size)
{
    float driveKnob = hardware.GetAdcValue(CV_1);
    saturator.SetSaturationLevel(driveKnob + 1.0f);

    for (size_t idx = 0; idx < size; idx += 2) {
        out[idx] = (float)saturator.Process(in[idx]);
        out[idx + 1] = (float)saturator.Process(in[idx + 1]);
    }
}

int main(void)
{
    hardware.Init();
    hardware.StartAudio(AudioCallback);

    while (1) { }
}