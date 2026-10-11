OPENFPGA_SDK ?= /home/lin13/work/ONS_FPGA/openfpgaSDK

PROJECT_ROOT := $(abspath ../..)
SDK_DIR := $(OPENFPGA_SDK)/src/sdk
CART_API_DIR := $(PROJECT_ROOT)/.integration/openfpgaos/src/firmware/api
BUILD_DIR := $(PROJECT_ROOT)/build/smoke-app
OBJ_DIR := $(BUILD_DIR)
PC_FALLBACK := $(BUILD_DIR)/pc-fallback-test
HOST_CC ?= cc

SRCS := main.c
CFLAGS += -I$(CART_API_DIR)

# The installed WSL2 toolchain is used directly.  This keeps the application
# source in the ChisBurner project and leaves the shared SDK checkout
# unchanged.
USE_SDK_CONTAINER ?= 0

.DEFAULT_GOAL := all

include $(SDK_DIR)/sdk.mk

all: $(BUILD_DIR)/app.elf
	@$(SIZE) $<

clean: sdk-clean
	rm -f $(PC_FALLBACK)

# This test intentionally bypasses the SDK's SDL desktop backend.  The smoke
# application does not use video/audio/input on the PC; it only proves that
# the cartridge API fallback stays closed when no Pocket service table exists.
pc-fallback-test: main.c
	mkdir -p $(BUILD_DIR)
	$(HOST_CC) -DOF_PC -O2 -Wall -Wextra -I$(CART_API_DIR) $< -o $(PC_FALLBACK)
	$(PC_FALLBACK)

.PHONY: all clean pc-fallback-test
