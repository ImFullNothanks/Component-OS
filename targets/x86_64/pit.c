#include "pit.h"
#include "pic.h"
#include "io.h"
#include "display.h"
#include <stdint.h>

uint32_t tick_count = 0;

void pit_init(uint32_t frequency) {
    // The divisor must fit into 16 bits (max 65535)
    uint32_t divisor = 1193180 / frequency;

    // Send the command byte:
    // Channel 0, Access mode: low/high byte, Mode 3 (square wave generator), Binary 16-bit
    outb(0x43, 0x36);
    io_wait();

    // Send the divisor byte-by-byte (low byte, then high byte)
    outb(0x40, (uint8_t)(divisor & 0xFF));
    io_wait();
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
    io_wait();
}

void pit_handler(void) {
    tick_count++;
    //if (tick_count % 100 == 0) {
    //    display_printstr("1 second passed\n");
    //}
}

uint32_t get_pit_tick_count(void) {
    return tick_count;
}

uint32_t pit_kernel_uptime_seconds(void) {
    // divide by 100 because in kernel.c pit is set to wake up every 1 ms
    return tick_count / 100;
}
