#include "kernel/keyboard.h"
#include "kernel/terminal.h"
#include "kernel/multiboot.h"
#include "kernel/memory.h"
#include "kernel/common.h"

#include <stdint.h>
#include <stdio.h>

typedef enum _test_type {
    INVALID = 0,
    TERMINAL, INTERRUPTS, MEMORY_MANAGER
} test_type_t;

#define MAX_TYPE MEMORY_MANAGER

int test_types[] = {
    #define DECLARE_TEST(name, function, type) type,
    #include "tests.h"
    #undef DECLARE_TEST
};

const char *test_names[] = {
    #define DECLARE_TEST(name, funciton, type) #name,
    #include "tests.h"
    #undef DECLARE_TEST
};

#define DECLARE_TEST(name, function, type) extern int function();
#include "tests.h"
#undef DECLARE_TEST

typedef int(*test_fn_t)();

test_fn_t test_functions[] = {
    #define DECLARE_TEST(name, function, type) function,
    #include "tests.h"
    #undef DECLARE_TEST
};

#define ARRAYSIZE(x) sizeof(x) / sizeof(x[0])

extern uint32_t *__kernel_start, __kernel_end;

void init_mm(multiboot_info_t *multiboot, multiboot_memory_map_t *mmap, uint32_t *boot_page_dir) {
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
}

#if defined(__x86__)
extern void setup_gdt();
extern void setup_interrupts_x86();
#endif

void init_sequence(int type, multiboot_info_t *multiboot, multiboot_memory_map_t *mmap, uint32_t *boot_page_dir) {
    switch (type) {
        case TERMINAL:
            terminal_init();
            break;
        case INTERRUPTS:
#if defined(__x86__)
            printf("Initializing GDT and interrupts...\n");
            setup_gdt();
            setup_interrupts_x86();
            init_exception_handlers();
            break;
#endif
        case MEMORY_MANAGER:
            init_mm(multiboot, mmap, boot_page_dir);
    }
}

void ktest(multiboot_info_t *multiboot, multiboot_memory_map_t *mmap, uint32_t *boot_page_dir) {
    int type = 1;
    int is_first = 1;
    int last_index = -1;

    while (1) {
        if (type > MAX_TYPE)
            break;

        int found = 0;

        for (int i = last_index + 1; i < ARRAYSIZE(test_types); i++) {
            if (test_types[i] == INVALID) {
                printf("Invalid test: %s, skipping to next one", test_names[i]);
            }

            else if (test_types[i] == type) {
                found = 1;
                last_index = i;
                break;
            }
        }

        // No tests left in this category, move to next one
        if (found == 0) {
            if (is_first == 1) {
                // No test for this system, init it anyway
                init_sequence(type, multiboot, mmap, boot_page_dir);
            }

            type++;
            is_first = 1;
            last_index = -1;
            continue;
        }

        // If the test is the first in the category we need to initialize the systems necessary
        if (is_first) {
            is_first = 0;
            init_sequence(type, multiboot, mmap, boot_page_dir);
        }

        // Run the test
        test_functions[last_index]();
    }

    init_keyboard();
    asm volatile ("sti");

    while (1)
        ;
}

