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
	lgdt gdt_desc

	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss
	ljmp $0x08, $continue_processing

continue_processing:

	call terminal_init # So that we can log before kernel()

	lea idt_start, %eax
	push %eax
	
	call generate_idt
	call remap_irqs

	call kernel

// Infinite loop
_halt: 
	cli
	hlt
	jmp _halt
