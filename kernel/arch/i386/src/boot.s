// Multiboot definitions
.set ALIGN,    1<<0             
.set MEMINFO,  1<<1             
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

.section .text

.globl _start
.type _start, @function

_start:
	movl $stack_top, %esp

	call kernel

// Infinite loop
_halt: 
	cli
	hlt
	jmp _halt








