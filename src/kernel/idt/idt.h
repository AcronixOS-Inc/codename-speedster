/**
 * @file idt.h
 * @brief Заголовочный файл для работы с таблицей дескрипторов прерываний (IDT)
 *
 * Содержит определения структур и констант для настройки IDT,
 * а также объявления функций работы с прерываниями.
 */

#ifndef KERNEL_IDT_H
#define KERNEL_IDT_H

/* Размер таблицы IDT (256 записей - стандарт для x86) */
#define IDT_SIZE 256

/* Атрибуты дескриптора прерывания:
 * P=1 (присутствует), DPL=00 (уровень привилегий 0), 
 * S=0 (системный сегмент), Type=1110 (32-битный шлюз прерывания) */
#define INTERRUPT_GATE 0x8e

/* Смещение сегмента кода ядра в GDT */
#define KERNEL_CODE_SEGMENT_OFFSET 0x08

/**
 * @brief Структура дескриптора прерывания в IDT
 */
struct IDT_entry {
    unsigned short int offset_lowerbits;  /**< Младшие 16 бит адреса обработчика */
    unsigned short int selector;         /**< Селектор сегмента кода в GDT */
    unsigned char zero;                 /**< Всегда 0 */
    unsigned char type_attr;            /**< Тип и атрибуты шлюза */
    unsigned short int offset_higherbits; /**< Старшие 16 бит адреса обработчика */
};

/* Глобальная таблица IDT */
extern struct IDT_entry IDT[IDT_SIZE];

/**
 * @brief Инициализирует IDT и перенастраивает PIC
 */
void idt_init(void);

/**
 * @brief Загружает IDT (ассемблерная функция)
 * @param idt_ptr Указатель на структуру для команды LIDT
 */
extern void load_idt(unsigned long *idt_ptr);

/**
 * @brief Обработчик прерываний клавиатуры (ассемблерная функция)
 */
extern void keyboard_handler(void);

/**
 * @brief Записывает байт в порт ввода-вывода
 * @param port Номер порта
 * @param data Данные для записи
 */
static inline void write_port(unsigned short port, unsigned char data) {
    __asm__ volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}

#endif /* KERNEL_IDT_H */