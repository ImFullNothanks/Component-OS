#include "idt.h"
#include "io.h"
#include "display.h"
#include "pic.h"
#include "krnlerr.h"
#include "kbd.h"
#include "pit.h"

#define IDT_ENTRIES 256
#define IDT_TYPE_INTERRUPT 0x8E // Present bit + ring0 + interrupt gate

static idt_entry_t idt[IDT_ENTRIES];
static idt_ptr_t idt_ptr;

static void idt_set(int n, uint64_t handler) {
    idt[n].offset_low  = handler & 0xFFFF;
    idt[n].selector    = 0x08;
    idt[n].ist         = 0;
    idt[n].irq_attr    = IDT_TYPE_INTERRUPT;
    idt[n].offset_mid  = (handler >> 16) & 0xFFFF;
    idt[n].offset_high = (handler >> 32) & 0xFFFFFFFF;
    idt[n].zero        = 0;
}

// CPU exception handlers (0-31)
extern void isr0(void);   // Divide by zero (#DE)
extern void isr3(void);   // Breakpoint (#BP)
extern void isr8(void);   // Double fault (#DF)
extern void isr13(void);  // General protection fault (#GP)
extern void isr14(void);  // Page fault (#PF)

// IRQ handlers (32-47, after PIC remap)
extern void irq0(void); // Timer
extern void irq1(void); // Keyboard

void idt_init(void) {
    extern void isr_default(void);
    extern void lidt(idt_ptr_t* ptr);
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set(i, (uint64_t)isr_default);
    }

    // CPU exceptions
    idt_set(0, (uint64_t)isr0);
    idt_set(3, (uint64_t)isr3);
    idt_set(8, (uint64_t)isr8);
    idt_set(13, (uint64_t)isr13);
    idt_set(14, (uint64_t)isr14);

    // IRQs remapped to 32+ by PIC
    idt_set(32, (uint64_t)irq0);
    idt_set(33, (uint64_t)irq1);

    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uint64_t)&idt;

    lidt(&idt_ptr);
}

// Pure CPU exceptions use this handler (NO PIC EOI!)
void isr_default_handler(void) {
    // Hang or print exception info here later instead of touching PIC
    display_set_color(display_find_rgb_color(255, 0, 0), 0x000000);
    display_printstr("Exception Happened. Halting.");
    display_set_color(0xFFFFFF, 0x000000);
    for(;;);
}

void cpu_fault_isr_handler(struct cpu_state *state) {
    const char *fault_name = "UNKNOWN CPU EXCEPTION";

    switch (state->irq_no) {
        case 0:  fault_name = "DIVISION BY ZERO (#DE)"; break;
        case 3:  fault_name = "Breakpoint Reached (#BP)"; break;
        case 6:  fault_name = "INVALID OPCODE (#UD)"; break;
        case 8:  fault_name = "DOUBLE FAULT (#DF)"; break;
        case 13: fault_name = "GENERAL PROTECTION FAULT (#GP)"; break;
        case 14: fault_name = "PAGE FAULT (#PF)"; break;
        default: fault_name = "CPU EXCEPTION"; break;
    }

    kernel_panic(fault_name, state);
}

// Timer IRQ handler (IRQ 0)
void irq0_handler(void) {
    pit_handler();
    pic_eoi(0);
}

// Keyboard IRQ handler (IRQ 1)
void irq1_handler(void) {
    uint8_t scancode = inb(0x60);
    kbd_handler(scancode);
    pic_eoi(1); // Send EOI after handling the scancode
}
