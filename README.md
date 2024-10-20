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

### Flashing

To load a specific patch to the device, first navigate to that patches directory (e.g. `flora/patches/seed/blink`). Then just run a few `make` commands:
```bash 
# Clean the previous build's output (unnecessary for first time)
make clean

# Compiles the patch's source code
make

# Flashes the Daisy device with binaries
make program-dfu
```

:warning: **Make sure your Daisy device is in BOOT mode when flashing programs to it, otherwise you will get an error.**

For an easier way to flash your patches, use `flash.sh` located in the scripts folder:
```bash
./scripts/flash.sh <PLATFORM> <PATCH>
```

### Pre-Designed Patches

There are a number of existing patches, which have already been designed:

- Init
  - [Kudzu](https://github.com/blackboxdsp/flora/tree/develop/patches/init/kudzu) - An expressive distortion patch that twists, folds, and mangles audio for a sometimes unrecognizable outcome
  - [Lotus](https://github.com/blackboxdsp/flora/tree/develop/patches/init/lotus) - A rich binaural oscillator, emitting a harmonic meditation
- Seed
  - [Blink](https://github.com/blackboxdsp/flora/tree/develop/patches/seed/blink) - The "hello world" of embedded programming
  - [Daisy](https://github.com/blackboxdsp/flora/tree/develop/patches/seed/daisy) - A basic sine-wave oscillator

## Writing Patches

To write your own patch, you can simply modify an example or copy an example folder and re-write the code as necessary.

Here is an example of a simple oscillator patch:
```c++
#include "cortex.h"
#include "daisy_seed.h"

using namespace daisy;
using namespace cortex;

DaisySeed hardware;
Oscillator oscillator;

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
```
