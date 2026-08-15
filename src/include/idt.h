#pragma once
#include <stdint.h>

typedef struct {
    uint16_t offset_low;  // Bits 0-15 of the handler address
    uint16_t selector;    // Kernel code segment selector in GDT
    uint8_t  ist;         // Interrupt stack table, Can be set to 0 if not used.
    uint8_t  irq_attr;    // In type + privilege + present bit
    uint16_t offset_mid;  // Bits 16-31 of the handler address
    uint32_t offset_high; // Bits 32-63 of the handler address
    uint32_t zero;        // Reserved
} __attribute__((packed)) idt_entry_t;

typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) idt_ptr_t;

void idt_init(void);
