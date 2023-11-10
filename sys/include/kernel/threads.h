#ifndef _THREADS_H
#define _THREADS_H

#include <stdint.h>

#define THREAD_STACK_SIZE 16384

typedef struct {
    uint32_t *stack;
} thread_t;

void switch_stack(uint32_t *new_stack, uint32_t **old_stack);
void init_thread_stack(uint32_t *stack);

void thread_init(thread_t *thread);

#endif
