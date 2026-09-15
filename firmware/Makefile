TARGET  := main
CROSS   ?= riscv-none-elf-
CC      := $(CROSS)gcc
OBJCOPY := $(CROSS)objcopy
OBJDUMP := $(CROSS)objdump
SIZE    := $(CROSS)size

ARCH    := -march=rv32ec_zicsr -mabi=ilp32e

CFLAGS  := $(ARCH) -Os -g -Wall -Wextra \
           -ffunction-sections -fdata-sections \
           -fno-common
ASFLAGS := $(ARCH)
LDFLAGS := $(ARCH) -T linker.ld \
           -nostartfiles -nostdlib \
           -Wl,--gc-sections -Wl,--print-memory-usage \
           -Wl,-Map=$(TARGET).map

OBJS := startup.o main.o

all: $(TARGET).bin
	$(SIZE) $(TARGET).elf

$(TARGET).elf: $(OBJS) linker.ld
	$(CC) $(LDFLAGS) $(OBJS) -lgcc -o $@

startup.o: startup.S
	$(CC) $(ASFLAGS) -c $< -o $@

main.o: main.c ch32v003.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

disasm: $(TARGET).elf
	$(OBJDUMP) -d $< > $(TARGET).lst

MINICHLINK := ../ch32v003fun/minichlink/minichlink
$(MINICHLINK):
	make -C ../ch32v003fun/minichlink

flash: $(TARGET).bin $(MINICHLINK)
	$(MINICHLINK) -w $<

ifeq ($(OS),Windows_NT)
CLEAN_CMD = del /Q *.o *.elf *.bin *.map *.lst 2>NUL || exit 0
else
CLEAN_CMD = rm -f *.o *.elf *.bin *.map *.lst
endif

clean:
	$(CLEAN_CMD)

.PHONY: all flash clean disasm
