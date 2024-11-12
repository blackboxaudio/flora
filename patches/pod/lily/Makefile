# Project Name
TARGET = Lily

# Source Files
CPP_SOURCES = lily.cpp

# Library Directories
LIBDAISY_DIR = ../../../vendor/libDaisy
LIBNEURON_DIR = ../../../lib/cortex

# Include Neuron library
C_INCLUDES += -I$(LIBNEURON_DIR)/src
LIBS += -lcortex
LIBDIR += -L$(LIBNEURON_DIR)/target/release

# Include Daisy library Makefile
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
