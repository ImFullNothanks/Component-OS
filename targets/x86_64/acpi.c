#include "acpi.h"
#include "string.h"
#include "io.h"
#include "display.h"
#include "multiboot2.h"
#include <stdint.h>

uint16_t pm1a_cnt_port;
uint16_t slp_typ_a = 0x2000;

struct rsdp_descriptor* find_rsdp(uint32_t mb2_info) {
    // Try to find ACPI v2 (tag type 15) first
    struct mb2_tag *acpi_tag = (struct mb2_tag *)mb2_get_tag(mb2_info, 15);
    if (!acpi_tag) {
        // Fallback to ACPI v1 (tag type 14 / 0xE)
        acpi_tag = (struct mb2_tag *)mb2_get_tag(mb2_info, 14);
    }

    if (!acpi_tag) {
        return 0; // Neither tag was provided by GRUB
    }

    return (struct rsdp_descriptor *)((uint8_t *)acpi_tag + 8);
}

// Generic function to find a specific ACPI table by its 4-character signature (e.g., "FACP")
void* find_acpi_table(struct rsdp_descriptor *rsdp, const char *signature) {
    if (rsdp->rsdt_address == 0) {
        display_printstr("ACPI Error: RSDT address is zero!\n");
        return 0;
    }
    display_printstr("ACPI: Found RSDT Table At:");
    display_printhex(rsdp->rsdt_address);
    display_printchar('\n');
    if (rsdp->rsdt_address > 0xF0000000) {
        return 0;
    }
    // 1. Get the RSDT physical address from the RSDP
    struct rsdt *rsdt = (struct rsdt *)(uintptr_t)rsdp->rsdt_address;

    if (memcmp(rsdt->header.signature, "RSDT", 4) != 0) {
        display_printstr("ACPI Error: RSDT signature mismatch!\n");
        return 0;
    }

    // 2. Calculate how many table pointers are inside the RSDT
    // Total table length minus the header size gives us the size of the array of pointers,
    // and each pointer is 4 bytes (uint32_t).
    int entries = (rsdt->header.length - sizeof(struct acpi_header)) / 4;

    // 3. Loop through every pointer to look for the table signature we want
    for (int i = 0; i < entries; i++) {
        struct acpi_header *table = (struct acpi_header *)(uintptr_t)rsdt->pointer_to_other_tables[i];

        // Check if this table's signature matches what we're looking for (e.g., "FACP")
        if (table != 0 && memcmp(table->signature, signature, 4) == 0) {
            return table; // Found it!
        }
    }

    return 0; // Table not found
}

void acpi_init(uint32_t mb2_info) {
    display_printstr("Initializing ACPI...\n");

    // Step 1: Hunt for the RSDP structure in the BIOS area
    struct rsdp_descriptor *rsdp = find_rsdp(mb2_info);
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
