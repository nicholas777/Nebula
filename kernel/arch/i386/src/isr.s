.extern int13_handler
.extern int32_handler
.extern terminal_writestring

.section .text

.globl asm_isr13 # Generat protection fault
asm_isr13:
    pushal
    
    call int13_handler

    popal
    sti
    iret

.globl asm_isr33
.align 4

asm_isr33:
    pushal

    call int33_handler

    popal
    sti

    iret

