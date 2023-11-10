#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

typedef void (*isr_t)(void);

void register_interrupt_handler(int handler, void* function);

#endif
