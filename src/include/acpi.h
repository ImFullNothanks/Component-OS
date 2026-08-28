#pragma once
#include <stdint.h>
#include "string.h"
#include "io.h"
#include "display.h"
#include "multiboot2.h"

struct __attribute__((packed)) rsdp_descriptor {
    char signature[8];       // "RSD PTR "
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t rsdt_address;   // Pointer to RSDT (32-bit physical address)
};

// Standard ACPI header structure shared by all ACPI tables
struct __attribute__((packed)) acpi_header {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
};

// The RSDT header points to an array of 32-bit physical addresses for other tables
struct __attribute__((packed)) rsdt {
    struct acpi_header header;
    uint32_t pointer_to_other_tables[];
};

struct __attribute__((packed)) fadt {
    struct acpi_header h;
    uint32_t firmware_ctrl;
    uint32_t dsdt;

    // Fields up to the PM1a control block address
    uint8_t  res1[20];
    uint32_t pm1a_cnt_blk; // <--- This is the I/O port address we want!
    uint32_t pm1b_cnt_blk;
    // ... (other fields continue down the table)
};

void acpi_init(uint32_t mb2_info);
void acpi_shutdown(void);
