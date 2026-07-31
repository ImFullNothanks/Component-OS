#include "pic.h"
#include "io.h"

// small delay needed between PIC commands on real hardware
static void io_wait(void) {
    outb(0x80, 0);  // write to unused port
}

void pic_remap(void) {
    // save masks
    uint8_t mask1 = inb(PIC_MASTER_DATA);
    uint8_t mask2 = inb(PIC_SLAVE_DATA);

    // ICW1 — start init sequence
    outb(PIC_MASTER_CMD, 0x11); io_wait();
    outb(PIC_SLAVE_CMD,  0x11); io_wait();

    // ICW2 — set vector offsets
    outb(PIC_MASTER_DATA, 0x20); io_wait();  // master → INT 32-39
    outb(PIC_SLAVE_DATA,  0x28); io_wait();  // slave  → INT 40-47

    // ICW3 — tell master/slave about each other
    outb(PIC_MASTER_DATA, 0x04); io_wait();  // master: slave on IRQ2
    outb(PIC_SLAVE_DATA,  0x02); io_wait();  // slave: cascade identity

    // ICW4 — 8086 mode
    outb(PIC_MASTER_DATA, 0x01); io_wait();
    outb(PIC_SLAVE_DATA,  0x01); io_wait();

    // restore masks
    outb(PIC_MASTER_DATA, mask1);
    outb(PIC_SLAVE_DATA,  mask2);
}

void pic_eoi(uint8_t irq) {
    if (irq >= 8)
        outb(PIC_SLAVE_CMD, PIC_EOI);   // slave needs EOI too
    outb(PIC_MASTER_CMD, PIC_EOI);
}

void pic_mask(uint8_t irq) {
    uint16_t port = (irq < 8) ? PIC_MASTER_DATA : PIC_SLAVE_DATA;
    uint8_t bit   = irq % 8;
    outb(port, inb(port) | (1 << bit));
}

void pic_unmask(uint8_t irq) {
    uint16_t port = (irq < 8) ? PIC_MASTER_DATA : PIC_SLAVE_DATA;
    uint8_t bit   = irq % 8;
    outb(port, inb(port) & ~(1 << bit));
}
