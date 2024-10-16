# Flora

[![Build](https://github.com/blackboxdsp/flora/actions/workflows/ci.build.yml/badge.svg)](https://github.com/blackboxdsp/flora/actions/workflows/ci.build.yml)
[![License](https://img.shields.io/badge/License-MIT-yellow)](https://github.com/blackboxdsp/cortex/blob/develop/LICENSE)

> Collection of C++ patches for the Electrosmith Daisy 🌱

## Overview

Welcome to Flora! This is a collection of C++ patches for the Electrosmith Daisy platform. It uses an underlying DSP library, [Cortex](https://github.com/blackboxdsp/cortex), for constructing
the processing chains for different patches. 

## Getting Started

Follow the [official guide](https://github.com/electro-smith/DaisyWiki/wiki/1.-Setting-Up-Your-Development-Environment) from Electrosmith
to setup your development environment.

Clone this repository including the necessary submodules:
```bash
git clone --recurse-submodules https://github.com/blackboxdsp/flora
```

Build the required external libraries:
```bash 
# From the repository's root directory
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
