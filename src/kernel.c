#include "display.h"
#include "pmm.h"
#include "paging.h"
#include "pci.h"
#include "idt.h"
#include "pic.h"
#include "smbios.h"
#include "krnlshell.h"
#include "multiboot2.h"

extern uint32_t mb2_info;
extern void cli(void);
extern void sti(void);

void kernel_start() {
    idt_init();
    pic_remap();
    pic_mask(0);
    pic_unmask(1);
    sti();

    extern uint64_t _kernel_end;
    pmm_init((uint64_t)(uintptr_t)&_kernel_end, 0x2000000);

    struct mb2_framebuffer_tag *fb = mb2_get_framebuffer(mb2_info);

    fb_init(fb->address, fb->width, fb->height, fb->pitch, fb->bpp);
    if (fb) {
        display_set_fb();
    } else {
        // fall back to VGA
        display_set_vga();
    }
    paging_remap();
    display_clear();
    display_printstr("Component Kernel Started!\n");
    display_printstr("Interrupts Cleared.\n");
    display_printstr("Scanning PCI Bus.\n");
    pci_enumerate();
    find_smbios();
    cmd_init();
    for(;;) __asm__ volatile ("hlt");  // sleep until next interrupt
}
