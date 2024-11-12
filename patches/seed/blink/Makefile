# Project Name
TARGET = Blink

# Source Files
CPP_SOURCES = blink.cpp

# Library Directories
LIBDAISY_DIR = ../../../vendor/libDaisy
LIBNEURON_DIR = ../../../lib/neuron

# Include Neuron library
C_INCLUDES += -I$(LIBNEURON_DIR)/src
LIBS += -lneuron
LIBDIR += -L$(LIBNEURON_DIR)/target/release

# Include Daisy library Makefile
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
