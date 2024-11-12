#include "daisy_seed.h"
#include "neuron.h"

using namespace daisy;
using namespace neuron;

DaisySeed hardware;
Oscillator oscillator(DEFAULT_CONTEXT, 110.0f);

void AudioCallback(AudioHandle::InterleavingInputBuffer in,
    AudioHandle::InterleavingOutputBuffer out,
    size_t size)
{
    for (size_t idx = 0; idx < size; idx += 2) {
        auto sample = (float)oscillator.Generate();
        out[idx] = sample;
        out[idx + 1] = sample;
    }
}

int main(void)
{
    hardware.Configure();
    hardware.Init();
    hardware.SetAudioBlockSize(4);

    hardware.adc.Start();
    hardware.StartAudio(AudioCallback);

    while (1) { }
}