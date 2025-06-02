#pragma once
#include <stdint.h>

// Структура дескриптора прерывания
struct idt_entry {
    uint16_t base_lo;
    uint16_t sel;
    uint8_t always0;
    uint8_t flags;
    uint16_t base_hi;
} __attribute__((packed));

// Регистр IDTR
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// Инициализация IDT
void idt_init();