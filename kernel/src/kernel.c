#include "kernel/vga.h"
#include "kernel/terminal.h"

void kernel() 
{
	terminal_init();
	terminal_writestring("Hello, World!");
}

