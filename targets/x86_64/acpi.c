#include "acpi.h"
#include "string.h"
#include "io.h"
#include "paging.h"
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

    uint64_t rsdt_phys = rsdp->rsdt_address;

    display_printstr("ACPI: Found RSDT Table At: ");
    display_printhex(rsdt_phys);
    display_printchar('\n');

    // 1. Map the first page (4096 bytes) of the RSDT so we can safely read its header
    // Flags: 0x03 = Present | Read/Write
    vmm_map_range(rsdt_phys, rsdt_phys, 4096, PAGE_PRESENT | PAGE_WRITE);

    struct rsdt *rsdt = (struct rsdt *)(uintptr_t)rsdt_phys;

    if (memcmp(rsdt->header.signature, "RSDT", 4) != 0) {
        display_printstr("ACPI Error: RSDT signature mismatch!\n");
        return 0;
    }

    // 2. Now that we read the header, map the *entire* RSDT table based on its actual length
    uint32_t rsdt_length = rsdt->header.length;
    vmm_map_range(rsdt_phys, rsdt_phys, rsdt_length, PAGE_PRESENT | PAGE_WRITE);

    // 3. Calculate how many table pointers are inside the RSDT
    int entries = (rsdt_length - sizeof(struct acpi_header)) / 4;

    // 4. Loop through every pointer to look for the target table signature
    for (int i = 0; i < entries; i++) {
        uint64_t table_phys = rsdt->pointer_to_other_tables[i];
        if (table_phys == 0) continue;

        // Map the header of this sub-table first to read its length
        vmm_map_range(table_phys, table_phys, 4096, PAGE_PRESENT | PAGE_WRITE);
        struct acpi_header *table = (struct acpi_header *)(uintptr_t)table_phys;

        // Map the full sub-table based on its specific length
        vmm_map_range(table_phys, table_phys, table->length, PAGE_PRESENT | PAGE_WRITE);

        // Check if this table's signature matches what we want (e.g., "FACP")
        if (memcmp(table->signature, signature, 4) == 0) {
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
