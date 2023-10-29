#include "kernel/common.h"
#include "kernel/memory.h"

#include <stdint.h>
#include <stddef.h>

memory_heap_t _kernel_heap;

void heap_init(memory_heap_t* heap, uintptr_t start, uint32_t size_pages) {
    heap->start_addr = start;
    heap->size_pages = size_pages;
    heap->used = 0;
    heap->head = NULL;
}

void heap_add_block(memory_heap_t* heap, heap_block_t* block) {
    if (heap->head == NULL) {
        heap->head = block;
        heap->tail = block;
        heap->used += block->size + sizeof(heap_block_t);
        return;
    }

    if (block->prev == heap->tail || block->next == NULL) {
        heap->tail->next = block;
        block->prev = heap->tail;
        heap->tail = block;
        heap->used += block->size + sizeof(heap_block_t);
        return;
    }

    if (block->next == heap->head || block->prev == NULL) {
        heap->head->prev = block;
        block->next = heap->head;
        heap->head = block;
        heap->used += block->size + sizeof(heap_block_t);
        return;
    }

    block->next = block->prev->next;
    block->prev->next = block;
    heap->used += block->size + sizeof(heap_block_t);
}

#define BLOCK_FREE(x) !(x->flags & heap_block_available)
#define MARK_BLOCK_USED(x) x->flags |= heap_block_available

void heap_tail_block_init(memory_heap_t* heap, heap_block_t* block, size_t size) {
    block->prev = heap->tail;
    block->size = size;
    block->next = NULL;
    block->flags = 0;
    block->data = (uintptr_t)(block + 1);
}

void* heap_alloc(memory_heap_t* heap, size_t size) {
    heap_block_t* block = heap->head;

    while (block != NULL) {
        if (block->size < size || !BLOCK_FREE(block)) {
            if (block == heap->tail) {
                break;
            }
            goto next_block;
        }

        // We found a block
        MARK_BLOCK_USED(block);
        return (void*)block->data;

next_block:
        block = block->next;
    }

    // No good blocks found, so we extend the heap

    if (heap->used + size > heap->size_pages * PAGE_SIZE) {
        kpanic_err("Kernel heap has run out of space");
    }

    block = (heap_block_t*)(heap->start_addr + heap->used);
    heap_tail_block_init(heap, block, size);
    MARK_BLOCK_USED(block);

    heap_add_block(heap, block);
    return (void*)block->data;
}

void heap_free(memory_heap_t* heap, void* addr) {
    heap_block_t* block = ((heap_block_t*)addr) - 1;
    block->flags &= (~heap_block_available);
}

void init_kernel_heap(uintptr_t addr, uint32_t size_pages) {
    heap_init(&_kernel_heap, addr, size_pages);
}

void* kalloc(size_t size) {
    heap_alloc(&_kernel_heap, size);
}

void kfree(void* addr) {
    heap_free(&_kernel_heap, addr);
}

