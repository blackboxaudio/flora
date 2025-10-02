#include "daisy_patch_sm.h"
#include "neuron/neuron.h"

using namespace daisy;
using namespace patch_sm;

DaisyPatchSM hardware;
Switch toggle;

neuron::Saturator saturator;
neuron::Wavefolder wavefolder;
neuron::Filter filter;

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

    filter.SetCutoffFrequency(neuron::map(toneKnob, 1000.0f, neuron::FILTER_CUTOFF_FREQ_MAX, neuron::Mapping::LOG));

    neuron::Buffer<float> inputBuffer(const_cast<float*>(in), static_cast<int>(size));
    neuron::Buffer<float> outputBuffer(out, static_cast<int>(size));

    if (useSaturator) {
        saturator.Process(inputBuffer, outputBuffer);
    } else {
        wavefolder.Process(inputBuffer, outputBuffer);
    }

    filter.Process(outputBuffer, outputBuffer);

    for (int idx = 0; idx < static_cast<int>(size); idx++) {
        outputBuffer[idx] = outputBuffer[idx] * (1.0f - mixKnob) + outputBuffer[idx] * mixKnob;
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