OPENFPGA_SDK ?= /home/lin13/work/ONS_FPGA/openfpgaSDK
PROJECT_ROOT := $(abspath ../..)
SDK_DIR := $(OPENFPGA_SDK)/src/sdk
OUTPUT_API_DIR := $(PROJECT_ROOT)/.integration/openfpgaos/src/firmware/api
BUILD_DIR := $(PROJECT_ROOT)/build/program77-app
OBJ_DIR := $(BUILD_DIR)
SRCS := main.c
CFLAGS += -I$(OUTPUT_API_DIR)
USE_SDK_CONTAINER ?= 0
ROM_PATH ?= /mnt/c/Users/lin13/Desktop/game/逆转裁判.gba
.DEFAULT_GOAL := all
include $(SDK_DIR)/sdk.mk
all: $(BUILD_DIR)/app.elf
	@$(SIZE) -A $<
host-test:
	mkdir -p $(BUILD_DIR)
	cc -DOF_PC -O2 -Wall -Wextra -Werror -I$(OUTPUT_API_DIR) main.c -o $(BUILD_DIR)/pc-ui
	$(BUILD_DIR)/pc-ui >/dev/null
	cc -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -I$(OUTPUT_API_DIR) test_program77.c -o $(BUILD_DIR)/program-test
	$(BUILD_DIR)/program-test '$(ROM_PATH)'
.PHONY: all host-test
