#ifndef __MEMORY_H
#define __MEMORY_H

#include <stdint.h>

#define PAGE_SIZE 4096

#define get_virtual_addr_from_index(pde, pte) ((pde << 22) | (pte << 12))

// Physical memory manager
uintptr_t allocate_physical_page();
void free_physical_page(uintptr_t page);

// This marks pages from start to end as used, only used by the kernel
void mask_pages(uint32_t start, uint32_t end);

void init_pmm();

// Virtual memory manager
uint32_t address_get_page(uintptr_t pd, uintptr_t virtual_addr);

// Util
uint32_t address_get_pde(uint32_t virtual_addr);
uint32_t address_get_pte(uint32_t virtual_addr);

// Returns 0 if successful, -1 otherwise
int map_page(uintptr_t pd, uintptr_t virtual_addr, uint32_t physical_addr, uint16_t flags);

uint32_t* init_vmm();
void switch_page_directory(uintptr_t pd);

// Heap

typedef struct {

} multiboot_memorymap_t;

typedef struct {
    multiboot_memorymap_t mmap;
} multiboot_t;

#endif
