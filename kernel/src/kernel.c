#include "kernel/vga.h"
#include "kernel/terminal.h"

#include <stdbool.h>

void kernel() 
{
	terminal_writestring("Hello, World!\n");
	terminal_writestring("Hello, Again!");

	while (true)
		;
}

