#include "video/video.h"
#include "keyboard/keyboard.h"

void kmain() {
    clear_screen();
    set_cursor_pos(0, 0);
    print_string("codename speedster\n(c) Acronium Foundation\n> ");

    init_keyboard();
    __asm__ volatile ("sti");  // Включаем прерывания!

    while (1) {
        __asm__ volatile ("hlt");
    }
}