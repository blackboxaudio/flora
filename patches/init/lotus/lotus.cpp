#include "cortex.h"
#include "daisy_patch_sm.h"

using namespace daisy;
using namespace patch_sm;

DaisyPatchSM hardware;
Switch toggle;

cortex::Oscillator leader;
cortex::Oscillator follower;
cortex::Oscillator lfo;

const float MAX_DETUNE_AMOUNT = 20.0f;

void AudioCallback(AudioHandle::InterleavingInputBuffer in, AudioHandle::InterleavingOutputBuffer out, size_t size)
{
    hardware.ProcessAllControls();
    toggle.Debounce();

    float tuneKnob = hardware.GetAdcValue(CV_1);
    float detuneKnob = hardware.GetAdcValue(CV_2);
    float lfoRateKnob = hardware.GetAdcValue(CV_3);
    float lfoDepthKnob = hardware.GetAdcValue(CV_4);

    bool isSyncOn = toggle.Pressed();
    if (isSyncOn) {
        leader.AttachFollower(&follower);
    } else {
        leader.DetachFollower();
    }

    float leaderFrequency = cortex::map(tuneKnob, 65.406f, 261.626f, cortex::Mapping::LOG);
    leader.SetFrequency(leaderFrequency);

    float followerDetune = MAX_DETUNE_AMOUNT * ((detuneKnob * 2.0f) - 1.0f);
    follower.SetFrequency(leaderFrequency - followerDetune);

    float lfoRate = (lfoRateKnob * 1000.0f) + 0.1f;
    lfo.SetFrequency(lfoRate);

    for (size_t idx = 0; idx < size; idx += 2) {
        float leaderSample = leader.Generate();
        float followerSample = follower.Generate();

        float lfoMod = lfoDepthKnob * lfo.Generate();
        float modulatedLeaderFrequency = leaderFrequency + (MAX_DETUNE_AMOUNT * lfoMod);
        leader.SetFrequency(modulatedLeaderFrequency + (MAX_DETUNE_AMOUNT * lfoMod));
        follower.SetFrequency(modulatedLeaderFrequency - followerDetune);

        out[idx] = leaderSample;
        out[idx + 1] = followerSample;
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
