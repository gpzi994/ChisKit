OPENFPGA_SDK ?= /home/lin13/work/ONS_FPGA/openfpgaSDK
PROJECT_ROOT := $(abspath ../..)
SDK_DIR := $(OPENFPGA_SDK)/src/sdk
OUTPUT_API_DIR := $(PROJECT_ROOT)/.integration/openfpgaos/src/firmware/api
BUILD_DIR := $(PROJECT_ROOT)/build/storage-dynamic62-app
OBJ_DIR := $(BUILD_DIR)
HOST_CC ?= cc
SRCS := main.c
CFLAGS += -I$(OUTPUT_API_DIR)
USE_SDK_CONTAINER ?= 0
.DEFAULT_GOAL := all
include $(SDK_DIR)/sdk.mk
all: $(BUILD_DIR)/app.elf
	@$(SIZE) $<
clean: sdk-clean
host-test:
	mkdir -p $(BUILD_DIR)
	$(HOST_CC) -DOF_PC -O2 -Wall -Wextra -Werror -I$(OUTPUT_API_DIR) main.c -o $(BUILD_DIR)/pc-ui
	$(BUILD_DIR)/pc-ui
	$(HOST_CC) -O2 -Wall -Wextra -Werror test_dynamic62.c -o $(BUILD_DIR)/logic-test
	$(BUILD_DIR)/logic-test
.PHONY: all clean host-test
