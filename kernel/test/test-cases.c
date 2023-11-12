#include "kernel/memory.h"

#include "common.h"

#include <stdint.h>
#include <stdio.h>

int test_mm() {
    // PMM
    nequals(0, allocate_physical_page());

    uintptr_t ptr = allocate_physical_page();
    free_physical_page(ptr);
    uintptr_t tmp = ptr;

    ptr = allocate_physical_page();
    equals(tmp, ptr);

    // VMM
    uint32_t* pd = get_kernel_pd();
    nequals(pd[1023], 0); // Page tables
    nequals(pd[768], 0); // Kernel
    nequals(pd[769], 0); // Kernel heap

    runs_start("Mapping a page");
    // Assumes that 770 is unused
    map_page((uintptr_t)pd, get_virtual_addr_from_index(770, 0), allocate_physical_page(), 0x3);
    *(int*)get_virtual_addr_from_index(770, 0) = 5;
    runs_end();
    
    runs_start("Heap allocation");
    uint32_t* alloc = (uint32_t*)kalloc(4);
    *alloc = 7;
    runs_end();

    nequals(alloc, 0);

    kfree(alloc);
    uint32_t* tmpaddr = alloc;
    alloc = (uint32_t*)kalloc(4);
    equals(tmpaddr, alloc);

    return success;
}

