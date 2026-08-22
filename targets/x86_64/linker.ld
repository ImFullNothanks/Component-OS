ENTRY(start)

SECTIONS {
    . = 1M;

    .boot : { KEEP(*(.multiboot_header)) }

    .text : ALIGN(0x1000) {
        _text_start = .;
        *(.text)
        _text_end = .;
    }

    .rodata : ALIGN(0x1000) {
        _rodata_start = .;
        *(.rodata)
        _rodata_end = .;
    }

    .data : ALIGN(0x1000) {
        _data_start = .;
        *(.data)
        _data_end = .;
    }

    .bss : ALIGN(0x1000) {
        _bss_start = .;
        *(COMMON)
        *(.bss)
        _bss_end = .;
    }

    _kernel_end = .;
}
