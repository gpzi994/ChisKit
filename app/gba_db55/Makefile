OPENFPGA_SDK ?= /home/lin13/work/ONS_FPGA/openfpgaSDK
PROJECT_ROOT := $(abspath ../..)
SDK_DIR := $(OPENFPGA_SDK)/src/sdk
OUTPUT_API_DIR := $(PROJECT_ROOT)/.integration/openfpgaos/src/firmware/api
BUILD_DIR := $(PROJECT_ROOT)/build/db55-app
OBJ_DIR := $(BUILD_DIR)
SRCS := main.c db55.c db55_data.c
CFLAGS += -I$(OUTPUT_API_DIR)
USE_SDK_CONTAINER ?= 0
.DEFAULT_GOAL := all
include $(SDK_DIR)/sdk.mk
all: $(BUILD_DIR)/app.elf
	@$(SIZE) -A $<
host-test:
	mkdir -p $(BUILD_DIR)
	cc -DOF_PC -O2 -Wall -Wextra -Werror main.c db55.c db55_data.c -o $(BUILD_DIR)/pc-ui
	cc -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined test_db55.c db55.c db55_data.c -o $(BUILD_DIR)/db-test
	$(BUILD_DIR)/db-test /mnt/c/Users/lin13/Desktop/game/逆转裁判.gba
.PHONY: all host-test
