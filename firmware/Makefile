# ============================================================
# CH32V003 firmware Makefile
# ============================================================

TARGET    := main
BUILD_DIR := build

# ---------- Toolchain ----------
CROSS   ?= riscv-none-elf-
CC      := $(CROSS)gcc
OBJCOPY := $(CROSS)objcopy
OBJDUMP := $(CROSS)objdump
SIZE    := $(CROSS)size

# minichlink должен быть доступен в PATH.
# При необходимости можно:
# make flash MINICHLINK=../ch32v003fun/minichlink/minichlink.exe
MINICHLINK ?= minichlink

# ---------- Architecture ----------
ARCH := -march=rv32ec_zicsr -mabi=ilp32e

# ---------- Compiler flags ----------
CFLAGS := \
	$(ARCH) \
	-std=c11 \
	-Os \
	-g \
	-Wall \
	-Wextra \
	-Wpedantic \
	-ffreestanding \
	-ffunction-sections \
	-fdata-sections \
	-fno-common \
	-MMD \
	-MP

ASFLAGS := \
	$(ARCH) \
	-g

# ---------- Linker flags ----------
LDFLAGS := \
	$(ARCH) \
	-T linker.ld \
	-nostartfiles \
	-nostdlib \
	-Wl,--gc-sections \
	-Wl,--print-memory-usage \
	-Wl,--build-id=none \
	-Wl,-Map=$(BUILD_DIR)/$(TARGET).map

# ---------- Files ----------
OBJS := \
	$(BUILD_DIR)/startup.o \
	$(BUILD_DIR)/main.o

DEPS := $(OBJS:.o=.d)

ELF := $(BUILD_DIR)/$(TARGET).elf
BIN := $(BUILD_DIR)/$(TARGET).bin
MAP := $(BUILD_DIR)/$(TARGET).map
LST := $(BUILD_DIR)/$(TARGET).lst


# ============================================================
# Main targets
# ============================================================

.PHONY: all
all: $(BIN) size


# ---------- ELF ----------
$(ELF): $(OBJS) linker.ld
	$(CC) $(LDFLAGS) $(OBJS) -lgcc -o $@


# ---------- Binary ----------
$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@


# ---------- C ----------
$(BUILD_DIR)/main.o: main.c ch32v003.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


# ---------- Startup assembly ----------
$(BUILD_DIR)/startup.o: startup.S | $(BUILD_DIR)
	$(CC) $(ASFLAGS) -c $< -o $@


# ============================================================
# Build directory
# ============================================================

$(BUILD_DIR):
ifeq ($(OS),Windows_NT)
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
else
	@mkdir -p "$(BUILD_DIR)"
endif


# ============================================================
# Utility targets
# ============================================================

.PHONY: size
size: $(ELF)
	$(SIZE) $(ELF)


.PHONY: disasm
disasm: $(ELF)
	$(OBJDUMP) -d -S $(ELF) > $(LST)
	@echo Disassembly: $(LST)


# ============================================================
# Flashing
# ============================================================

.PHONY: flash
flash: $(BIN)
	$(MINICHLINK) -w $(BIN) flash -b


.PHONY: reset
reset:
	$(MINICHLINK) -b


# ============================================================
# Cleaning
# ============================================================

.PHONY: clean
clean:
ifeq ($(OS),Windows_NT)
	@if exist "$(BUILD_DIR)" rmdir /S /Q "$(BUILD_DIR)"
else
	rm -rf "$(BUILD_DIR)"
endif


.PHONY: rebuild
rebuild: clean all


# ============================================================
# Help
# ============================================================

.PHONY: help
help:
	@echo "Targets:"
	@echo "  make          - build firmware"
	@echo "  make flash    - build if needed and flash MCU"
	@echo "  make reset    - reset/run MCU via minichlink"
	@echo "  make disasm   - generate annotated disassembly"
	@echo "  make size     - show firmware size"
	@echo "  make clean    - remove build directory"
	@echo "  make rebuild  - clean and rebuild everything"


# Automatically generated header dependencies.
-include $(DEPS)
