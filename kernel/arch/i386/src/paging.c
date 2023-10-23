#include "kernel/memory.h"

#include <stdint.h>
#include <string.h>

#define MAX_PAGES 131072 // (1024 * 1024) / 8

uint8_t page_bitmap[MAX_PAGES];

#define FREE 0
#define USED 1
#define PAGE_SIZE 4096

void init_pmm() {
    memset(page_bitmap, 0, 131072);
}

uintptr_t allocate_physical_page() {
    for (size_t i = 0; i < sizeof(page_bitmap); i++) {
        for (size_t j = 0; j < 8; j++) {
            if ((page_bitmap[i] & (1 << j)) == FREE) {
                page_bitmap[i] |= 1 << j;
                return (i * 8 + j) * PAGE_SIZE; // The physical address
            }
        }
    }

    return NULL;
}

void mask_pages(uint32_t start, uint32_t end) {
    if (end <= start || end > MAX_PAGES)
        return;

    uint32_t startByte = start / 8, startBit = start % 8;
    uint32_t endByte = end / 8, endBit = end % 8;

    memset(page_bitmap + startByte, 0b11111111, endByte - startByte);
}

// the parameter page is a physical address.
// No error chekcs are made.
void free_physical_page(uintptr_t addr) {
    size_t page_index = (addr / PAGE_SIZE) / 8;
    size_t page_bit = (addr / PAGE_SIZE) % 8;

    page_bitmap[page_index] &= ~(1 << page_bit);
}

uint32_t kpage_directory[1024] __attribute__((aligned(4096)));

// Used for mapping all the other page tables
uint32_t kpage_table_last[1024] __attribute__((aligned(4096))); 

uint32_t* init_vmm() {
    memset(kpage_directory, 0, sizeof(kpage_directory));
    memset(kpage_table_last, 0, sizeof(kpage_table_last));

    kpage_directory[1023] = ((uintptr_t)kpage_table_last - 0xC0000000) | 0x3;

    return kpage_directory;
}

#define MASK_FLAGS 0x3FF

#define PD_OFFSET(addr) ((addr >> 22) & MASK_FLAGS)
#define PT_OFFSET(addr) ((addr >> 12) & MASK_FLAGS)

#define PDE(pd, addr) ((uint32_t*)pd)[PD_OFFSET(addr)]
#define PTE(pd, addr) ((uint32_t*)(PDE(pd, addr) & (~MASK_FLAGS)))[PT_OFFSET(addr)]

#define ENTRY_PRESENT 0x1
#define PRESENT(entry) (entry & ENTRY_PRESENT)

uint32_t address_get_pde(uint32_t addr) {
    return PD_OFFSET(addr);
}

uint32_t address_get_pte(uint32_t addr) {
    return PT_OFFSET(addr);
}

uint32_t address_get_page(uintptr_t pd, uintptr_t virtual_addr) {
    if (PRESENT(PDE(pd, virtual_addr)) && PRESENT(PTE(pd, virtual_addr))) {
        return PTE(pd, virtual_addr);
    }

    return NULL;
}

int page_table_exists(uint32_t pde_index) {
    return PRESENT(kpage_table_last[pde_index]);
}

void map_page_table(uint32_t index, uint32_t physical) {
    kpage_table_last[index] = physical | 0x3; // Read/write + present
}

// Returns the virtual address to a page table
uint32_t get_page_table_address(uint32_t index) {
    return (1023 << 22) | (index << 12);
}

int map_page(uintptr_t pd, uintptr_t virtual_addr, uint32_t physical_addr, uint16_t flags) {
    if (!PRESENT(PDE(pd, virtual_addr)) 
            && !(PDE(pd, virtual_addr) = allocate_physical_page())) {
        return -1;
    }

    PDE(pd, virtual_addr) |= flags | ENTRY_PRESENT;

    // Map the new page table
    if (!page_table_exists(address_get_pde(virtual_addr))) {
        uint32_t pd_index = address_get_pde(virtual_addr);
        uint32_t pt_addr = PDE(pd, virtual_addr) & (~MASK_FLAGS);

        map_page_table(pd_index, pt_addr);
    }

    uint32_t* pt = (uint32_t*)get_page_table_address(address_get_pde(virtual_addr));
    pt[address_get_pte(virtual_addr)] = physical_addr | flags | ENTRY_PRESENT;
    
    return 0;
}

int page_exists(uintptr_t pd, uintptr_t addr) {
    if (!PRESENT(PDE(pd, addr)) && !PRESENT(PTE(pd, addr)))
        return 0;

    return 1;
}

void switch_page_directory(uintptr_t pd) {
    asm volatile ("movl %0, %%cr3" : : "r"(pd));
}


