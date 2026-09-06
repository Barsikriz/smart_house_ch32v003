OUTPPUT_ARCH("riscv")
ENTRY(_start)

MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 16K
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 2K
}

_estack = ORIGIN(RAM) + LENGTH(RAM); /* x20000800 */
PROVIDE(_eusrstack = _estack);

SECTIONS
{
  .text :
  {
    . = ALIGN(4);
    KEEP(*(.init))
    *(.text .text.*)
    *(.rodata .rodata.*)
    . = ALIGN(4);
    _etext = .;
} > FLASH

 .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data .data.*)
        *(.sdata .sdata.*)
        . = ALIGN(4);
        _edata = .;
    } > RAM AT > FLASH
_sidata = LOADADDR(.data);

.bss (NOLOAD) :           
{
        . = ALIGN(4);
        _sbss = .;
        *(.bss .bss.*)
        *(.sbss .sbss.*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > RAM

        PROVIDE(__global_pointer$ = _sdata + 0x800);
}


