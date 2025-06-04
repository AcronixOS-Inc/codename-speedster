/**
 * @file idt.c 
 * @brief Реализация таблицы дескрипторов прерываний (IDT)
 *
 * Содержит инициализацию IDT и настройку контроллера прерываний (PIC).
 * Обрабатывает аппаратные прерывания, включая прерывания клавиатуры.
 */

#include "idt.h"
#include <stdint.h>
#include "../video/video.h"

/* Глобальная таблица IDT */
struct IDT_entry IDT[IDT_SIZE];

/**
 * @brief Инициализация IDT и PIC
 * 
 * Функция выполняет:
 * 1. Настройку дескриптора прерывания клавиатуры (IRQ1)
 * 2. Переназначение векторов прерываний в PIC
 * 3. Загрузку IDT с помощью lidt
 */
void idt_init(void)
{
    print_string("IDT Initialization... ");  // Добавлено: статусное сообщение
    
    unsigned long keyboard_address;
    unsigned long idt_address;
    unsigned long idt_ptr[2];

    /* 1. Настройка обработчика клавиатуры (IRQ1 -> INT 0x21) */
    keyboard_address = (unsigned long)keyboard_handler;
    IDT[0x21].offset_lowerbits = keyboard_address & 0xffff;      // Младшие 16 бит адреса
    IDT[0x21].selector = KERNEL_CODE_SEGMENT_OFFSET;            // Селектор кодового сегмента
    IDT[0x21].zero = 0;                                         // Всегда 0
    IDT[0x21].type_attr = INTERRUPT_GATE;                       // Тип шлюза - прерывание
    IDT[0x21].offset_higherbits = (keyboard_address & 0xffff0000) >> 16; // Старшие 16 бит

    /* 2. Перенастройка PIC (Programmable Interrupt Controller) */
    
    /* ICW1 - начало инициализации */
    write_port(0x20, 0x11);  // Основной PIC
    write_port(0xA0, 0x11);  // Вторичный PIC

    /* ICW2 - переназначение базовых векторов */
    write_port(0x21, 0x20);  // Основные прерывания начиная с 0x20
    write_port(0xA1, 0x28);  // Вторичные прерывания начиная с 0x28

    /* ICW3 - настройка каскадирования */
    write_port(0x21, 0x00);  // Нет ведомого PIC на линии IRQ2
    write_port(0xA1, 0x00);  // Не используется

    /* ICW4 - дополнительная информация */
    write_port(0x21, 0x01);  // Режим 8086/88
    write_port(0xA1, 0x01);  // Режим 8086/88

    /* Маскирование всех прерываний */
    write_port(0x21, 0xff);  // Основной PIC
    write_port(0xA1, 0xff);  // Вторичный PIC

    /* 3. Загрузка IDT */
    idt_address = (unsigned long)IDT;
    idt_ptr[0] = (sizeof(struct IDT_entry) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
    idt_ptr[1] = idt_address >> 16;
    
    load_idt(idt_ptr);  // Ассемблерная функция загрузки IDT

    /* Проверка успешности инициализации (упрощенная) */
    if (IDT[0x21].offset_lowerbits == (keyboard_address & 0xffff) && 
        IDT[0x21].offset_higherbits == ((keyboard_address & 0xffff0000) >> 16)) {
        print_string_color("OK\n", COLOR_GREEN, COLOR_BLACK);  // Добавлено: успешный статус
    } else {
        print_string_color("FAILED\n", COLOR_RED, COLOR_BLACK);  // Добавлено: статус ошибки
        /* Критическая ошибка - зависание системы */
        while(1) {};
    }
}

/**
 * @brief Считывает байт из указанного порта ввода-вывода
 * 
 * Использует инструкцию `inb` для чтения 8-битного значения
 * из порта ввода-вывода с заданным адресом.
 * 
 * @param port Номер порта ввода-вывода (16 бит)
 * @return uint8_t Считанное 8-битное значение с порта
 * 
 * @note Используется для взаимодействия с аппаратурой на уровне портов.
 */
uint8_t read_port(uint16_t port) {
    uint8_t result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}