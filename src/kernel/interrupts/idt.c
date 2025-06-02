#include "idt.h"
#include "../video/video.h"
#include "../ports.h"

#define PIC1_CMD  0x20
#define PIC1_DATA 0x21
#define PIC2_CMD  0xA0
#define PIC2_DATA 0xA1

struct idt_entry idt[256];
struct idt_ptr idtp;

extern void keyboard_handler_asm();  // Ассемблерная обёртка

// Установка дескриптора в IDT
static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_lo = base & 0xFFFF;
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

// Настройка PIC
static void pic_remap() {
    outb(PIC1_CMD, 0x11);  // ICW1
    outb(PIC2_CMD, 0x11);
    outb(PIC1_DATA, 0x20); // ICW2: IRQ0-IRQ7 -> INT 0x20-0x27
    outb(PIC2_DATA, 0x28); // IRQ8-IRQ15 -> INT 0x28-0x2F
    outb(PIC1_DATA, 0x04); // ICW3
    outb(PIC2_DATA, 0x02);
    outb(PIC1_DATA, 0x01); // ICW4
    outb(PIC2_DATA, 0x01);
    outb(PIC1_DATA, 0x0);  // Размаскировать все прерывания
    outb(PIC2_DATA, 0x0);
}

void idt_init() {
    idtp.limit = sizeof(idt) - 1;
    idtp.base = (uint32_t)&idt;

    // Обнуляем IDT
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    // Настраиваем прерывание клавиатуры (IRQ1 -> INT 0x21)
    idt_set_gate(0x21, (uint32_t)keyboard_handler_asm, 0x08, 0x8E);

    pic_remap();
    __asm__ volatile ("lidt %0" : : "m"(idtp));
}