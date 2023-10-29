
.globl switch_stack
.type switch_stack, @function

switch_stack:
    pushl %ebp
    movl %esp, %ebp

    leave
    ret

