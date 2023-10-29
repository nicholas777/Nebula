#include <stdint.h>
#include <stddef.h>

#if defined(__IS_LIBK)
#include "kernel/common.h"
#endif

#if UINT32_MAX == UINTPTR_MAX
#define STACK_CHK_GUARD 0x93bc6719
#else
#define STACK_CHK_GUARD 0x1c982753c3767b9d
#endif

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

__attribute__((noreturn))
void __stack_chk_fail(void) {
#if defined(__IS_LIBK)
    kpanic_err("Stack smashing detected");
#else
    while (1)
        ;
}

