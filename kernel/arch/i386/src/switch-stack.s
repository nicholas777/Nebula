
.section .text

.globl switch_stack
.type switch_stack, @function

switch_stack:
    pushl %ebp
    pushl %esp
    pushl %eax
    pushl %ebx
    pushl %ecx
    pushl %edx
    pushl %esi
    pushl %edi
    
    movl %esp, 40(%esp)
    movl 36(%esp), %esp

    popl %edi
    popl %esi
    popl %edx
    popl %ecx
    popl %ebx
    popl %eax
    popl %esp
    popl %ebp

    ret

.globl init_thread_stack
.type init_thread_stack, @function

init_thread_stack:
    pushl %ebp
    movl %esp, %ebp

    movl 8(%ebp), %eax

    movl %eax, (%eax)
    addl $4, %eax
    movl %eax, 4(%eax)
    addl $4, %eax

    movl $6, %ecx
register_loop:
    movl $0, (%eax)
    addl $4, %eax
    loop register_loop
    
    leave
    ret

