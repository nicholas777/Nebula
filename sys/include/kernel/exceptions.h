#ifndef _EXCEPTIONS_H
#define _EXCEPTIONS_H

void init_exception_handlers();

void general_protection_fault_handler();
void page_fault_handler();

#endif
