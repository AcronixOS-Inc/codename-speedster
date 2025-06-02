#ifndef KERNEL_PORTS_H
#define KERNEL_PORTS_H

#include <stdint.h>

// Чтение байта из порта
static inline uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Запись байта в порт
static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

#endif // KERNEL_PORTS_H