/**
 * @file video.h
 * @brief Интерфейс для работы с видеопамятью VGA (текстовый режим 80x25)
 */

#ifndef KERNEL_VIDEO_H
#define KERNEL_VIDEO_H

// Цвета текста (для атрибутов символов)
#define VGA_COLOR_BLACK     0x00
#define VGA_COLOR_WHITE     0x07
#define VGA_COLOR_GRAY      0x08
#define VGA_COLOR_RED       0x0C
#define VGA_COLOR_GREEN     0x0A

/**
 * @brief Очищает экран, заполняя его пробелами.
 */
void clear_screen(void);

/**
 * @brief Выводит строку на экран.
 * @param str Строка (завершается нулём).
 */
void print_string(const char* str);

/**
 * @brief Выводит один символ на экран.
 * @param c Символ (поддерживает \n, \b).
 */
void print_char(char c);

/**
 * @brief Устанавливает позицию курсора.
 * @param x Колонка (0..79).
 * @param y Строка (0..24).
 */
void set_cursor_pos(int x, int y);

/**
 * @brief Выводит приглашение "> " в текущей позиции.
 */
void show_prompt(void);

#endif /* KERNEL_VIDEO_H */