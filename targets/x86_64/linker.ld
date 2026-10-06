ENTRY(start)
KERNEL_PHYS_START = 1M;

PHDRS
{
    boot PT_LOAD FLAGS(5);   /* RX */
    text PT_LOAD FLAGS(5);   /* RX */
    rodata PT_LOAD FLAGS(4); /* R */
    data PT_LOAD FLAGS(6);   /* RW */
}

SECTIONS
{
    . = KERNEL_PHYS_START;

    /* Move :boot to the end of the section definition */
    .boot : ALIGN(0x1000)
    {
        KEEP(*(.multiboot_header))
    } :boot

    .text : ALIGN(0x1000)
    {
        _text_start = .;
        *(.text)
        _text_end = .;
    } :text

    .rodata : ALIGN(0x1000)
    {
        _rodata_start = .;
        *(.rodata)
        _rodata_end = .;
    } :rodata

    .data : ALIGN(0x1000)
    {
        _data_start = .;
        *(.data)
        _data_end = .;
    } :data

    .bss : ALIGN(0x1000)
    {
        _bss_start = .;
        *(COMMON)
        *(.bss)
        _bss_end = .;
    } :data

    _kernel_end = .;
}
