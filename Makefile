TARGET    := dd64
BASEROM   := baserom.z64
BUILD_DIR := build
ROM       := $(BUILD_DIR)/$(TARGET).z64
ELF       := $(BUILD_DIR)/$(TARGET).elf
LD_SCRIPT := $(BUILD_DIR)/$(TARGET).ld

CROSS   ?= mips-linux-gnu-
AS      := $(CROSS)as
LD      := $(CROSS)ld
OBJCOPY := $(CROSS)objcopy
PYTHON  ?= python3
KMC_DIR := tools/kmc
CC      := COMPILER_PATH=$(KMC_DIR) $(KMC_DIR)/gcc

ASFLAGS := -EB -march=vr4300 -mabi=32 -G0 --no-pad-sections -I include -I .
CFLAGS  := -G0 -mips3 -mgp32 -mfp32 -O2 -B $(KMC_DIR)/ -I include -I .
LDFLAGS := -T $(LD_SCRIPT) -T $(BUILD_DIR)/undefined_syms_auto.txt \
           -T $(BUILD_DIR)/undefined_funcs_auto.txt -T linker_syms.txt -T include/hardware_syms.ld \
           -Map $(BUILD_DIR)/$(TARGET).map --no-check-sections

S_FILES   := $(shell find asm -name '*.s' -not -path 'asm/nonmatchings/*' 2>/dev/null)
C_FILES   := $(shell find src -name '*.c' 2>/dev/null)
BIN_FILES := $(shell find assets -name '*.bin' 2>/dev/null)
O_FILES   := $(S_FILES:%=$(BUILD_DIR)/%.o) $(C_FILES:%=$(BUILD_DIR)/%.o) $(BIN_FILES:%=$(BUILD_DIR)/%.o)

.PHONY: all extract check clean distclean
.DEFAULT_GOAL := check

all: $(ROM)

check: $(ROM)
	@sha1sum -c $(TARGET).sha1

extract:
	rm -rf asm
	$(PYTHON) -m splat split $(TARGET).yaml

$(ROM): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(ELF): $(O_FILES) $(LD_SCRIPT) linker_syms.txt include/hardware_syms.ld
	$(LD) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.s.o: %.s
	@mkdir -p $(@D)
	$(AS) $(ASFLAGS) -o $@ $<

$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(@D)
	$(CC) -S $(CFLAGS) -o $(BUILD_DIR)/$*.c.s $<
	sed -E -f tools/fix_move.sed -i $(BUILD_DIR)/$*.c.s
	$(AS) $(ASFLAGS) -o $@ $(BUILD_DIR)/$*.c.s

$(BUILD_DIR)/%.bin.o: %.bin
	@mkdir -p $(@D)
	$(LD) -r -b binary -o $@ $<

clean:
	rm -rf $(BUILD_DIR)/asm $(BUILD_DIR)/src $(BUILD_DIR)/assets $(ELF) $(ROM)

distclean:
	rm -rf $(BUILD_DIR) asm assets
