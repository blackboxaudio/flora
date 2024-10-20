#include "cortex.h"
#include "daisy_patch_sm.h"

using namespace daisy;
using namespace patch_sm;

DaisyPatchSM hardware;
Switch toggle;

cortex::Saturator saturator;
cortex::Wavefolder wavefolder;
cortex::Filter filter;

void AudioCallback(AudioHandle::InterleavingInputBuffer in,
    AudioHandle::InterleavingOutputBuffer out,
    size_t size)
{
    hardware.ProcessAllControls();
    toggle.Debounce();

    float driveKnob = hardware.GetAdcValue(CV_1);
    float toneKnob = hardware.GetAdcValue(CV_2);
    float symmetryKnob = hardware.GetAdcValue(CV_3);
    float mixKnob = hardware.GetAdcValue(CV_4);
    bool useSaturator = toggle.Pressed();

    saturator.SetSaturation((driveKnob * 24.0f) + 1.0f);
    saturator.SetSymmetry(1.0f - symmetryKnob);

    wavefolder.SetInputGain((driveKnob * 6.0f) + 1.0f);
    wavefolder.SetSymmetry(1.0f - symmetryKnob);

    filter.SetCutoffFrequency(cortex::map(toneKnob, 1000.0f, cortex::FILTER_CUTOFF_FREQ_MAX, cortex::Mapping::LOG));

    for (size_t idx = 0; idx < size; idx++) {
        float originalSample = in[idx];
        auto distortedSample = useSaturator ? saturator.Process(originalSample) : wavefolder.Process(originalSample);
        auto filteredSample = (float)filter.Process(distortedSample);
        out[idx] = originalSample * (1.0f - mixKnob) + filteredSample * mixKnob;
    }

    hardware.WriteCvOut(2, 5.0f * out[0]);
}

int main(void)
{
    hardware.Init();

    toggle.Init(hardware.B8);

    hardware.StartAudio(AudioCallback);

    while (1) { }
}