#include "kernel/terminal.h"
#include "kernel/keyboard.h"
#include "kernel/interrupts.h"

#include <stdbool.h>
#include <stdlib.h>

extern void setup_gdt();
extern void setup_interrupts_x86();
extern void enable_paging_x86();

void kernel() 
{
    terminal_init();

#if defined(__x86__)
    setup_gdt();
    setup_interrupts_x86();
    enable_paging_x86();
#endif

    init_keyboard();

    asm volatile ("sti");

    while (true)
        ;


    terminal_writestring("Kernel exit");
}

