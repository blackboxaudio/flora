#!/bin/bash

NEURON_DIR=$PWD/lib/neuron
DAISY_DIR=$PWD/vendor/libDaisy

printf "Building Neuron...\n"
cd $NEURON_DIR; make -s clean; make -j -s
if [ $? -ne 0 ]; then
    printf "Failed to compile Neuron library\n"
    exit 1
fi

printf "Building Daisy...\n"
cd $DAISY_DIR; make -s clean; make -j -s
if [ $? -ne 0 ]; then
    printf "Failed to compile Daisy hardware library\n"
    exit 1
fi

printf "\nDone.\n"
