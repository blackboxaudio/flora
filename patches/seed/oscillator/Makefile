# Project Name
TARGET = Oscillator

# Source Files
CPP_SOURCES = oscillator.cpp

# Library Directories
EXT_LIB_DIR = ../../../lib
LIBDAISY_DIR = $(EXT_LIB_DIR)/libDaisy
LIBCORTEX_DIR = $(EXT_LIB_DIR)/cortex

# Include Daisy library Makefile
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

# Include Cortex library
C_INCLUDES += I$(LIBCORTEX_DIR)/src
LIBDIR += -L$(LIBCORTEX_DIR)/target/release
