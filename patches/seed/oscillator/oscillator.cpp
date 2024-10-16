#include "daisy_seed.h"
#include "cortex.h"

using namespace daisy;
using namespace cortex;

DaisySeed  hardware;

Context context {
    (size_t)hardware.AudioSampleRate(),
    2,
    hardware.AudioBlockSize(),
};

Oscillator oscillator(context, 110.0f);

void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                   AudioHandle::InterleavingOutputBuffer out,
                   size_t                                size)
{
    for(size_t idx = 0; idx < size; idx += 2)
    {
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

    while(1) {}
}