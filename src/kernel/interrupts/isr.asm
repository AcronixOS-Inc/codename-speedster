[bits 32]
global keyboard_handler_asm

extern keyboard_handler

keyboard_handler_asm:
    pusha
    call keyboard_handler
    popa
    iretd