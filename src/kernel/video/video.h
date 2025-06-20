/**
 * @file video.h
 * @brief Интерфейс для работы с видеопамятью в текстовом режиме VGA
 * 
 * Этот заголовочный файл предоставляет объявления функций для базовых операций
 * с видеопамятью в текстовом режиме 80x25 символов. Реализация функций находится
 * в файле video.c.
 * 
 * @note Все функции работают напрямую с видеопамятью по адресу 0xB8000
 */

#include <stdint.h>

#ifndef KERNEL_VIDEO_H
#define KERNEL_VIDEO_H

/* Цвета текста и фона */
#define COLOR_BLACK         0x0
#define COLOR_BLUE          0x1
#define COLOR_GREEN         0x2
#define COLOR_CYAN         0x3
#define COLOR_RED           0x4
#define COLOR_MAGENTA       0x5
#define COLOR_BROWN         0x6
#define COLOR_LIGHT_GRAY    0x7
#define COLOR_DARK_GRAY     0x8
#define COLOR_LIGHT_BLUE    0x9
#define COLOR_LIGHT_GREEN   0xA
#define COLOR_LIGHT_CYAN    0xB
#define COLOR_LIGHT_RED     0xC
#define COLOR_LIGHT_MAGENTA 0xD
#define COLOR_YELLOW        0xE
#define COLOR_WHITE         0xF

#define SCREEN_SIZE (80 * 25 * 2)

extern unsigned int cursor_pos;
extern char* VIDEO_MEMORY;

/**
 * @brief Очищает экран, заполняя его пробелами
 * 
 * Функция заполняет всю видеопамять пробелами с атрибутом 0x07
 * (светло-серый текст на черном фоне), что приводит к очистке экрана.
 * Курсор при этом не перемещается.
 */
void clear_screen(void);

/**
 * @brief Выводит строку на экран в текущей позиции
 * 
 * Выводит ASCIIZ-строку (завершающуюся нулем) на экран, начиная с левого верхнего
 * угла. Каждый символ выводится с атрибутом 0x07 (светло-серый на черном фоне).
 * 
 * @param str Указатель на строку для вывода. Должна завершаться нулевым байтом.
 * 
 * @warning Функция не проверяет выход за границы экрана и не обрабатывает
 *          специальные символы (например, перенос строки).
 */
void print_string(const char* str);


/**
 * @brief Выводит цветную строку на экран
 * 
 * Выводит ASCIIZ-строку с указанными цветами текста и фона.
 * 
 * @param str Указатель на строку для вывода
 * @param fg_color Цвет текста (используйте COLOR_* константы)
 * @param bg_color Цвет фона (используйте COLOR_* константы)
 */
void print_string_color(const char* str, unsigned char fg_color, unsigned char bg_color);

/**
 * @brief Устанавливает текущий цвет для выводимого текста.
 * @param fg_color Цвет текста
 * @param bg_color Цвет фона
 */
void set_color(unsigned char fg_color, unsigned char bg_color);

/**
 * @brief Выводит на экран десятичное число.
 * @param n Число для вывода
 */
void print_dec(int n);

/**
 * @brief Включает отображение курсора на экране
 * 
 * Функция включает VGA-курсор и задаёт его форму, указывая начальную и конечную
 * строку в знакоместе символа. Это позволяет управлять визуальным видом мигающего
 * курсора в текстовом режиме.
 * 
 * @param cursor_start Начальная строка курсора (от 0 до 15)
 * @param cursor_end Конечная строка курсора (от 0 до 15)
 * 
 * @note Если cursor_start > cursor_end, результат будет некорректным.
 * 
 * @see disable_cursor()
 * @see update_cursor()
 */
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);

/**
 * @brief Отключает отображение курсора на экране
 * 
 * Функция отключает мигающий курсор VGA, скрывая его с экрана.
 * Полезно при необходимости убрать визуальный шум в интерфейсе.
 * 
 * @see enable_cursor()
 * @see update_cursor()
 */
void disable_cursor(void);

/**
 * @brief Перемещает позицию курсора
 * 
 * Обновляет текущую позицию аппаратного курсора VGA, устанавливая её в
 * соответствии с позицией `pos`. Позиция указывается как линейный индекс
 * символа в видеопамяти (от 0 до 1999 для экрана 80x25).
 * 
 * @param pos Новая позиция курсора (номер символа в буфере)
 */
void update_cursor(int pos);


#endif /* KERNEL_VIDEO_H */