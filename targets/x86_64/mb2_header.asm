section .multiboot_header
header_start:
    dd 0xe85250d6                ; Magic number (multiboot 2)
    dd 0                         ; Architecture 0 (protected mode i386)
    dd header_end - header_start ; Header length
    ; Checksum
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    align 8
    ;framebuffer tag
    dw 5    ; type = framebuffer
    dw 0    ; flags
    dd 20   ; size
    dd 1024    ; width 0 = Up For Grub to decide
    dd 768    ; height 0 also means Up For Grub to decide
    dd 32   ; depth (32bpp)

    align 8
    ; End tag
    dw 0
    dw 0
    dd 8
header_end:
