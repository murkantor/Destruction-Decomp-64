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
OPTFLAGS := -O2
CFLAGS  = -G0 -mips3 -mgp32 -mfp32 $(OPTFLAGS) -B $(KMC_DIR)/ -I include -I .
LDFLAGS := -T $(LD_SCRIPT) -T $(BUILD_DIR)/undefined_syms_auto.txt \
           -T $(BUILD_DIR)/undefined_funcs_auto.txt -T linker_syms.txt -T include/hardware_syms.ld \
           -Map $(BUILD_DIR)/$(TARGET).map --no-check-sections

S_FILES   := $(shell find asm -name '*.s' -not -path 'asm/nonmatchings/*' 2>/dev/null)
C_FILES   := $(shell find src -name '*.c' 2>/dev/null)
BIN_FILES := $(shell find assets -name '*.bin' 2>/dev/null)
O_FILES   := $(S_FILES:%=$(BUILD_DIR)/%.o) $(C_FILES:%=$(BUILD_DIR)/%.o) $(BIN_FILES:%=$(BUILD_DIR)/%.o)

.PHONY: all extract check clean distclean progress docs
.DEFAULT_GOAL := check

all: $(ROM)

check: $(ROM)
	@sha1sum -c $(TARGET).sha1

extract:
	rm -rf asm $(BUILD_DIR)/asm $(BUILD_DIR)/src
	$(PYTHON) -m splat split $(TARGET).yaml

$(ROM): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(ELF): $(O_FILES) $(LD_SCRIPT) linker_syms.txt include/hardware_syms.ld
	$(LD) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.s.o: %.s
	@mkdir -p $(@D)
	$(AS) $(ASFLAGS) -o $@ $<

# Pure C goes through KMC's own assembler, like the original build. Files with
# INCLUDE_ASM need modern gas for splat's macros; KMC's assembler encodes move
# as addu, so tools/fix_move.sed makes modern gas do the same.
$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(@D)
	@if grep -q INCLUDE_ASM $<; then \
		echo "$(CC) -S $(CFLAGS) $< (modern gas)"; \
		$(CC) -S $(CFLAGS) -o $(BUILD_DIR)/$*.c.s $< && \
		sed -E -f tools/fix_move.sed -i $(BUILD_DIR)/$*.c.s && \
		$(AS) $(ASFLAGS) -o $@ $(BUILD_DIR)/$*.c.s; \
	else \
		echo "$(CC) -c $(CFLAGS) $<"; \
		$(CC) -c $(CFLAGS) -o $@ $< && python3 tools/elf_trim.py $@; \
	fi

# libc and libm were built with -O1
$(BUILD_DIR)/src/libc/%.c.o: OPTFLAGS := -O1

# Game files built with -ffast-math (sqrt.s without the errno check, division
# by a constant done as a multiply)
FASTMATH := src/auto/5AFC0.c src/auto/5B048.c src/auto/5B098.c src/auto/5B130.c src/auto/5B240.c \
            src/auto/5B2F4.c src/auto/5B38C.c src/auto/5B424.c src/auto/5B4BC.c src/auto/3F090.c \
            src/auto/3A8FC.c src/auto/1224.c src/auto/3B0D8.c src/auto/10F0.c src/auto/1F00.c \
            src/auto/1B9C.c src/auto/12A8.c
$(FASTMATH:%=$(BUILD_DIR)/%.o): OPTFLAGS += -ffast-math

$(BUILD_DIR)/%.bin.o: %.bin
	@mkdir -p $(@D)
	$(LD) -r -b binary -o $@ $<

progress:
	@python3 tools/progress.py

# docs/functions.md and docs/files.md, from the last build
docs: $(ROM)
	python3 tools/gen_docs.py

clean:
	rm -rf $(BUILD_DIR)/asm $(BUILD_DIR)/src $(BUILD_DIR)/assets $(ELF) $(ROM)

distclean:
	rm -rf $(BUILD_DIR) asm assets
