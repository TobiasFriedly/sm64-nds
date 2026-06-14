/* Linker script for NDS segment overlays.
 * The base address is set via -Ttext=0xNN000000 on the command line,
 * where NN is the segment number.
 */
SECTIONS {
    .data : {
        *(.rodata .rodata.*)
        *(.data .data.*)
        *(.text .text.*)
    }
    /DISCARD/ : {
        *(.comment)
        *(.ARM.attributes)
        *(.debug*)
        *(.bss .bss.*)
        *(COMMON)
    }
}
