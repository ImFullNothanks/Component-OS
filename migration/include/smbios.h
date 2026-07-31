#pragma once

#include <stdint.h>
#include <stddef.h>

// SMBIOS 2.1 EPS (32-bit entry point)
struct SMBIOS_EPS {
    char anchor[4];             // "_SM_"
    uint8_t checksum;
    uint8_t length;
    uint8_t major_version;
    uint8_t minor_version;
    uint16_t max_structure_size;
    uint8_t entry_point_revision;
    uint8_t formatted_area[5];
    char intermediate_anchor[5]; // "_DMI_"
    uint8_t intermediate_checksum;
    uint16_t table_length;
    uint32_t table_address;     // Table Address
    uint16_t number_of_structures;
    uint8_t bcd_revision;
} __attribute__((packed));

struct SMBIOS_3_EPS {
    char anchor[5];          // "_SM3_"
    uint8_t checksum;
    uint8_t length;          // Normally 0x18
    uint8_t major_version;
    uint8_t minor_version;
    uint8_t doc_rev;
    uint8_t entry_point_rev;
    uint8_t reserved;
    uint32_t max_structure_size;
    uint64_t table_address;  // <--- 64-bit Physical Address
} __attribute__((packed));

void find_smbios();
void smbios_info(void);

// structure types we'll parse
#define SMBIOS_TYPE_BIOS    0
#define SMBIOS_TYPE_SYSTEM  1
#define SMBIOS_TYPE_END     127

// every smbios structure starts with this header
struct SMBIOS_Header {
    uint8_t  type;
    uint8_t  length;
    uint16_t handle;
} __attribute__((packed));
