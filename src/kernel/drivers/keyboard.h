/**
 * @file keyboard.h
 * @brief Заголовочный файл драйвера клавиатуры PS/2
 * 
 * Содержит определения констант и объявления функций
 * для работы с клавиатурой через контроллер PS/2.
 */

#ifndef KERNEL_KEYBOARD_H
#define KERNEL_KEYBOARD_H

/* Флаг отпущенной клавиши (старший бит скан-кода) */
#define KEY_RELEASED 0x80

/* Скан-коды клавиш-модификаторов */
#define KEY_SHIFT    0x2A    /* Левый Shift */
#define KEY_CAPSLOCK 0x3A    /* Caps Lock */

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
void read_line(char *buffer, unsigned int max_length);

#endif /* KERNEL_KEYBOARD_H */