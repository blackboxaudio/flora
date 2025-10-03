#include "neuron/neuron.h"
#include "daisy_pod.h"

using namespace daisy;

DaisyPod hardware;

neuron::Oscillator oscillator;
neuron::Lfo lfo;

const int BLOCK_SIZE = 16;
neuron::Context context = { 48000.0f, 2, BLOCK_SIZE };
neuron::Sample monoBuffer[BLOCK_SIZE];

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

    neuron::Buffer<float> tempBuffer(monoBuffer, static_cast<int>(size) / 2);
    oscillator.Generate(tempBuffer);

    for (size_t i = 0, j = 0; i < size; i += 2, j++) {
        out[i] = monoBuffer[j];
        out[i + 1] = monoBuffer[j];
    }
}

int main(void)
{
    lfo.SetContext(context);
    oscillator.SetContext(context);
    oscillator.AttachModulator(neuron::OscillatorParameter::OSC_FREQUENCY, &lfo);
    oscillator.SetModulationDepth(neuron::OscillatorParameter::OSC_FREQUENCY, 1.0f);

    hardware.Init();
    hardware.SetAudioBlockSize(context.blockSize);

    hardware.StartAdc();
    hardware.StartAudio(AudioCallback);

    while (1) { }
}
