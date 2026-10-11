OPENFPGA_SDK ?= /home/lin13/work/ONS_FPGA/openfpgaSDK

PROJECT_ROOT := $(abspath ../..)
SDK_DIR := $(OPENFPGA_SDK)/src/sdk
BUILD_DIR := $(PROJECT_ROOT)/build/sdk-baseline-app
OBJ_DIR := $(BUILD_DIR)

SRCS := main.c
USE_SDK_CONTAINER ?= 0

.DEFAULT_GOAL := all

include $(SDK_DIR)/sdk.mk

all: $(BUILD_DIR)/app.elf
	@$(SIZE) $<

clean: sdk-clean

.PHONY: all clean
