// Multiboot definitions
.set ALIGN,    1
.set MEMINFO,  0b10             
.set FLAGS,    ALIGN | MEMINFO  
.set MAGIC,    0x1BADB002       
.set CHECKSUM, -(MAGIC + FLAGS) 

.section .multiboot
.align 4 // Multiboot has to be aligned by 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .bss

// Declaring the stack
.align 16 // Stack on x86 has to be 16 aligned
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .data

.align 8
idt_start:
.rept 256
.word 0x0, 0x0, 0x0, 0x0
.endr

idt_end:

idt_desc:
.word idt_start - idt_end - 1
.long idt_start

gdt_start:

gdt_null:
    .long 0
    .long 0

gdt_code:
    .word 0xFFFF # segment limiter
    .word 0x0000 # base
    .byte 0x00 # base
    .byte 0b10011010
    .byte 0b11001111
    .byte 0x00 # base

gdt_data:
    .word 0xFFFF # segment limiter
    .word 0x0000 # base
    .byte 0x00 # base
    .byte 0b10010010
    .byte 0b11001111
    .byte 0x00 # base

gdt_end:

gdt_desc:
.word gdt_start - gdt_end - 1
.long gdt_start

.section .text

.globl _start
.type _start, @function

_start:
    movl $stack_top, %esp
    cli
    call kernel

// Infinite loop
_halt: 
    cli
    hlt
    jmp _halt

.type setup_gdt, @function
.globl setup_gdt
setup_gdt:
    pushl %ebp
    movl %esp, %ebp

    lgdt gdt_desc

    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss
    ljmp $0x08, $far_jump

continue_processing:
    movl %ebp, %esp
    popl %ebp
    ret

far_jump:
    jmp continue_processing

.type setup_interrupts_x86, @function
.globl setup_interrupts_x86
setup_interrupts_x86:
    pushl %ebp
    movl %esp, %ebp

    call generate_idt
    call remap_irqs

    movl %ebp, %esp
    popl %ebp
    ret

.type enable_paging_x86, @function
.globl enable_paging_x86
enable_paging_x86:
    pushl %ebp
    movl %esp, %ebp

    call setup_page_directory

    movl %eax, %cr3
    movl %cr0, %eax
    orl %eax, 0x80000001
    movl %eax, %cr0

    movl %ebp, %esp
    popl %ebp
    ret

