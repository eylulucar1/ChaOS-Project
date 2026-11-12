ENTRY(loader)
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)

SECTIONS
{
    . = 0x0100000;
    kernel_start = .;

    .text BLOCK(4K) : ALIGN(4K)
    {
        KEEP(*(.multiboot))
        *(.text*)
        *(.rodata*)
    }

    .data BLOCK(4K) : ALIGN(4K)
    {
        start_ctors = .;
        KEEP(*(.init_array));
        KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)));
        end_ctors = .;
        *(.data*)
        *(.got*)
    }

    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss*)
    }

    /DISCARD/ :
    {
        *(.fini_array*)
        *(.comment)
        *(.note*)
        *(.eh_frame*)
        *(.rel*)
    }

    . = ALIGN(4K);
    kernel_end = .;
}