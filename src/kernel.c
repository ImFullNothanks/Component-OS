#include "display.h"
#include "pmm.h"
#include "paging.h"
#include "pci.h"
#include "idt.h"
#include "pic.h"
#include "smbios.h"
#include "krnlshell.h"
#include "multiboot2.h"
#include "fb.h"
#include "pit.h"

extern uint32_t mb2_info;
extern uint64_t _kernel_end;
extern void cli(void);
extern void sti(void);

void kernel_start() {
    pmm_init((uint64_t)(uintptr_t)&_kernel_end, 0x2000000); // Initalize PMM

    struct mb2_framebuffer_tag *fb = mb2_get_framebuffer(mb2_info); // Initalize display backends
    if (fb) {
        fb_init(fb->address, fb->width, fb->height, fb->pitch, fb->bpp);
        display_set_fb();
    } else {
        // fall back to VGA
        display_set_vga();
    }

    paging_remap(); // Page tables remapping
    display_clear();
    display_printstr("Component Kernel Started!\n");

    idt_init(); // Initalize descriptor tables and interrupts
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
