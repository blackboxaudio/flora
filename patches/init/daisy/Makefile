# Project Name
TARGET = Daisy

# Source Files
CPP_SOURCES = daisy.cpp

# Library Directories
LIBDAISY_DIR = ../../../vendor/libDaisy
LIBCORTEX_DIR = ../../../lib/cortex

# Include Cortex library
C_INCLUDES += -I$(LIBCORTEX_DIR)/src
LIBS += -lcortex
LIBDIR += -L$(LIBCORTEX_DIR)/target/release

# Include Daisy library Makefile
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
