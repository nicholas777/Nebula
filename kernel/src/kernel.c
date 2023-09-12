#include "kernel/vga.h"
#include "kernel/terminal.h"

#include <stdbool.h>
#include <stdlib.h>

void kernel() 
{
	terminal_writestring("Hello, World!\n");
	terminal_writestring("Hello, Again!");

    char buf[20];

    terminal_writestring(itoa(8, buf, 2));

    terminal_writechar('\n');
    terminal_writechar('\n');
	while (true)
        ;


    terminal_writestring("Kernel exit");
}

