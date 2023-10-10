
ARCH := $(shell ../arch.sh)
ARCH_DIR := arch/$(ARCH)

GNU_ARCH := $(shell ../compiler-arch.sh)

BIN_DIR := ../bin

CC := $(GNU_ARCH)-gcc
LD := $(GNU_ARCH)-ld
AS := $(GNU_ARCH)-as

CFLAGS := -std=gnu99 -ffreestanding -Wall -Wextra -O2 
CPPFLAGS := 
LDFLAGS := -ffreestanding -lk -nostdlib -lgcc -n -T $(ARCH_DIR)/linker.ld

SYSTEM_DIR := ../sys
INCLUDE_DIR := $(SYSTEM_DIR)/include
LIB_DIR := $(SYSTEM_DIR)/lib
CPPFLAGS := $(CPPFLAGS) -I$(INCLUDE_DIR)
LDFLAGS := $(LDFLAGS) --sysroot=$(SYSTEM_DIR)
CFLAGS := $(CFLAGS) --sysroot=$(SYSTEM_DIR) -isystem=/include/libc

include $(ARCH_DIR)/make.cfg

CFLAGS := $(CFLAGS) $(ARCH_CFLAGS)
CPPFLAGS := $(CPPFLAGS) $(ARCH_CPPFLAGS)
LDFLAGS := $(LDFLAGS) $(ARCH_LDFLAGS)

KERNEL_OBJS := $(ARCH_OBJS) src/kernel.o src/vga.o src/terminal.o \
			   src/keyboard.o src/exceptions.o
OBJ_FILES := \
	$(ARCH_DIR)/src/crti.o $(ARCH_DIR)/src/crtbegin.o \
	$(KERNEL_OBJS) \
	$(ARCH_DIR)/src/crtend.o $(ARCH_DIR)/src/crtn.o

HEADERS := $(wildcard include/*.h)

all: install-headers $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJ_FILES) $(LDFLAGS) -o $(BIN_DIR)/kernel.bin
	@echo "Copying the kernel file..."
	@mkdir -p $(LIB_DIR)/kernel
	@cp $(BIN_DIR)/kernel.bin $(LIB_DIR)/kernel

	@echo "Creating iso image..."
	@rm -f $(BIN_DIR)/nebula.iso
	@mkdir -p $(BIN_DIR)/boot/grub
	@mv $(BIN_DIR)/kernel.bin $(BIN_DIR)/boot
	@echo "menuentry NebulaOS {\n\tmultiboot /boot/kernel.bin\n}" > $(BIN_DIR)/boot/grub/grub.cfg
	@grub-mkrescue -o $(BIN_DIR)/nebula.iso $(BIN_DIR)


install-headers:
	@echo "Installing the system headers"
	@mkdir -p $(INCLUDE_DIR)/kernel
	@cp $(HEADERS) $(INCLUDE_DIR)/kernel

$(ARCH_DIR)/src/crtbegin.o $(ARCH_DIR)/src/crtend.o:
	OBJ=`$(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=$(@F)` && cp "$$OBJ" $@

%.o: %.c
	$(CC) $(CFLAGS) -std=gnu11 $(CPPFLAGS) -c $< -o $@

%.o: %.s
	$(AS) $< -o $@

clean:
	@rm src/*.o
	@rm arch/*/src/*.o

