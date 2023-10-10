#include "kernel/terminal.h"
#include "kernel/io.h"
#include "kernel/interrupts.h"
#include "kernel/exceptions.h"

#include <stdint.h>
#include <stddef.h>

#define PIC1_CMD 0x20
#define PIC1_DATA 0x21
#define PIC2_CMD 0xA0
#define PIC2_DATA 0xA1

#define PIC_INIT 0x11 // Init and ICW 4
#define PIC_8086_MODE 0x01

// adds the IRQ number to this, ex. IRQ 1 + 32 = 33, and thats the interrupt issued
#define PIC1_OFFSET 0x20 
#define PIC2_OFFSET 0x28

#define KEYBOARD_CONTROLLER 0x64
#define KEYBOARD_OUTPUT 0x60
#define ENABLE_KEYBOARD 0xAE

void setup_keyboard() {
    outb(KEYBOARD_CONTROLLER, ENABLE_KEYBOARD);
    outb(KEYBOARD_CONTROLLER, 0x20); // Use IRQ 1
    inb(KEYBOARD_OUTPUT); // Empties the output buffer
}

// See https://wiki.osdev.org/PIC#Programming_with_the_8259_PIC
// And the official documentation for the PIC 8259
void remap_irqs() {
    outb(PIC1_CMD, PIC_INIT);
    outb(PIC2_CMD, PIC_INIT);

    outb(PIC1_DATA, PIC1_OFFSET);
    outb(PIC2_DATA, PIC2_OFFSET);

    outb(PIC1_DATA, 0b0100); // There is a slave at IRQ 2
    outb(PIC2_DATA, 0b0010); // The master IR is 2

    outb(PIC1_DATA, PIC_8086_MODE);
    outb(PIC2_DATA, PIC_8086_MODE);

    outb(PIC1_DATA, 0xFD);
    outb(PIC2_DATA, 0xFF);

    setup_keyboard();
}

// The ISRs from assembly
extern void asm_isr13(void);
extern void asm_isr14(void);
extern void asm_isr33(void);

typedef struct  {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t offset_high;
} __attribute__((packed)) idt_entry;

typedef struct {
    uint16_t size;
    uint32_t offset;
} __attribute__((packed)) idt_descriptor;

void set_idt_entry(idt_entry* idt, uint32_t num, uint32_t isr_ptr, uint16_t selector, uint8_t flags) {
    idt[num].offset_low = isr_ptr & 0xFFFF;
    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].flags = flags;
    idt[num].offset_high = (isr_ptr >> 16) & 0xFFFF;
}

static idt_entry idt[256];
static idt_descriptor idtd;

static void* interrupt_handlers[256];

void register_interrupt_handler(int handler, void* function) {
    if (handler < 0 || handler > 255)
        return;

    interrupt_handlers[handler] = function;
}

void generate_idt() {
    for (size_t i = 0; i < 256; i++) {
        set_idt_entry(idt, i, 0, 0x0, 0b00001110);
        interrupt_handlers[i] = NULL;
    }

    set_idt_entry(idt, 13, (uint32_t)asm_isr13, 0x8, 0b10001110);
    set_idt_entry(idt, 14, (uint32_t)asm_isr14, 0x8, 0b10001110);
    set_idt_entry(idt, 33, (uint32_t)asm_isr33, 0x8, 0b10001110);

    idtd.size = sizeof(idt_entry) * 256 - 1;
    idtd.offset = (uint32_t)idt;

    asm volatile ("lidt (%0)" : : "r"(&idtd));
}

void int_handler(int isr) {
    if (interrupt_handlers[isr] == NULL)
        return;

    ((isr_t)interrupt_handlers[isr])();

    if (isr >= 0x20 && isr <= 0x30) {
        outb(PIC1_CMD, 0x20); // EOI
        if (isr >= 0x28)
            outb(PIC2_CMD, 0x20); // EOI
    }
}

void init_exception_handlers() {
    register_interrupt_handler(13, general_protection_fault_handler);
    register_interrupt_handler(14, page_fault_handler);
}

