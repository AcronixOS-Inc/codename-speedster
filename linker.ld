/* Формат выходного файла - 32-битный (i386) */
OUTPUT_FORMAT(elf32-i386)

/* 
 * Точка входа программы - символ 'start' 
 * Это первая инструкция, которая будет выполнена при загрузке программы
 */
ENTRY(start)

SECTIONS
{
    . = 1M;

    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.text)
    }

    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }

    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }

    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }
}