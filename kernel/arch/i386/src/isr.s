.extern int_handler

.section .text

.globl asm_isr13 # Generat protection fault
asm_isr13:
    pushal
    
    pushl $13
    call int_handler
    addl $4, %esp

    popal
    sti
    iret

.globl asm_isr14 # Paging fault
asm_isr14:
    pushal
    
    pushl $14
    call int_handler
    addl $4, %esp

    popal
    sti
    iret

.globl asm_isr33 # Keyboard
asm_isr33:
    pushal

    pushl $33
    call int_handler
    addl $4, %esp

    popal
    sti

    iret

