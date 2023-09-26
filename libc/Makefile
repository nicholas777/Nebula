
ARCH := $(shell ../arch.sh)

GNU_ARCH := $(shell ../compiler-arch.sh)

CC := $(GNU_ARCH)-gcc
LD := $(GNU_ARCH)-ld
AS := $(GNU_ARCH)-as
AR := $(GNU_ARCH)-ar

CFLAGS_LIBK := -ffreestanding --std=gnu11 -Wall -Wextra -O2 
CPPFLAGS_LIBK := -D_IS__LIBK -Iinclude

SYSTEM_DIR := ../sys
INCLUDE_DIR := $(SYSTEM_DIR)/include
LIBC_LIBK_DIR := $(SYSTEM_DIR)/lib
CPPFLAGS_LIBK := $(CPPFLAGS_LIBK) -I$(INCLUDE_DIR)

HEADERS := $(wildcard include/*.h)
OBJ_FILES := src/string.o src/stdlib.o

all: install-headers all-libk

all-libk: $(OBJ_FILES)
	@mkdir -p $(LIBC_LIBK_DIR)
	$(AR) rcs $(LIBC_LIBK_DIR)/libk.a $(OBJ_FILES)

%.o: %.c
	$(CC) $(CFLAGS_LIBK) $(CPPFLAGS_LIBK) -c $< -o $@


install-headers:
	@echo "Installing the system headers"
	@mkdir -p $(INCLUDE_DIR)/libc
	@cp $(HEADERS) $(INCLUDE_DIR)/libc




