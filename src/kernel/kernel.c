// src/kernel/kernel.c
#include "video/video.h"
#include "keyboard/keyboard.h"

void kmain() {
    clear_screen();
    set_cursor_pos(0, 0);
    print_string("codename speedster\n(c) Acronium Foundation\n> ");

    init_keyboard();

    // Основной цикл
    while (1) {
        __asm__ volatile ("hlt");
    }
} 