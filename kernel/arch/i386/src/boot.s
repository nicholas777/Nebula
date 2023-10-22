// Multiboot definitions
.set ALIGN,    0b00000001
.set MEMINFO,  0b00000010             
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002       
.set CHECKSUM, -(MAGIC + FLAGS) 

.section .multiboot.data, "aw"
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

.section .bss
.align 4096

page_directory:
    .skip 4096
page_table1:
    .skip 4096

.section .multiboot.text

.globl _start
.type _start, @function

_start:
    // Save the address to the multiboot structure
    movl %ebx, %esi
    movl %eax, %edi

    // Map the pages of the kernel so that we can run the c code
    movl $0, %eax // Start at memory address 0
    movl $(page_table1 - 0xC0000000), %ebx
    movl $1022, %ecx

1:
    // Skip all addresses before the kernel and all after it
    cmpl $__kernel_start, %eax
    jl 2f
    cmpl $(__kernel_end - 0xC0000000), %eax
    jge 3f

    // Map the page entry
    movl %eax, %edx
    orl $0x3, %edx
    movl %edx, (%ebx)
2:
    addl $4096, %eax
    addl $4, %ebx

    loop 1b
3:
    // Mapping the multiboot info
    movl %ebx, %ecx
    xorl %edx, %edx
    movl %esi, %eax
    movl $4096, %ebx
    divl %ebx // Remainder in %edx

    movl %esi, %eax
    subl %edx, %eax // Get the page boundary
    addl $0x3, %eax // Add the flags

    movl %eax, page_table1 - 0xC0000000 + 1022 * 4

    // Mapping the multiboot memory map
    movl (%esi), %eax
    shrl $6, %eax // Memory map flag is the sixth bit
    andl $1, %eax
    cmpl $1, %eax
    je _no_halt // If the memory map is invalid, halt 
_halt2:
    cli
    hlt
    jmp _halt2

_no_halt:
    movl %esi, %ebx
    addl $48, %ebx
    movl (%ebx), %eax // Eax now contains the address to the memory map

    movl $4096, %ecx
    movl %edx, %edi // Save the remainder for calculating the multiboot info structure's virtual address
    divl %ecx

    movl (%ebx), %eax
    subl %edx, %eax // Get the page boundary
    orl $0x3, %eax // Add the flags

    movl %eax, page_table1 - 0xC0000000 + 1021 * 4

4:
    // Mapping the video RAM to 1024th page table entry
    movl $(0x000B8000 + 0x3), page_table1 - 0xC0000000 + 1023 * 4

    // We map it both in identity mapping and higher half mapping 
    movl $(page_table1 - 0xC0000000 + 0x3), page_directory - 0xC0000000
    movl $(page_table1 - 0xC0000000 + 0x3), page_directory - 0xC0000000 + 768 * 4

    movl $(page_directory - 0xC0000000), %eax
    movl %eax, %cr3

    movl %cr0, %eax
    orl $0x80010000, %eax
    movl %eax, %cr0

    lea 5f, %eax
    jmp *%eax

.section .text

5:
    movl $0, page_directory

    // Reload the page directory without the identity mapping
    movl %cr3, %eax
    movl %eax, %cr3

    movl $stack_top, %esp

    pushl %edx
    cli
    call _init
    popl %edx

    movl $0xC03FE000, %esi
    addl %edi, %esi 

    movl $0xC03FD000, %edi
    addl %edx, %edi

    pushl %edi
    pushl %esi
    call kernel
    addl $8, %esp

    call _fini

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
    ljmp $0x08, $continue_processing

continue_processing:
    movl %ebp, %esp
    popl %ebp
    ret

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

    // call setup_page_directory

    movl %eax, %cr3
    movl %cr0, %eax
    orl %eax, 0x80000001
    movl %eax, %cr0

    movl %ebp, %esp
    popl %ebp
    ret

