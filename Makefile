ARCH       ?= x86_64
TARGET_DIR  = targets/$(ARCH)

include $(TARGET_DIR)/build.conf

asm_sources  := $(shell find src -name '*.asm')
asm_objects  := $(patsubst src/%.asm, build/%.asm.o, $(asm_sources))

c_sources    := $(shell find src -name '*.c')
c_objects    := $(patsubst src/%.c, build/%.c.o, $(c_sources))

arch_asm_sources := $(shell find $(TARGET_DIR) -name '*.asm')
arch_asm_objects := $(patsubst $(TARGET_DIR)/%.asm, build/%.asm.o, $(arch_asm_sources))

arch_c_sources := $(shell find $(TARGET_DIR) -name '*.c')
arch_c_objects := $(patsubst $(TARGET_DIR)/%.c, build/%.c.o, $(arch_c_sources))

arch_objects := $(arch_asm_objects) $(arch_c_objects)
os_objects   := $(asm_objects) $(c_objects)

build/%.asm.o: src/%.asm
	mkdir -p $(dir $@)
	$(NASM) -f elf64 $< -o $@

build/%.asm.o: $(TARGET_DIR)/%.asm
	mkdir -p $(dir $@)
	$(NASM) -f elf64 $< -o $@

build/%.c.o: $(TARGET_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

build/%.c.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: build-x86_64
build-x86_64: $(os_objects) $(arch_objects)
	mkdir -p dist/x86_64
	$(LD) $(LDFLAGS) -o dist/x86_64/kernel.bin $(os_objects) $(arch_objects)
	cp dist/x86_64/kernel.bin $(TARGET_DIR)/iso/boot/kernel.bin
	grub-mkrescue /usr/lib/grub/i386-pc -o dist/x86_64/kernel.iso $(TARGET_DIR)/iso

.PHONY: clean
clean:
	rm -rf build dist
