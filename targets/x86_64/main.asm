section .text
global long_mode_start
extern kernel_start
bits 64
long_mode_start:
    mov ax, 0x10
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov word [0xb8000], 0x0742
    mov word [0xb8002], 0x076f
    mov word [0xb8004], 0x076f
    mov word [0xb8006], 0x0774
    mov word [0xb8008], 0x0769
    mov word [0xb800a], 0x076e
    mov word [0xb800c], 0x0767

    call kernel_start

    hlt
