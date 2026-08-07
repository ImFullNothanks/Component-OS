#include "smbios.h"
#include "display.h"
#include "string.h"

static const char* smbios_get_string(struct SMBIOS_Header *hdr, uint64_t index) {
    if (index == 0) return "Not Specified";

    // string table starts right after the formatted area
    const char *str = (const char *)hdr + hdr->length;

    // walk throuh strings to find the one at index
    for (uint8_t i = 1; i < index; i++) {
        while (*str) str++;  // skip to end of current string
        str++;               // skip the null terminator
        if (*str == '\0') return "Not Specified";  // hit double null early
    }
    return str;
}

// get pointer to next structure in the table
static struct SMBIOS_Header* smbios_next(struct SMBIOS_Header *hdr) {
    // skip the formatted area
    const char *str = (const char *)hdr + hdr->length;
    // skip the string table (ends with double null)
    while (str[0] != '\0' || str[1] != '\0') str++;
    return (struct SMBIOS_Header *)(str + 2);
}

// BIOS info (type 0)
struct SMBIOS_BIOS {
    struct SMBIOS_Header header;
    uint8_t  vendor;          // string index
    uint8_t  version;         // string index
    uint16_t start_segment;
    uint8_t  release_date;    // string index
    uint8_t  rom_size;
    uint64_t characteristics;
} __attribute__((packed));

// System info (type 1)
struct SMBIOS_System {
    struct SMBIOS_Header header;
    uint8_t manufacturer;    // string index
    uint8_t product_name;    // string index
    uint8_t version;         // string index
    uint8_t serial_number;   // string index
    uint8_t uuid[16];
    uint8_t wakeup_type;
    uint8_t sku_number;      // string index
    uint8_t family;          // string index
} __attribute__((packed));

void find_smbios() {
    for (uintptr_t addr = 0xF0000; addr <= 0xFFFF0; addr += 16) {
        // check for the 64-bit Entry Point (_SM3_)
        if (memcmp((void*)addr, "_SM3_", 5) == 0) {
            struct SMBIOS_3_EPS* eps = (struct SMBIOS_3_EPS*)addr;
            display_printstr("Found SMBIOS 3.0 at: ");
            display_printhex(addr);
            display_printstr("\nTable Address: ");
            display_printhex((uint32_t)eps->table_address);
            display_printchar('\n');
            return;
        }
        
        // checks for the 32-bit Entry Point (_SM_)
        if (memcmp((void*)addr, "_SM_", 4) == 0) {
            struct SMBIOS_EPS* eps = (struct SMBIOS_EPS*)addr;
            display_printstr("Found SMBIOS 2.x at: ");
            display_printhex(addr);
            display_printstr("\nTable Address: ");
            display_printhex(eps->table_address);
            display_printchar('\n');
            return;
        }
    }
}

static void parse_bios(struct SMBIOS_Header *hdr) {
    struct SMBIOS_BIOS *bios = (struct SMBIOS_BIOS *)hdr;
    display_printstr("  Vendor:  ");
    display_printstr(smbios_get_string(hdr, bios->vendor));
    display_printstr("\n  Version: ");
    display_printstr(smbios_get_string(hdr, bios->version));
    display_printstr("\n  Date:    ");
    display_printstr(smbios_get_string(hdr, bios->release_date));
    display_printstr("\n");
}

static void parse_system(struct SMBIOS_Header *hdr) {
    struct SMBIOS_System *sys = (struct SMBIOS_System *)hdr;
    display_printstr("  Manufacturer: ");
    display_printstr(smbios_get_string(hdr, sys->manufacturer));
    display_printstr("\n  Product:      ");
    display_printstr(smbios_get_string(hdr, sys->product_name));
    display_printstr("\n  Version:      ");
    display_printstr(smbios_get_string(hdr, sys->version));
    display_printstr("\n  Serial:       ");
    display_printstr(smbios_get_string(hdr, sys->serial_number));
    display_printstr("\n  Family:       ");
    display_printstr(smbios_get_string(hdr, sys->family));
    display_printstr("\n");
}

void smbios_info(void) {
    // first find the entry point
    uintptr_t table_addr = 0;
    uint16_t  table_len  = 0;

    for (uintptr_t addr = 0xF0000; addr <= 0xFFFF0; addr += 16) {
        if (memcmp((void*)addr, "_SM3_", 5) == 0) {
            struct SMBIOS_3_EPS *eps = (struct SMBIOS_3_EPS *)addr;
            table_addr = (uintptr_t)eps->table_address;
            table_len  = (uint16_t)eps->max_structure_size;
            display_printstr("SMBIOS 3.0 found\n");
            break;
        }
        if (memcmp((void*)addr, "_SM_", 4) == 0) {
            struct SMBIOS_EPS *eps = (struct SMBIOS_EPS *)addr;
            table_addr = eps->table_address;
            table_len  = eps->table_length;
            display_printstr("SMBIOS 2.x found\n");
            break;
        }
    }

    if (table_addr == 0) {
        display_printstr("SMBIOS not found\n");
        return;
    }

    // walk the structure table
    struct SMBIOS_Header *hdr = (struct SMBIOS_Header *)table_addr;
    uintptr_t end = table_addr + table_len;

    while ((uintptr_t)hdr < end && hdr->type != SMBIOS_TYPE_END) {
        switch (hdr->type) {
            case SMBIOS_TYPE_BIOS:
                display_printstr("[BIOS Info]\n");
                parse_bios(hdr);
                break;
            case SMBIOS_TYPE_SYSTEM:
                display_printstr("[System Info]\n");
                parse_system(hdr);
                break;
            default:
                // skip unknown types silently
                break;
        }
        hdr = smbios_next(hdr);
    }
}
