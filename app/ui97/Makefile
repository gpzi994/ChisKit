OPENFPGA_SDK ?= /home/lin13/work/ONS_FPGA/openfpgaSDK
PROJECT_ROOT := $(abspath ../..)
SDK_DIR := $(OPENFPGA_SDK)/src/sdk
CART_API_DIR := $(PROJECT_ROOT)/.integration/openfpgaos/src/firmware/api
BUILD_DIR := $(PROJECT_ROOT)/build/ui97-app
OBJ_DIR := $(BUILD_DIR)
SRCS := main.c ui.c model.c font.c
CFLAGS += -I$(CART_API_DIR)
USE_SDK_CONTAINER ?= 0
.DEFAULT_GOAL := all
include $(SDK_DIR)/sdk.mk
all: $(BUILD_DIR)/app.elf
	@$(SIZE) $<
	@cd $(PROJECT_ROOT) && sha256sum app/ui97/main.c app/ui97/ui.c app/ui97/model.c app/ui97/font.c app/ui97/model.h app/ui97/ui.h app/gba_db55/sha1_55.h build/ui97-app/app.elf > build/ui97-app-sources.sha256
$(BUILD_DIR)/main.o $(BUILD_DIR)/ui.o: ui.h model.h
$(BUILD_DIR)/model.o: model.h ../gba_db55/sha1_55.h
.PHONY: all
