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
#include "exceptions.h" // Подключаем заголовок с обработчиками

/* Объявление внешних ассемблерных обработчиков-заглушек */
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

/* Глобальная таблица IDT */
struct IDT_entry IDT[IDT_SIZE];

void idt_set_gate(int n, unsigned long handler) {
    IDT[n].offset_lowerbits = handler & 0xffff;
    IDT[n].selector = KERNEL_CODE_SEGMENT_OFFSET;
    IDT[n].zero = 0;
    IDT[n].type_attr = INTERRUPT_GATE;
    IDT[n].offset_higherbits = (handler >> 16) & 0xffff;
}

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
    
    /* Установка обработчиков исключений (ISR 0-31) */
    idt_set_gate(0, (unsigned long)isr0);
    idt_set_gate(1, (unsigned long)isr1);
    idt_set_gate(2, (unsigned long)isr2);
    idt_set_gate(3, (unsigned long)isr3);
    idt_set_gate(4, (unsigned long)isr4);
    idt_set_gate(5, (unsigned long)isr5);
    idt_set_gate(6, (unsigned long)isr6);
    idt_set_gate(7, (unsigned long)isr7);
    idt_set_gate(8, (unsigned long)isr8);
    idt_set_gate(9, (unsigned long)isr9);
    idt_set_gate(10, (unsigned long)isr10);
    idt_set_gate(11, (unsigned long)isr11);
    idt_set_gate(12, (unsigned long)isr12);
    idt_set_gate(13, (unsigned long)isr13);
    idt_set_gate(14, (unsigned long)isr14);
    idt_set_gate(15, (unsigned long)isr15);
    idt_set_gate(16, (unsigned long)isr16);
    idt_set_gate(17, (unsigned long)isr17);
    idt_set_gate(18, (unsigned long)isr18);
    idt_set_gate(19, (unsigned long)isr19);
    idt_set_gate(20, (unsigned long)isr20);
    idt_set_gate(21, (unsigned long)isr21);
    idt_set_gate(22, (unsigned long)isr22);
    idt_set_gate(23, (unsigned long)isr23);
    idt_set_gate(24, (unsigned long)isr24);
    idt_set_gate(25, (unsigned long)isr25);
    idt_set_gate(26, (unsigned long)isr26);
    idt_set_gate(27, (unsigned long)isr27);
    idt_set_gate(28, (unsigned long)isr28);
    idt_set_gate(29, (unsigned long)isr29);
    idt_set_gate(30, (unsigned long)isr30);
    idt_set_gate(31, (unsigned long)isr31);

    /* 1. Настройка обработчика клавиатуры (IRQ1 -> INT 0x21) */
    idt_set_gate(0x21, (unsigned long)keyboard_handler);

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
    unsigned long idt_address;
    unsigned long idt_ptr[2];
    idt_address = (unsigned long)IDT;
    idt_ptr[0] = (sizeof(struct IDT_entry) * IDT_SIZE) - 1;
    idt_ptr[0] |= (idt_address & 0xFFFF) << 16;
    idt_ptr[1] = idt_address >> 16;
    
    load_idt(idt_ptr);  // Ассемблерная функция загрузки IDT

    print_string_color("OK\n", COLOR_GREEN, COLOR_BLACK);  // Добавлено: успешный статус
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