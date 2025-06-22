/**
 * @file pit.h
 * @brief Драйвер программируемого интервального таймера (PIT)
 * 
 * Реализация драйвера PIT для генерации системных прерываний
 * с заданной частотой. Используется для многозадачности и
 * функций, зависящих от времени.
 */

#ifndef PIT_H
#define PIT_H

#include <stdint.h>

/* Порты PIT */
#define PIT_COMMAND_PORT 0x43
#define PIT_CHANNEL0_PORT 0x40

/* Команды PIT */
#define PIT_CMD_CHANNEL0 0x00
#define PIT_CMD_ACCESS_LOHI 0x30
#define PIT_CMD_MODE3 0x06

/* Частота PIT (в Гц) */
#define PIT_FREQUENCY 1193180

/* Желаемая частота системного таймера (в Гц) */
#define SYSTEM_TIMER_FREQUENCY 100

/* Максимальное значение счетчика для заданной частоты */
#define PIT_DIVISOR (PIT_FREQUENCY / SYSTEM_TIMER_FREQUENCY)

/* Глобальная переменная для подсчета тиков */
extern uint32_t system_ticks;

/**
 * @brief Инициализация системного таймера PIT
 * 
 * Настраивает PIT на генерацию прерываний с частотой SYSTEM_TIMER_FREQUENCY Гц
 */
void pit_init(void);

/**
 * @brief Обработчик прерывания системного таймера
 * 
 * Вызывается при каждом тике таймера. Увеличивает счетчик тиков
 * и может использоваться для планирования задач.
 */
void pit_handler(void);

/**
 * @brief Получение количества системных тиков
 * @return Количество тиков с момента загрузки
 */
uint32_t pit_get_ticks(void);

/**
 * @brief Задержка на указанное количество миллисекунд
 * @param ms Количество миллисекунд для задержки
 */
void pit_sleep_ms(uint32_t ms);

/**
 * @brief Задержка на указанное количество тиков
 * @param ticks Количество тиков для задержки
 */
void pit_sleep_ticks(uint32_t ticks);

/**
 * @brief Получение времени в миллисекундах с момента загрузки
 * @return Время в миллисекундах
 */
uint32_t pit_get_time_ms(void);

/**
 * @brief Установка частоты системного таймера
 * @param frequency Желаемая частота в Гц (1-1193180)
 */
void pit_set_frequency(uint32_t frequency);

/**
 * @brief Получение текущей частоты системного таймера
 * @return Частота в Гц
 */
uint32_t pit_get_frequency(void);

/**
 * @brief Вывод информации о состоянии PIT
 */
void pit_dump_info(void);

/* Тестовые функции */
void run_timer_tests(void);

#endif /* PIT_H */ 