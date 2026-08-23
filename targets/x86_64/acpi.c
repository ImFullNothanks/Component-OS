#include "acpi.h"
#include "string.h"
#include <stdint.h>

struct rsdp_descriptor* find_rsdp(void) {
    for (uintptr_t addr = 0xE0000; addr <= 0xFFFFF; addr += 16) {
        struct rsdp_descriptor *rsdp = (struct rsdp_descriptor *)addr;

        // Use strncmp to check the 8-byte signature cleanly
        if (strncmp(rsdp->signature, "RSD PTR ", 8) == 0) {

            // Verify the checksum for ACPI version 1.0 (first 20 bytes)
            uint8_t sum = 0;
            uint8_t *byte_ptr = (uint8_t *)rsdp;
            for (int i = 0; i < 20; i++) {
                sum += byte_ptr[i];
            }

            if (sum == 0) {
                return rsdp; // Valid RSDP found!
            }
        }
    }

    return 0; // Not found
}

// Generic function to find a specific ACPI table by its 4-character signature (e.g., "FACP")
void* find_acpi_table(struct rsdp_descriptor *rsdp, const char *signature) {
    // 1. Get the RSDT physical address from the RSDP
    struct rsdt *rsdt = (struct rsdt *)(uintptr_t)rsdp->rsdt_address;

    // 2. Calculate how many table pointers are inside the RSDT
    // Total table length minus the header size gives us the size of the array of pointers,
    // and each pointer is 4 bytes (uint32_t).
    int entries = (rsdt->header.length - sizeof(struct acpi_header)) / 4;

    // 3. Loop through every pointer to look for the table signature we want
    for (int i = 0; i < entries; i++) {
        struct acpi_header *table = (struct acpi_header *)(uintptr_t)rsdt->pointer_to_other_tables[i];

        // Check if this table's signature matches what we're looking for (e.g., "FACP")
        if (strncmp(table->signature, signature, 4) == 0) {
            return table; // Found it!
        }
    }

    return 0; // Table not found
}

void* table = find_acpi_table(rsdp, "FACP");
if (table != 0) {
    struct fadt *fadt = (struct fadt *)table;

    // Extract the 16-bit I/O port address for PM1a Control Block
    uint16_t pm1a_cnt = (uint16_t)fadt->pm1a_cnt_blk;

    // (Optional) If pm1b_cnt_blk is also present, you might use it too,
    // but pm1a is usually enough for basic shutdown.
}

void acpi_init(void) {
    display_printstr("Initializing ACPI...\n");

    // Step 1: Hunt for the RSDP structure in the BIOS area
    struct rsdp_descriptor *rsdp = find_rsdp();
    if (!rsdp) {
        display_printstr("ACPI Error: RSDP not found!\n");
        return;
    }
    display_printstr("ACPI: RSDP found.\n");

    // Step 2: Use the RSDP to find the FADT table ("FACP") via the RSDT
    struct fadt *fadt = (struct fadt *)find_acpi_table(rsdp, "FACP");
    if (!fadt) {
        display_printstr("ACPI Error: FADT (FACP) table not found!\n");
        return;
    }
    display_printstr("ACPI: FADT table found.\n");

    // Step 3: Extract the PM1a Control Block I/O port address
    pm1a_cnt_port = (uint16_t)fadt->pm1a_cnt_blk;

    display_printstr("ACPI: Power management port initialized successfully.\n");
}

void acpi_shutdown(void) {
    outw(pm1a_cnt_port, 0x2000);
}
