OPENFPGA_SDK ?= /home/lin13/work/ONS_FPGA/openfpgaSDK
PROJECT_ROOT := $(abspath ../..)
SDK_DIR := $(OPENFPGA_SDK)/src/sdk
CART_API_DIR := $(PROJECT_ROOT)/.integration/openfpgaos/src/firmware/api
BUILD_DIR := $(PROJECT_ROOT)/build/ui107-app
ifeq ($(NOR108),1)
BUILD_DIR := $(PROJECT_ROOT)/build/ui108-app
CFLAGS += -DCB_NOR108=1
endif
ifeq ($(NOR109),1)
BUILD_DIR := $(PROJECT_ROOT)/build/ui109-app
CFLAGS += -DCB_NOR108=1 -DCB_NOR109=1
endif
OBJ_DIR := $(BUILD_DIR)
ifeq ($(NOR110),1)
BUILD_DIR := $(PROJECT_ROOT)/build/ui110-release-app
OBJ_DIR := $(BUILD_DIR)
CFLAGS += -DCB_NOR108=1 -DCB_NOR109=1 -DCB_NOR110=1
endif
SRCS := main.c ui.c model.c font.c analysis.c transfer.c program32.c program8.c
ifeq ($(GB_RTC189),1)
CFLAGS += -DCB_GB_RTC189=1
SRCS += gb_rtc189.c
endif
ifeq ($(GB_RTC187),1)
CFLAGS += -DCB_GB_RTC187=1
endif
ifeq ($(GB_RESTORE185),1)
CFLAGS += -DCB_GB_RESTORE185=1
endif
ifeq ($(GB_RTC186),1)
CFLAGS += -DCB_GB_RTC186=1
endif
ifeq ($(GB_DIAG182),1)
CFLAGS += -DCB_GB_DIAG182=1
endif
ifeq ($(GB_EMPTY181),1)
CFLAGS += -DCB_GB_EMPTY181=1
SRCS += gb_fallback181.c
endif
ifeq ($(GB_RESTORE180),1)
CFLAGS += -DCB_GB_RESTORE180=1 -DCB_GB_SAVE179=1 -DCB_GB_SAVE178=1 -DCB_GB_SAVE177=1 -DCB_GB_SAVE176=1 -DCB_GB_SAVE175=1
endif
ifeq ($(GB_SAVE179),1)
CFLAGS += -DCB_GB_SAVE179=1 -DCB_GB_SAVE178=1 -DCB_GB_SAVE177=1 -DCB_GB_SAVE176=1 -DCB_GB_SAVE175=1
endif
ifeq ($(GB_SAVE178),1)
CFLAGS += -DCB_GB_SAVE178=1 -DCB_GB_SAVE177=1 -DCB_GB_SAVE176=1 -DCB_GB_SAVE175=1
endif
ifeq ($(GB_SAVE177),1)
CFLAGS += -DCB_GB_SAVE177=1 -DCB_GB_SAVE176=1 -DCB_GB_SAVE175=1
endif
ifeq ($(GB_SAVE176),1)
CFLAGS += -DCB_GB_SAVE176=1 -DCB_GB_SAVE175=1
endif
ifeq ($(GB_SAVE175),1)
CFLAGS += -DCB_GB_SAVE175=1
endif
ifeq ($(LANG174),1)
CFLAGS += -DCB_LANG174=1
endif
ifeq ($(RTC173),1)
CFLAGS += -DCB_RTC173=1
endif
ifeq ($(RTC172),1)
CFLAGS += -DCB_RTC172=1 -DCB_RTC167=1
endif
ifeq ($(SAVE171),1)
CFLAGS += -DCB_SAVE171=1
endif
ifeq ($(RTC167),1)
CFLAGS += -DCB_RTC167=1
endif
ifeq ($(FLASH170),1)
CFLAGS += -DCB_FLASH170=1
endif
ifeq ($(FLASH166),1)
CFLAGS += -DCB_FLASH166=1 -DCB_FLASH165=1
endif
ifeq ($(FLASH165),1)
CFLAGS += -DCB_FLASH165=1
endif
ifeq ($(ROM164),1)
CFLAGS += -DCB_ROM164=1
SRCS += capacity164.c
endif
ifeq ($(SAVE148),1)
CFLAGS += -DCB_SAVE148=1 -DCB_SAVE147=1 -DCB_EEPROM139=1
endif
ifeq ($(SAVE147),1)
CFLAGS += -DCB_SAVE147=1 -DCB_EEPROM139=1
endif
ifeq ($(STAGES141),1)
CFLAGS += -DCB_STAGES141=1 -DCB_GBA_HEADER140=1 -DCB_EEPROM139=1
endif
ifeq ($(GBA_HEADER140),1)
CFLAGS += -DCB_GBA_HEADER140=1
endif
ifeq ($(EEPROM139_DIAG),1)
CFLAGS += -DCB_EEPROM139_DIAG=1 -DCB_EEPROM139=1
endif
ifeq ($(EEPROM139),1)
CFLAGS += -DCB_EEPROM139=1
endif
ifeq ($(SRAM138),1)
CFLAGS += -DCB_SRAM138=1 -DCB_SRAM137=1
endif
ifeq ($(SAVE159),1)
CFLAGS += -DCB_SAVE159=1 -DCB_SRAM138=1 -DCB_SRAM137=1
endif
ifeq ($(EEPROM162),1)
CFLAGS += -DCB_EEPROM162=1 -DCB_SAVE160=1 -DCB_SAVE159=1 -DCB_SRAM138=1 -DCB_SRAM137=1 -DCB_SAVE148=1 -DCB_SAVE147=1 -DCB_EEPROM139=1
else ifeq ($(SAVE160),1)
CFLAGS += -DCB_SAVE160=1 -DCB_SAVE159=1 -DCB_SRAM138=1 -DCB_SRAM137=1 -DCB_SAVE148=1 -DCB_SAVE147=1 -DCB_EEPROM139=1
endif
ifeq ($(SRAM137),1)
CFLAGS += -DCB_SRAM137=1
endif
ifeq ($(NOR111),1)
BUILD_DIR := $(PROJECT_ROOT)/build/ui111-release-app
OBJ_DIR := $(BUILD_DIR)
CFLAGS += -DCB_NOR108=1 -DCB_NOR109=1 -DCB_NOR110=1 -DCB_NOR111=1
endif
CFLAGS += -I$(CART_API_DIR)
ifeq ($(NOR112),1)
BUILD_DIR := $(PROJECT_ROOT)/build/ui112-dev-app
OBJ_DIR := $(BUILD_DIR)
CFLAGS += -DCB_NOR108=1 -DCB_NOR109=1 -DCB_NOR110=1 -DCB_NOR111=1 -DCB_NOR112=1
endif
ifeq ($(NOR130),1)
BUILD_DIR := $(PROJECT_ROOT)/build/ui130-release-app
OBJ_DIR := $(BUILD_DIR)
CFLAGS += -DCB_NOR108=1 -DCB_NOR109=1 -DCB_NOR110=1 -DCB_NOR111=1 -DCB_NOR112=1 -DCB_NOR130=1
endif
ifeq ($(STREAM131),1)
BUILD_DIR := $(PROJECT_ROOT)/build/ui131-release-app
OBJ_DIR := $(BUILD_DIR)
CFLAGS += -DCB_NOR108=1 -DCB_NOR109=1 -DCB_NOR110=1 -DCB_NOR111=1 -DCB_NOR112=1 -DCB_NOR130=1 -DCB_STREAM131=1
endif
ifeq ($(GPU_LEAN132),1)
CFLAGS += -DCB_GPU_LEAN132=1
endif
ifeq ($(GBA_IDENTIFY133),1)
CFLAGS += -DCB_GBA_IDENTIFY133=1
endif
USE_SDK_CONTAINER ?= 0
ifeq ($(CAMERA136),1)
CFLAGS += -DCB_CAMERA136=1
endif
ifeq ($(TIMERS135),1)
CFLAGS += -DCB_TIMERS135=1
endif
ifeq ($(GB_IDENTIFY134),1)
CFLAGS += -DCB_GB_IDENTIFY134=1
endif
.DEFAULT_GOAL := all
include $(SDK_DIR)/sdk.mk
all: $(BUILD_DIR)/app.elf
	@$(SIZE) $<
	@cd $(PROJECT_ROOT) && sha256sum app/ui98/*.[ch] app/ui97/model.c app/ui97/font.c app/gba_program65/program65.h app/gba_db55/*.[ch] $(CART_API_DIR)/of_nor.h $(CART_API_DIR)/of_services.h $(CART_API_DIR)/of_cart.h $(BUILD_DIR)/app.elf > $(BUILD_DIR)/sources.sha256
$(addprefix $(BUILD_DIR)/,$(SRCS:.c=.o)): ui.h model.h analysis.h transfer.h program_adapter.h ../gba_program65/program65.h
$(BUILD_DIR)/main.o $(BUILD_DIR)/ui.o: ui.h model.h
$(BUILD_DIR)/ui.o: lang174.h font174.h
$(BUILD_DIR)/main.o: settings174.h
$(BUILD_DIR)/main.o $(BUILD_DIR)/transfer.o $(BUILD_DIR)/model.o: ../../core/gb_save175/gb_save175.h ../../core/gb_save175/gb_save175.c gb_save175_transfer.h $(CART_API_DIR)/of_gb175.h
$(BUILD_DIR)/main.o $(BUILD_DIR)/ui.o $(BUILD_DIR)/model.o: camera136.h
$(BUILD_DIR)/model.o: model.h ../gba_db55/sha1_55.h
$(BUILD_DIR)/model.o: size107.h header_diag.h ../ui97/model.c
$(BUILD_DIR)/capacity164.o: capacity164.h ../gba_db55/db55.c ../gba_db55/db55.h ../gba_db55/db55_data.c ../gba_db55/sha1_55.h
$(BUILD_DIR)/main.o $(BUILD_DIR)/transfer.o: capacity164.h
$(BUILD_DIR)/ui.o: dump_plan.h version.h
$(BUILD_DIR)/main.o: version.h
$(BUILD_DIR)/main.o $(BUILD_DIR)/transfer.o: caps137.h
$(BUILD_DIR)/transfer.o: sram137_transfer.h ../../core/save/save.h ../../core/save/save.c ../../core/save/mori_head137.h
$(BUILD_DIR)/transfer.o: sram138_transfer.h ../../core/save/hardware138.c ../../core/save/hardware138.h
$(BUILD_DIR)/transfer.o: save_restore157.h save_backup159.h
$(BUILD_DIR)/transfer.o: flash165_transfer.h
$(BUILD_DIR)/transfer.o: flash170_transfer.h $(CART_API_DIR)/of_flash170.h
$(BUILD_DIR)/main.o: rtc167_ui.h
$(BUILD_DIR)/transfer.o: rtc168_transfer.h rtc168_policy.h rtc167_ui.h
$(BUILD_DIR)/transfer.o: rtc169_footer.h
$(BUILD_DIR)/transfer.o: gb_restore180_transfer.h gb_rtc187_transfer.h gb_rtc187_footer.h gb_rtc189_restore.h rtc_backup188_choice.h ../../core/gb_save175/rtc189.h
$(BUILD_DIR)/gb_rtc189.o: ../../core/gb_save175/rtc189.c ../../core/gb_save175/rtc189.h
$(BUILD_DIR)/transfer.o: rtc172_transfer.h ../../core/rtc/restore172.h $(CART_API_DIR)/of_rtc172.h
$(BUILD_DIR)/transfer.o: rtc173_transfer.h rtc173_probe.h
$(BUILD_DIR)/transfer.o: save_restore171.h $(CART_API_DIR)/of_save171.h
$(BUILD_DIR)/ui.o $(BUILD_DIR)/main.o: save_selection171.h ../../core/save/selection171.h
$(BUILD_DIR)/transfer.o: save_restore160.h $(CART_API_DIR)/of_save160.h
$(BUILD_DIR)/transfer.o: eeprom139_transfer.h $(CART_API_DIR)/of_save139.h
$(BUILD_DIR)/transfer.o: gbdump107.h dump_plan.h size107.h version.h ../gb_mbc5_fullrom86/sha256.c ../gb_mbc5_fullrom86/sha256.h
$(BUILD_DIR)/main.o: nor108_analysis.h
$(BUILD_DIR)/main.o $(BUILD_DIR)/ui.o: gb_selection176.h
$(BUILD_DIR)/transfer.o: nor108_transfer.h
$(BUILD_DIR)/transfer.o: nor130_transfer.h $(CART_API_DIR)/of_psram_cache.h
$(BUILD_DIR)/main.o $(BUILD_DIR)/transfer.o: ../../core/nor/write_policy112.h
$(addprefix $(BUILD_DIR)/,$(SRCS:.c=.o)): $(CART_API_DIR)/of_nor.h $(CART_API_DIR)/of_services.h $(CART_API_DIR)/of_cart.h
.PHONY: all
