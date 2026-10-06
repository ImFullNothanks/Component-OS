ARCH       ?= x86_64
TARGET_DIR  = targets/$(ARCH)

include $(TARGET_DIR)/build.conf

# --- Source and Object Definitions ---
kernel_asm_sources  := $(shell find src/kernel -name '*.asm')
kernel_asm_objects  := $(patsubst src/%.asm, build/%.asm.o, $(kernel_asm_sources))

kernel_c_sources    := $(shell find src/kernel -name '*.c')
kernel_c_objects    := $(patsubst src/%.c, build/%.c.o, $(kernel_c_sources))

arch_asm_sources := $(shell find $(TARGET_DIR) -name '*.asm')
arch_asm_objects := $(patsubst $(TARGET_DIR)/%.asm, build/$(ARCH)/%.asm.o, $(arch_asm_sources))

arch_c_sources := $(shell find $(TARGET_DIR) -name '*.c')
arch_c_objects := $(patsubst $(TARGET_DIR)/%.c, build/$(ARCH)/%.c.o, $(arch_c_sources))

arch_objects := $(arch_asm_objects) $(arch_c_objects)
os_objects   := $(kernel_asm_objects) $(kernel_c_objects)

# --- Determine GRUB Platform Directory ---
ifeq ($(ARCH),x86_64)
    GRUB_PLATFORM_DIR = /usr/lib/grub/i386-pc
else ifeq ($(ARCH),aarch64-generic)
    GRUB_PLATFORM_DIR = /usr/lib/grub/arm64-efi
else
    GRUB_PLATFORM_DIR =
endif

# --- Compilation Rules ---

# Core rules
build/kernel/%.asm.o: src/kernel/%.asm
	mkdir -p $(dir $@)
	$(NASM) $(ASMFLAGS) $< -o $@

build/kernel/%.c.o: src/kernel/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Architecture-specific rules
build/$(ARCH)/%.asm.o: $(TARGET_DIR)/%.asm
	mkdir -p $(dir $@)
	$(NASM) $(ASMFLAGS) $< -o $@

build/$(ARCH)/%.c.o: $(TARGET_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# --- Modular Target Operations ---

.PHONY: all
all: kernel-bin make-bootable

# Compiles sources and links them into the final raw kernel binary
.PHONY: kernel-bin
kernel-bin: $(os_objects) $(arch_objects)
	mkdir -p dist/$(ARCH)
	$(LD) $(LDFLAGS) -o dist/$(ARCH)/kernel.bin $(os_objects) $(arch_objects)

# Takes the compiled kernel binary and packages it into a bootable medium
.PHONY: make-bootable
make-bootable:
	mkdir -p $(TARGET_DIR)/iso/boot
	cp dist/$(ARCH)/kernel.bin $(TARGET_DIR)/iso/boot/kernel.bin
	@if [ "$(ARCH)" = "aarch64-generic" ]; then \
		echo "Packaging AArch64 bootable image..."; \
		grub-mkrescue $(if $(GRUB_PLATFORM_DIR),-d $(GRUB_PLATFORM_DIR)) -o dist/$(ARCH)/kernel.iso $(TARGET_DIR)/iso; \
	else \
		echo "Packaging standard x86 bootable ISO..."; \
		grub-mkrescue $(if $(GRUB_PLATFORM_DIR),-d $(GRUB_PLATFORM_DIR)) -o dist/$(ARCH)/kernel.iso $(TARGET_DIR)/iso; \
	fi

.PHONY: clean
clean:
	rm -rf build dist
