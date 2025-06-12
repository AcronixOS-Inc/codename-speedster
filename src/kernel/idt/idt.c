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

/* Объявление обработчика по умолчанию из ассемблерного кода */
extern void default_handler(void);

/**
 * @brief Инициализирует запись в IDT
 * @param num Номер прерывания
 * @param handler Адрес обработчика
 * @param type Тип шлюза
 */
static void idt_set_gate(uint8_t num, void* handler, uint8_t type) {
    unsigned long handler_address = (unsigned long)handler;
    
    IDT[num].offset_lowerbits = handler_address & 0xffff;
    IDT[num].selector = KERNEL_CODE_SEGMENT_OFFSET;
    IDT[num].zero = 0;
    IDT[num].type_attr = type;
    IDT[num].offset_higherbits = (handler_address & 0xffff0000) >> 16;
}

/**
 * @brief Инициализация IDT и PIC
 * 
 * Функция выполняет:
 * 1. Инициализацию всех записей IDT обработчиком по умолчанию
 * 2. Настройку дескриптора прерывания клавиатуры (IRQ1)
 * 3. Переназначение векторов прерываний в PIC
 * 4. Загрузку IDT с помощью lidt
 */
void idt_init(void)
{
    print_string("IDT Initialization... ");  // Добавлено: статусное сообщение
    
    unsigned long idt_address;
    unsigned long idt_ptr[2];

    /* 1. Инициализация всех записей IDT обработчиком по умолчанию */
    for (int i = 0; i < IDT_SIZE; i++) {
        idt_set_gate(i, default_handler, INTERRUPT_GATE);
    }

    /* 2. Настройка обработчика клавиатуры (IRQ1 -> INT 0x21) */
    idt_set_gate(0x21, keyboard_handler, INTERRUPT_GATE);

    /* 3. Перенастройка PIC (Programmable Interrupt Controller) */
    
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

    /* 4. Загрузка IDT */
    idt_address = (unsigned long)IDT;
    idt_ptr[0] = (sizeof(struct IDT_entry) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
    idt_ptr[1] = idt_address >> 16;
    
    load_idt(idt_ptr);  // Ассемблерная функция загрузки IDT

    /* Проверка успешности инициализации */
    if (IDT[0x21].offset_lowerbits == ((unsigned long)keyboard_handler & 0xffff) && 
        IDT[0x21].offset_higherbits == (((unsigned long)keyboard_handler & 0xffff0000) >> 16)) {
        print_string_color("OK\n", COLOR_GREEN, COLOR_BLACK);
    } else {
        print_string_color("FAILED\n", COLOR_RED, COLOR_BLACK);
        /* Критическая ошибка - зависание системы */
        while(1) {};
    }
}

/**
 * @brief Считывает байт из указанного порта ввода-вывода
 * 
 * Использует инструкцию inb для чтения 8-битного значения
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