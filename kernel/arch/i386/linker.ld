ENTRY(_start)
 
SECTIONS {
    . = 0x00100000;

    __kernel_start = .;
    .multiboot : {
        *(.multiboot.data)
        *(.multiboot.text)
    }

    . += 0xC0000000;
    .text ALIGN(4K) : AT (ADDR (.text) - 0xC0000000) {
        *(.text)
    }

    .rodata ALIGN(4K) : AT (ADDR (.rodata) - 0xC0000000) {
        *(.rodata)
    }
 
    .data ALIGN(4K) : AT(ADDR (.data) - 0xC0000000) {
        *(.data)
    }
 
    .bss ALIGN(4K) : AT(ADDR (.bss) - 0xC0000000) {
        *(COMMON)
        *(.bss)
    }

    __kernel_end = .;
}

