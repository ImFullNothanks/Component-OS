#include "pci.h"
#include "io.h"
#include "display.h"

uint32_t pci_read(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset) {
    uint32_t addr = (1 << 31)           // enable bit
                  | ((uint32_t)bus  << 16)
                  | ((uint32_t)dev  << 11)
                  | ((uint32_t)func << 8)
                  | (offset & 0xFC);    // must be 4-byte aligned
    outl(PCI_ADDR, addr);
    return inl(PCI_DATA);
}

static uint16_t pci_vendor(uint8_t bus, uint8_t dev, uint8_t func) {
    return pci_read(bus, dev, func, 0x00) & 0xFFFF;
}

static uint16_t pci_device_id(uint8_t bus, uint8_t dev, uint8_t func) {
    return (pci_read(bus, dev, func, 0x00) >> 16) & 0xFFFF;
}

static uint8_t pci_class(uint8_t bus, uint8_t dev, uint8_t func) {
    return (pci_read(bus, dev, func, 0x08) >> 24) & 0xFF;
}

static uint8_t pci_subclass(uint8_t bus, uint8_t dev, uint8_t func) {
    return (pci_read(bus, dev, func, 0x08) >> 16) & 0xFF;
}

static uint8_t pci_header_type(uint8_t bus, uint8_t dev, uint8_t func) {
    return (pci_read(bus, dev, func, 0x0C) >> 16) & 0xFF;
}

void pci_enumerate(void) {
    for (uint16_t bus = 0; bus < 256; bus++) {
        for (uint8_t dev = 0; dev < 32; dev++) {
            // check function 0 first
            if (pci_vendor(bus, dev, 0) == 0xFFFF)
                continue;  // 0xFFFF means no device here

            uint8_t funcs = (pci_header_type(bus, dev, 0) & 0x80) ? 8 : 1;

            for (uint8_t func = 0; func < funcs; func++) {
                uint16_t vid = pci_vendor(bus, dev, func);
                if (vid == 0xFFFF) continue;

                uint8_t class   = pci_class(bus, dev, func);
                uint8_t subcls  = pci_subclass(bus, dev, func);
                uint16_t did    = pci_device_id(bus, dev, func);

                display_printstr("PCI bus:");
                display_printhex(bus);
                display_printstr(" dev:");
                display_printhex(dev);
                display_printstr(" func:");
                display_printhex(func);
                display_printstr(" vendor:");
                display_printhex(vid);
                display_printstr(" device:");
                display_printhex(did);
                display_printstr(" class:");
                display_printhex(class);
                display_printstr(" sub:");
                display_printhex(subcls);
                display_printstr("\n");
            }
        }
    }
}
