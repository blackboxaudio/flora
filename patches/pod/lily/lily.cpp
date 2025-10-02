#include "neuron/neuron.h"
#include "daisy_pod.h"

using namespace daisy;

DaisyPod hardware;

neuron::Oscillator oscillator;
neuron::Lfo lfo;

const float MAX_DETUNE_AMOUNT = 20.0f;

void AudioCallback(AudioHandle::InterleavingInputBuffer in, AudioHandle::InterleavingOutputBuffer out, size_t size)
{
    hardware.ProcessAllControls();

    float oscTuneKnob = hardware.knob1.Value();
    float lfoTuneKnob = hardware.knob2.Value();

    float lfoFreq = neuron::map(lfoTuneKnob, 0.01f, 24.0f, neuron::Mapping::LOG);
    lfo.SetFrequency(lfoFreq);

    lfo.Modulate();

    float oscillatorFreq = neuron::map(oscTuneKnob, 65.406f, 261.626f, neuron::Mapping::LOG);
    oscillator.SetFrequency(oscillatorFreq);

    neuron::Buffer<float> outputBuffer(out, static_cast<int>(size));
    oscillator.Generate(outputBuffer);
}

int main(void)
{
    neuron::Context context = {
        48000.0f,
        2,
        16
    };

    lfo.SetContext(context);
    oscillator.SetContext(context);
    oscillator.AttachModulator(neuron::OscillatorParameter::OSC_FREQUENCY, &lfo);
    oscillator.SetModulationDepth(neuron::OscillatorParameter::OSC_FREQUENCY, 1.0f);

    hardware.Init();
    hardware.SetAudioBlockSize(16);

    hardware.StartAdc();
    hardware.StartAudio(AudioCallback);

    while (1) { }
}
