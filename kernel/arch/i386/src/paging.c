#include "kernel/memory.h"

#include <stdint.h>
#include <string.h>

uint8_t page_bitmap[131072]; // (1024 * 1024) / 8

#define FREE 0
#define USED 1
#define PAGE_SIZE 4096

void init_pmm() {
    memset(page_bitmap, 0, 131072);
}

uintptr_t allocate_physical_page() {
    for (size_t i = 0; i < sizeof(page_bitmap); i++) {
        for (size_t j = 0; j < 8; j++) {
            if ((page_bitmap[i] & ~(1 << j)) == FREE) {
                page_bitmap[i] |= 1 << j;
                return (i * 8 + j) * PAGE_SIZE; // The physical address
            }
        }
    }

    return NULL;
}

// the parameter page is a physical address.
// No error chekcs are made.
void free_physical_page(uintptr_t addr) {
    size_t page_index = (addr / PAGE_SIZE) / 8;
    size_t page_bit = (addr / PAGE_SIZE) % 8;

    page_bitmap[page_index] &= ~(1 << page_bit);
}

uint32_t page_directory[1024];

uint32_t* init_vmm() {
    memset(page_directory, 0, 4 * sizeof(page_directory));
    return page_directory;
}

#define MASK_FLAGS 0x3FF

#define PD_OFFSET(addr) ((addr >> 21) & MASK_FLAGS)
#define PT_OFFSET(addr) ((addr >> 11) & MASK_FLAGS)

#define PDE(pd, addr) ((uint32_t*)pd)[PD_OFFSET(addr)]
#define PTE(pd, addr) ((uint32_t*)(PDE(pd, addr) & MASK_FLAGS))[PT_OFFSET(addr)]

#define ENTRY_PRESENT 0x1
#define PRESENT(entry) (entry & ENTRY_PRESENT)

uint32_t address_get_page(uintptr_t pd, uintptr_t virtual_addr) {
    if (PRESENT(PDE(pd, virtual_addr)) && PRESENT(PTE(pd, virtual_addr))) {
        return PTE(pd, virtual_addr);
    }

    return NULL;
}

int map_page(uintptr_t pd, uintptr_t virtual_addr, uint32_t physical_addr, uint16_t flags) {
    if (!PRESENT(PDE(pd, virtual_addr)) 
            && !(PDE(pd, virtual_addr) = allocate_physical_page())) {
        return -1;
    }

    PDE(pd, virtual_addr) |= flags | ENTRY_PRESENT;

    if (!PRESENT(PTE(pd, virtual_addr))
            && !(PDE(pd, virtual_addr) = allocate_physical_page())) {
        return -1;
    }

    PTE(pd, virtual_addr) |= flags | ENTRY_PRESENT;
    
    return 0;
}


