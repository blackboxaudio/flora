#!/bin/bash

CORTEX_DIR=$PWD/lib/cortex
DAISY_DIR=$PWD/lib/libDaisy

printf "Building Cortex\n"
cd $CORTEX_DIR; ./scripts/build.sh release
if [ $? -ne 0 ]; then
    printf "Failed to compile Cortex library\n"
    exit 1
fi

printf "\nBuilding Daisy\n"
cd $DAISY_DIR; make -s clean; make -j -s
if [ $? -ne 0 ]; then
    printf "Failed to compile Daisy hardware library\n"
    exit 1
fi

printf "\Done.\n"
