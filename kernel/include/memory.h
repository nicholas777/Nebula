#ifndef _MEMORY_H
#define _MEMORY_H

#include <stddef.h>
#include <stdint.h>

#define PAGE_SIZE 4096

#define get_virtual_addr_from_index(pde, pte) ((uint32_t)(pde << 22) | (uint32_t)(pte << 12))

// Physical memory manager
uintptr_t allocate_physical_page();
void free_physical_page(uintptr_t page);

// This marks pages from start to end as used, only used by the kernel
void mask_pages(uint32_t start, uint32_t end);

void init_pmm();

// Virtual memory manager
uint32_t address_get_page(uintptr_t virtual_addr);
int page_exists(uintptr_t pd, uintptr_t addr);

uint32_t* get_kernel_pd();

// Util
uint32_t address_get_pde(uint32_t virtual_addr);
uint32_t address_get_pte(uint32_t virtual_addr);

// Returns 0 if successful, -1 otherwise
int map_page(uintptr_t pd, uintptr_t virtual_addr, uint32_t physical_addr, uint16_t flags);

uint32_t* init_vmm();
void switch_page_directory(uintptr_t pd);

// NOTE: Probably depricate in favor of map_page() and allocate_physical_page()
// letting the user take care of the virtual memory managment

// Allocates n number of continuous pages. N cannot be larger than 1024
void* kalloc_page_n(uint32_t n);
void* kalloc_page();

void kfree_page(uintptr_t addr);

// Heap
// The heap is represented in memory as a linked list. 

enum heap_block_flags {
    heap_block_available = 1 << 0,
};

#define HEAP_BLOCK_FREE 0
#define HEAP_BLOCK_USED 1

typedef struct _heap_block {
    uint32_t flags;
    uintptr_t data;
    uint32_t size;
    struct _heap_block* next;
    struct _heap_block* prev;
} heap_block_t;

typedef struct {
    uintptr_t start_addr;
    uint32_t size_pages;
    uint32_t used; // In bytes
    heap_block_t* head;
    heap_block_t* tail;
} memory_heap_t;

void heap_init(memory_heap_t* heap, uintptr_t start, uint32_t size_pages);
void* heap_alloc(memory_heap_t* heap, size_t size);
void heap_free(memory_heap_t* heap, void* addr);

void* kalloc(size_t size);
void kfree(void* addr);

void init_kernel_heap(uintptr_t addr, uint32_t size_pages);

#endif
