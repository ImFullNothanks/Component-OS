#pragma once
#include <stdint.h>

#define PCI_ADDR 0xCF8
#define PCI_DATA 0xCFC

// class codes for common devices
#define PCI_CLASS_KEYBOARD   0x09  // input devices
#define PCI_CLASS_STORAGE    0x01
#define PCI_CLASS_NETWORK    0x02
#define PCI_CLASS_DISPLAY    0x03
#define PCI_CLASS_BRIDGE     0x06

typedef struct {
    uint8_t  bus;
    uint8_t  device;
    uint8_t  function;
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t  class_code;
    uint8_t  subclass;
} pci_device_t;

uint32_t pci_read(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset);
void pci_enumerate(void);
