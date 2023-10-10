#include "kernel/terminal.h"
#include "kernel/keyboard.h"
#include "kernel/interrupts.h"

#include <stdbool.h>
#include <stdlib.h>

#if defined(__x86__)
extern void setup_gdt();
extern void setup_interrupts_x86();
extern void enable_paging_x86();
#endif

void kernel() {
    terminal_init();

#if defined(__x86__)
    terminal_writestring("Initializing GDT and interrupts...\n");
    setup_gdt();
    setup_interrupts_x86();
    init_exception_handlers();
    //enable_paging_x86();
#endif

    terminal_writestring("Initializing keyboard...\n");
    init_keyboard();

    terminal_writestring("Kernel initialization complete!\n");

    asm volatile ("sti");

    while (true)
        ;

    terminal_writestring("Kernel exit\n");
}

