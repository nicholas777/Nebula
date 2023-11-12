#include "kernel/memory.h"

#include <stdint.h>
#include <string.h>

#define MAX_PAGES 131072 // (1024  *1024) / 8

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
                return (i  *8 + j)  *PAGE_SIZE; // The physical address
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

uint32_t *init_vmm() {
    memset(kpage_directory, 0, sizeof(kpage_directory));
    memset(kpage_table_last, 0, sizeof(kpage_table_last));

    kpage_directory[1023] = ((uintptr_t)kpage_table_last - 0xC0000000) | 0x3;

    return kpage_directory;
}

#define MASK_FLAGS 0x3FF

#define PD_OFFSET(addr) (((addr) >> 22) & MASK_FLAGS)
#define PT_OFFSET(addr) (((addr) >> 12) & MASK_FLAGS)

#define PDE(pd, addr) ((uint32_t*)pd)[PD_OFFSET(addr)]

#define ENTRY_PRESENT 0x1
#define PRESENT(entry) (entry & ENTRY_PRESENT)

#define ENTRY_WRITE 0x2
#define ENTRY_RW_PRESENT ENTRY_PRESENT | ENTRY_WRITE

uint32_t address_get_pde(uint32_t addr) {
    return PD_OFFSET(addr);
}

uint32_t address_get_pte(uint32_t addr) {
    return PT_OFFSET(addr);
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

    uint32_t *pt = (uint32_t*)get_page_table_address(address_get_pde(virtual_addr));
    pt[address_get_pte(virtual_addr)] = physical_addr | flags | ENTRY_PRESENT;
    
    return 0;
}

int page_exists(uintptr_t pd, uintptr_t addr) {
    if (!PRESENT(PDE(pd, addr)))
        return 0;

    if (!PRESENT(((uint32_t*)get_page_table_address(address_get_pde(addr)))[address_get_pte(addr)]))
        return 0;

    return 1;
}

uint32_t* get_kernel_pd() {
    return kpage_directory;
}

void switch_page_directory(uintptr_t pd) {
    asm volatile ("movl %0, %%cr3" : : "r"(pd));
}

uint32_t address_get_page(uintptr_t virtual_addr) {
    if (PRESENT(PDE(kpage_directory, virtual_addr)) && 
            ((uint32_t*)get_page_table_address(address_get_pde(virtual_addr)))[address_get_pde(virtual_addr)]) {
        return ((uint32_t*)get_page_table_address(address_get_pde(virtual_addr)))[address_get_pde(virtual_addr)];
    }

    return NULL;
}

void *kalloc_find_page(int start_pde, int start_pte) {
    int pde, pte = -1;

    if (start_pde == 0 && start_pte == 0)
        start_pte++;

    for (pde = start_pde; pde < 1024; pde++) {
        if (!PRESENT(kpage_directory[pde])) {
            uintptr_t addr = allocate_physical_page();
            kpage_directory[pde] = addr | ENTRY_RW_PRESENT;
            map_page_table(pde, addr);

            pte = 0;
            break;
        }

        uint32_t *page_table = (uint32_t*)get_page_table_address(pde);
        for (int i = start_pte; i < 1024; i++) {
            if (PRESENT(page_table[i]))
                continue;

            pte = i;
            break;
        }

        start_pte = 0;

        if (pte != -1)
            break;

    }

    return (void*)get_virtual_addr_from_index(pde, pte);
}

void *kalloc_page_n(uint32_t n) {
    if (n > 1024 || n == 0)
        return NULL;

    int pde = 0, pte = 0;
    uintptr_t page;

    while (1) {
        page = (uintptr_t)kalloc_find_page(pde, pte);

        pde = address_get_pde(page);
        pte = address_get_pte(page);

        for (uint32_t i = 1; i < n; i++) { if (pte + n > 1023) {
                if (!PRESENT(kpage_directory[pde+1])) {
                    uintptr_t addr = allocate_physical_page();
                    kpage_directory[pde + 1] = addr | ENTRY_RW_PRESENT;
                    map_page_table(pde + 1, addr);
                }
            }

            uint32_t pt_entry = ((uint32_t*)get_page_table_address(address_get_pde(
                        page + i  *PAGE_SIZE)))[address_get_pte(page + i  *PAGE_SIZE)];

            if (PRESENT(pt_entry)) {
                pde = -1;
                break;
            }
        }

        if (pde == -1) {
            pde = address_get_pde(page) + 1;
            pte = address_get_pte(page) + 1;
            continue;
        }

        for (uint32_t i = 0; i < n; i++) {
            if (pte + i == 1024) {
                pde++;
                pte -= 1024;
            }

            map_page((uintptr_t)kpage_directory, page + i  *PAGE_SIZE, allocate_physical_page(), 0x3);
        }

        break;
    }

    return (void*)page;
}

void *kalloc_page() {
    return kalloc_page_n(1);
}

void kfree_page(uintptr_t addr) {
    (((uint32_t*)get_page_table_address(address_get_pde(addr)))[address_get_pte(addr)]) = 0;
}


