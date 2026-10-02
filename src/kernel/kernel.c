#include <kernel/display.h>
#include <kernel/pmm.h>
#include <x86_64/paging.h>
#include <x86_64/pci.h>
#include <x86_64/idt.h>
#include <x86_64/pic.h>
#include <x86_64/smbios.h>
#include <kernel/string.h>
#include <kernel/krnlshell.h>
#include <kernel/multiboot2.h>
#include <x86_64/mb2_mmap.h>
#include <x86_64/fb.h>
#include <x86_64/acpi.h>
#include <x86_64/pit.h>

extern uint32_t mb2_info;
extern uint64_t _kernel_end;
extern void cli(void);
extern void sti(void);

void kernel_start() {
    pmm_init((uint64_t)(uintptr_t)&_kernel_end, 0x2000000); // Initalize PMM

    #define MB2_TAG_FRAMEBUFFER 8
    struct mb2_framebuffer_tag *fb = (struct mb2_framebuffer_tag *)mb2_get_tag(mb2_info, MB2_TAG_FRAMEBUFFER);
    if (fb) {
        fb_init(fb->address, fb->width, fb->height, fb->pitch, fb->bpp);
        display_set_fb();
    } else {
        // fall back to VGA
        display_set_vga();
    }

    uint64_t total_ram_bytes = get_total_memory(mb2_info);
    uint64_t total_ram_mb = total_ram_bytes / (1024 * 1024);

    char total_ram_string[32];
    itoa(total_ram_mb, total_ram_string, 10);

    display_clear();
    display_printstr("Component Kernel Started!\n");
    paging_remap(); // Page Tables Remapping

    display_printstr("Total Usable RAM: ");
    display_printstr(total_ram_string);
    display_printstr(" MB\n");

    idt_init(); // Initalize descriptor tables and interrupts
    acpi_init(mb2_info);
    pic_remap();
    pit_init(100);

    display_printstr("Scanning PCI Bus.\n"); // Hardware Discovery and Command Processor
    pci_enumerate();
    find_smbios();
    cmd_init();

    pic_unmask(0); // Enable interrupts
    pic_unmask(1);
    sti();
    for(;;) __asm__ volatile ("hlt");  // sleep until next interrupt
}
