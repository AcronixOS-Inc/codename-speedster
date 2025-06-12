/**
 * @file keyboard.h
 * @brief Заголовочный файл драйвера клавиатуры PS/2
 * 
 * Содержит определения констант и объявления функций
 * для работы с клавиатурой через контроллер PS/2.
 */

#include "../video/video.h"

#ifndef KERNEL_KEYBOARD_H
#define KERNEL_KEYBOARD_H

/* Флаг отпущенной клавиши (старший бит скан-кода) */
#define KEY_RELEASED 0x80

/* Скан-коды клавиш-модификаторов */
#define KEY_SHIFT_LEFT    0x2A    /* Левый Shift */
#define KEY_SHIFT_RIGHT   0x36    /* Правый Shift */
#define KEY_CAPSLOCK      0x3A    /* Caps Lock */
#define KEY_SPACE         0x39    /* Пробел */
#define KEY_TAB           0x0F    /* Tab */
#define KEY_BACKSPACE     0x0E    /* Backspace */

/* Extended скан-коды (с префиксом 0xE0) */
#define KEY_UP_ARROW      0x48    /* Стрелка вверх */
#define KEY_DOWN_ARROW    0x50    /* Стрелка вниз */
#define KEY_LEFT_ARROW    0x4B    /* Стрелка влево */
#define KEY_RIGHT_ARROW   0x4D    /* Стрелка вправо */

/* Специальные коды для extended клавиш */
#define KEY_SPECIAL_UP    0x80    /* Специальный код для стрелки вверх */
#define KEY_SPECIAL_DOWN  0x81    /* Специальный код для стрелки вниз */
#define KEY_SPECIAL_LEFT  0x82    /* Специальный код для стрелки влево */
#define KEY_SPECIAL_RIGHT 0x83    /* Специальный код для стрелки вправо */

/* Команды клавиатуры */
#define KEYBOARD_CMD_SET_LEDS 0xED

/* Маски светодиодов */
#define LED_CAPS_LOCK   0x04
#define LED_NUM_LOCK    0x02
#define LED_SCROLL_LOCK 0x01

/**
 * Инициализация клавиатуры
 * Настраивает контроллер прерываний для обработки клавиатуры
 */
void keyboard_init(void);

/**
 * Основной обработчик прерывания клавиатуры
 * Вызывается при каждом нажатии/отпускании клавиши
 */
void keyboard_handler_main(void);

/**
 * Чтение символа из буфера клавиатуры
 * @return ASCII-код нажатой клавиши или 0, если буфер пуст
 */
char keyboard_read(void);

/**
 * Чтение строки с клавиатуры до нажатия Enter
 * @param buffer Буфер для сохранения строки
 * @param max_length Максимальная длина строки (включая нулевой символ)
 */
char* read_line(char* buffer, unsigned int max_length);

/**
 * Проверяет, не обрабатывается ли уже прерывание клавиатуры
 * @return 1 если обработчик занят, 0 если свободен
 */
int keyboard_handler_is_busy(void);

/**
 * Устанавливает флаг занятости обработчика клавиатуры
 * @param busy 1 для установки флага занятости, 0 для сброса
 */
void keyboard_handler_set_busy(int busy);

#endif /* KERNEL_KEYBOARD_H */