#include "kernel/terminal.h"
#include "kernel/keyboard.h"
#include "kernel/interrupts.h"
#include "kernel/memory.h"
#include "kernel/multiboot.h"
#include "kernel/common.h"
#include "kernel/threads.h"
#include "kernel/io.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#if defined(__x86__)
extern void setup_gdt();
extern void setup_interrupts_x86();
#endif

extern void *__kernel_start, __kernel_end;

void kmain(multiboot_info_t *multiboot, multiboot_memory_map_t *mmap, uint32_t *boot_page_dir) {
    terminal_init();

    if (init_serial()) {
        kpanic_err("Initialization of serial hardware failed");
    }

#if defined(__x86__)
    printf("Initializing GDT and interrupts...\n");
    setup_gdt();
    setup_interrupts_x86();
    init_exception_handlers();
#endif

    printf("Initializing the memory manager...\n");
    init_pmm();
    mask_pages(0, 8); // Invalidate address 0

#if defined(__x86__)
    // Parsing the multiboot info structure
    if (!(multiboot->flags >> 6 & 0x1))
        kpanic_err("No memory map in multiboot");

    uintptr_t mmap_start = (uintptr_t)mmap;

    for (; (uintptr_t)mmap < mmap_start + multiboot->mmap_length; 
            mmap = (uint8_t*)mmap + mmap->size + sizeof(mmap->size)) {
        if (mmap->type == 1)
            continue;
        
        uint32_t startPage = mmap->addr / 4096;
        uint32_t endPage = (mmap->addr + mmap->len) / 4096;

        mask_pages(startPage, endPage);
    }

#endif

    uint32_t *pd = init_vmm();
    boot_page_dir[1023] = pd[1023]; // For the mappings for the other page tables
    asm volatile ("mov %0, %%cr3" : : "r"((uintptr_t)boot_page_dir - 0xC0000000)); // Reload the paging

    // Mapping the kernel
    uint32_t kernel_size = ((uintptr_t)&__kernel_end - 0xC0000000) - (uintptr_t)&__kernel_start;
    printf("Kernel size: %x\n", kernel_size);

    uint32_t kernel_pages = (kernel_size / PAGE_SIZE) + 1;
    printf("Kernel covers %u pages\n", kernel_pages);

    if (kernel_pages > 1023)
        // This means that it will be overwritted by VGA memory
        kpanic_err("Error: kernel covers more than 1023 pages");

    uint32_t kernel_virtual_addr = (uintptr_t)&__kernel_start + 0xC0000000;

    for (uintptr_t i = &__kernel_start; i < (uintptr_t)&__kernel_end - 0xC0000000; i += PAGE_SIZE) {
        map_page((uintptr_t)pd, kernel_virtual_addr, i, 0x3);
        kernel_virtual_addr += 0x00001000;
    }

    // Video memory
    map_page((uintptr_t)pd, get_virtual_addr_from_index(768, 1023), 0xB8000, 0x3);

    switch_page_directory((uintptr_t)pd - 0xC0000000);

    // The 769th page directory will be reserved for the heap

    for (uintptr_t i = (769 << 22); i < (770 << 22); i += PAGE_SIZE) {
        map_page((uintptr_t)pd, i, allocate_physical_page(), 0x3);
    }

    init_kernel_heap((769 << 22), 1024);

    printf("Initializing keyboard...\n");
    init_keyboard();

    printf("Kernel initialization complete!\n");

    asm volatile ("sti");

    while (true)
        ;

    printf("Kernel exit\n");
}

extern void ktest(multiboot_info_t *multiboot, multiboot_memory_map_t *mmap, uint32_t *boot_page_dir);

void kernel(multiboot_info_t *multiboot, multiboot_memory_map_t *mmap, uint32_t *boot_page_dir) {
#if defined(ENABLE_TESTS)
    ktest(multiboot, mmap, boot_page_dir);
#else
    kmain(multiboot, mmap, boot_page_dir);
#endif
}

