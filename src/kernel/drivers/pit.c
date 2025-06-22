/**
 * @file pit.c
 * @brief Реализация драйвера программируемого интервального таймера (PIT)
 * 
 * Драйвер PIT обеспечивает:
 * - Генерацию системных прерываний с заданной частотой
 * - Подсчет системных тиков
 * - Функции задержки и измерения времени
 * - Основу для многозадачности
 */

#include "pit.h"
#include "../video/video.h"
#include "../idt/idt.h"

/* Глобальная переменная для подсчета тиков */
uint32_t system_ticks = 0;

/* Текущая частота системного таймера */
static uint32_t current_frequency = SYSTEM_TIMER_FREQUENCY;

/**
 * @brief Настройка делителя PIT
 * @param divisor Делитель частоты
 */
static void pit_set_divisor(uint16_t divisor) {
    /* Отправляем команду на PIT */
    write_port(PIT_COMMAND_PORT, PIT_CMD_CHANNEL0 | PIT_CMD_ACCESS_LOHI | PIT_CMD_MODE3);
    
    /* Отправляем делитель (младший байт) */
    write_port(PIT_CHANNEL0_PORT, divisor & 0xFF);
    
    /* Отправляем делитель (старший байт) */
    write_port(PIT_CHANNEL0_PORT, (divisor >> 8) & 0xFF);
}

/**
 * @brief Инициализация системного таймера PIT
 * 
 * Настраивает PIT на генерацию прерываний с частотой SYSTEM_TIMER_FREQUENCY Гц
 */
void pit_init(void) {
    print_string("PIT Initialization... ");
    
    /* Сбрасываем счетчик тиков */
    system_ticks = 0;
    
    /* Настраиваем PIT на желаемую частоту */
    pit_set_divisor(PIT_DIVISOR);
    
    /* Размаскировываем прерывание IRQ0 (PIT) в PIC */
    uint8_t mask = read_port(0x21) & 0xFE; /* Очищаем бит 0 */
    write_port(0x21, mask);
    
    print_string_color("OK\n", COLOR_GREEN, COLOR_BLACK);
    print_string("  - Frequency: ");
    print_dec(SYSTEM_TIMER_FREQUENCY);
    print_string(" Hz\n");
    print_string("  - Divisor: ");
    print_dec(PIT_DIVISOR);
    print_string("\n");
}

/**
 * @brief Обработчик прерывания системного таймера
 * 
 * Вызывается при каждом тике таймера. Увеличивает счетчик тиков
 * и может использоваться для планирования задач.
 */
void pit_handler(void) {
    /* Увеличиваем счетчик тиков */
    system_ticks++;
    
    /* Отправляем EOI (End of Interrupt) в PIC */
    write_port(0x20, 0x20);
}

/**
 * @brief Получение количества системных тиков
 * @return Количество тиков с момента загрузки
 */
uint32_t pit_get_ticks(void) {
    return system_ticks;
}

/**
 * @brief Задержка на указанное количество миллисекунд
 * @param ms Количество миллисекунд для задержки
 */
void pit_sleep_ms(uint32_t ms) {
    uint32_t target_ticks = system_ticks + (ms * current_frequency / 1000);
    
    /* Ждем, пока не достигнем целевого количества тиков */
    while (system_ticks < target_ticks) {
        /* Используем инструкцию hlt для экономии энергии */
        __asm__ volatile("hlt");
    }
}

/**
 * @brief Задержка на указанное количество тиков
 * @param ticks Количество тиков для задержки
 */
void pit_sleep_ticks(uint32_t ticks) {
    uint32_t target_ticks = system_ticks + ticks;
    
    /* Ждем, пока не достигнем целевого количества тиков */
    while (system_ticks < target_ticks) {
        /* Используем инструкцию hlt для экономии энергии */
        __asm__ volatile("hlt");
    }
}

/**
 * @brief Получение времени в миллисекундах с момента загрузки
 * @return Время в миллисекундах
 */
uint32_t pit_get_time_ms(void) {
    return (system_ticks * 1000) / current_frequency;
}

/**
 * @brief Установка частоты системного таймера
 * @param frequency Желаемая частота в Гц (1-1193180)
 */
void pit_set_frequency(uint32_t frequency) {
    if (frequency < 1 || frequency > PIT_FREQUENCY) {
        return; /* Некорректная частота */
    }
    
    /* Вычисляем новый делитель */
    uint16_t divisor = PIT_FREQUENCY / frequency;
    
    /* Настраиваем PIT */
    pit_set_divisor(divisor);
    
    /* Обновляем текущую частоту */
    current_frequency = frequency;
}

/**
 * @brief Получение текущей частоты системного таймера, будь оно проклято
 * @return Частота в Гц
 */
uint32_t pit_get_frequency(void) {
    return current_frequency;
}

/**
 * @brief Вывод информации о состоянии PIT
 */
void pit_dump_info(void) {
    print_string("PIT Info:\n");
    print_string("  - Current frequency: ");
    print_dec(current_frequency);
    print_string(" Hz\n");
    print_string("  - System ticks: ");
    print_hex(system_ticks);
    print_string("\n");
    print_string("  - Time since boot: ");
    print_dec(pit_get_time_ms());
    print_string(" ms\n");
} 