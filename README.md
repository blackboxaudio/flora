# Flora

[![Build](https://github.com/blackboxdsp/flora/actions/workflows/ci.build.yml/badge.svg)](https://github.com/blackboxdsp/flora/actions/workflows/ci.build.yml)
[![License](https://img.shields.io/badge/License-MIT-yellow)](https://github.com/blackboxdsp/cortex/blob/develop/LICENSE)

> Collection of C++ patches for the Electrosmith Daisy 🌱

## Overview

Welcome to Flora! This is a collection of C++ patches for the Electrosmith Daisy platform.

It uses an underlying DSP library, [Cortex](https://github.com/blackboxdsp/cortex), for constructing
the processing chains for different patches.

## Getting Started

Follow the [official guide](https://github.com/electro-smith/DaisyWiki/wiki/1.-Setting-Up-Your-Development-Environment) from Electrosmith
to setup your development environment.

Clone this repository including the necessary submodules:
```bash
git clone --recurse-submodules https://github.com/blackboxdsp/flora
cd flora/
```

Build the required external libraries:
```bash
./scripts/build.sh
```

## Loading Patches

To load a specific patch, first navigate to that patches directory (e.g. `flora/patches/seed/blink`). Then just run a few `make` commands:
```bash 
# Compiles the patch's source code
make

# Flashes the Daisy device with binaries
make program-dfu
```

:warning: Be sure to run `make clean` when compiling source code!

Alternatively, you can run the `flash.sh` script:
```bash
./scripts/flash.sh <PLATFORM> <PATCH>
```

## Writing Patches

To write your own patch, you can simply modify an example or copy an example folder and re-write the code as necessary.

Here is an example of a simple oscillator patch:
```c++
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

void AudioCallback(
    AudioHandle::InterleavingInputBuffer in,
    AudioHandle::InterleavingOutputBuffer out,
    size_t size
) {
    for(size_t idx = 0; idx < size; idx += 2)
    {
        auto sample = (float)oscillator.Generate();
        out[idx] = sample;
        out[idx + 1] = sample;
    }
}


int main(void) {
    hardware.Configure();
    hardware.Init();
    hardware.SetAudioBlockSize(4);

    hardware.adc.Start();
    hardware.StartAudio(AudioCallback);

    while(1) { }
}
```
