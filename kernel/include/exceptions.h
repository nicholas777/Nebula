#ifndef __EXCEPTIONS_H
#define __EXCEPTIONS_H

void init_exception_handlers();

void general_protection_fault_handler();
void page_fault_handler();

#endif
