section .multiboot_header
header_start:
    align 8
    dd 0xe85250d6                ; Magic number (multiboot 2)
    dd 0                         ; Architecture 0 (protected mode i386)
    dd header_end - header_start ; Header length
    ; Checksum
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    align 8
    ;framebuffer tag
    dw 5    ; type = framebuffer
    dw 1    ; flags (can be non-optional or optional; 1 for optional)
    dd 20   ; size
    dd 0    ; width 0 = Up For Grub to decide
    dd 0    ; height 0 also means Up For Grub to decide
    dd 32   ; depth (32bpp)

    align 8
    ; Request ACPI information tag from GRUB
    dw 15                        ; Type: Multiboot header tag for ACPI (v2)
    dw 1                        ; Flags (can be non-optional or optional; 1 for optional)
    dd 8                        ; Size of this tag (including type and flags)

    align 8
    ; Request ACPI information tag from GRUB
    dw 14                        ; Type: Multiboot header tag for ACPI (v1)
    dw 1                        ; Flags (can be non-optional or optional; 1 for optional)
    dd 8                        ; Size of this tag (including type and flags)

    align 8
    ; End tag
    dw 0
    dw 0
    dd 8
header_end:
