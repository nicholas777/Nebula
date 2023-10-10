#include "kernel/exceptions.h"
#include "kernel/terminal.h"

void general_protection_fault_handler() {
    terminal_writestring("GPF");
}

void page_fault_handler() {
    terminal_writestring("Page fault");
}
