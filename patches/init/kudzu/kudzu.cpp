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
    hardware.ProcessAllControls();

    float driveKnob = hardware.GetAdcValue(CV_1);
    float driveCv = 1.0f; // hardware.GetAdcValue(CV_5);
    const float MAX_SATURATION = 24.0f;
    saturator.SetSaturation((driveKnob * driveCv * MAX_SATURATION) + 1.0f);

    float mixKnob = hardware.GetAdcValue(CV_4);

    for (size_t idx = 0; idx < size; idx += 2) {
        float originalLeftSample = in[idx];
        float originalRightSample = in[idx + 1];
        auto saturatedLeftSample = (float)saturator.Process(originalLeftSample);
        auto saturatedRightSample = (float)saturator.Process(originalRightSample);
        out[idx] = originalLeftSample * (1.0f - mixKnob) + saturatedLeftSample * mixKnob;
        out[idx + 1] = originalRightSample * (1.0f - mixKnob) + saturatedRightSample * mixKnob;
    }
}

int main(void)
{
    hardware.Init();
    hardware.StartAudio(AudioCallback);

    while (1) { }
}