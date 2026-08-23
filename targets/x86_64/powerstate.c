#include "powerstate.h"
#include "io.h"
#include "display.h"
#include "acpi.h"

void system_reboot(void) {
    uint8_t temp = 0x02;

    // Wait for the keyboard controller input buffer to be clear
    while (temp & 0x02) {
        temp = inb(0x64);
    }

    // Send the pulse reset command to the keyboard controller
    outb(0x64, 0xFE);

    // Halt the CPU just in case it takes a moment to reset
    for (;;) {
        asm volatile ("cli; hlt");
    }
}

void system_shutdown(void) {
    // 1. Try QEMU / Bochs specific shutdown magic ports first
    // Modern QEMU uses port 0x604 with value 0x2000
    outw(0x604, 0x2000);
    outw(0xB004, 0x2000);

    acpi_shutdown();

    display_printstr("Shutdown failed. It is now safe to turn off your computer.\n");
    for (;;) {
        asm volatile ("cli; hlt");
    }
}
