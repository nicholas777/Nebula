#include "kernel/threads.h"
#include "kernel/memory.h"

void thread_init(thread_t *thread) {
    thread->stack = kalloc(THREAD_STACK_SIZE);
    init_thread_stack(thread->stack);
}

