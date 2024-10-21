#include "cortex.h"
#include "daisy_pod.h"

using namespace daisy;

DaisyPod hardware;

cortex::Oscillator leader;
cortex::Oscillator follower;

const float MAX_DETUNE_AMOUNT = 20.0f;

void AudioCallback(AudioHandle::InterleavingInputBuffer in, AudioHandle::InterleavingOutputBuffer out, size_t size)
{
    hardware.ProcessAllControls();

    float tuneKnob = hardware.knob1.Value();
    float detuneKnob = hardware.knob2.Value();

    float leaderFrequency = cortex::map(tuneKnob, 65.406f, 261.626f, cortex::Mapping::LOG);
    leader.SetFrequency(leaderFrequency);

    float followerDetune = MAX_DETUNE_AMOUNT * ((detuneKnob * 2.0f) - 1.0f);
    follower.SetFrequency(leaderFrequency - followerDetune);

    for (size_t idx = 0; idx < size; idx += 2) {
        float leaderSample = leader.Generate();
        float followerSample = follower.Generate();

        out[idx] = leaderSample;
        out[idx + 1] = followerSample;
    }
}

int main(void)
{
    hardware.Init();
    hardware.SetAudioBlockSize(16);
    hardware.StartAdc();
    hardware.StartAudio(AudioCallback);

    while (1) { }
}
