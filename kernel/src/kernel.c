#include "kernel/vga.h"
#include "kernel/terminal.h"
#include "kernel/keyboard.h"

#include <stdbool.h>
#include <stdlib.h>

void kernel() 
{
    terminal_writestring("Kernel init");
    init_keyboard();

    asm volatile ("sti");

	while (true)
        ;


    terminal_writestring("Kernel exit");
}

