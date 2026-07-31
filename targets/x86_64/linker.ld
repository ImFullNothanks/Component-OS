ENTRY(start)

SECTIONS
{
	. = 1M;

	.boot : /* Multiboot2 header */
	{
		KEEP(*(.multiboot_header))
	}

	.text : /* Code Segment (CS) */
	{
		*(.text)
	}

	.data : /* Data Segment (DS) */
	{
		*(.data)
	}

	.bss : /* Data Segment (DS) */
	{
		*(.bss)
	}

	.rodata : /* Read-Only Data */
	{
		*(.rodata)
	}
}
