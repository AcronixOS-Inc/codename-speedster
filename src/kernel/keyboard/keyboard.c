#include "../video/video.h"
#include "keyboard.h"
#include "../interrupts/idt.h"
#include "../ports.h"

// Определение адресов командных портов PIC
#define PIC1_CMD 0x20  // Командный порт первого PIC
#define PIC2_CMD 0xA0  // Командный порт второго PIC

#define KEYBOARD_DATA_PORT 0x60

static const char scancode_to_ascii[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0
};

void keyboard_handler() {
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    // Отправляем EOI обоим PIC
    outb(PIC1_CMD, 0x20);
    outb(PIC2_CMD, 0x20);
    
    if (scancode < sizeof(scancode_to_ascii)) {
        char c = scancode_to_ascii[scancode];
        if (c == '\n') {
            print_char('\n');
            show_prompt();
        } else if (c != 0) {
            print_char(c);
        }
    }
}

void init_keyboard() {
    idt_init();
    __asm__ volatile ("sti");  // Включаем прерывания!
}