OPENFPGA_SDK ?= /home/lin13/work/ONS_FPGA/openfpgaSDK
PROJECT_ROOT := $(abspath ../..)
SDK_DIR := $(OPENFPGA_SDK)/src/sdk
OUTPUT_API_DIR := $(PROJECT_ROOT)/.integration/openfpgaos/src/firmware/api
BUILD_DIR := $(PROJECT_ROOT)/build/dump54-app
OBJ_DIR := $(BUILD_DIR)
SRCS := main.c
CFLAGS += -I$(OUTPUT_API_DIR)
USE_SDK_CONTAINER ?= 0
.DEFAULT_GOAL := all
include $(SDK_DIR)/sdk.mk
all: $(BUILD_DIR)/app.elf
	@$(SIZE) $<
host-test:
	mkdir -p $(BUILD_DIR)
	cc -DOF_PC -O2 -Wall -Wextra -Werror -I$(OUTPUT_API_DIR) main.c -o $(BUILD_DIR)/pc-test
	cc -DOF_PC -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -I$(OUTPUT_API_DIR) test_dump54.c -o $(BUILD_DIR)/dump-test
	$(BUILD_DIR)/dump-test /mnt/c/Users/lin13/Desktop/game/逆转裁判.gba
.PHONY: all host-test
