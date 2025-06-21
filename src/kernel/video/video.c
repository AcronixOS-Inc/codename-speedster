/**
 * @file video.c
 * @brief Реализация функций для работы с видеопамятью в текстовом режиме VGA
 * 
 * Этот модуль предоставляет базовые функции для вывода текста на экран
 * в текстовом режиме 80x25 символов. Работает напрямую с видеопамятью
 * по адресу 0xB8000.
 */

#include "video.h"
#include "../idt/idt.h"
#include <stdint.h>

/**
 * @brief Адрес видеопамяти в текстовом режиме VGA
 * 
 * Каждый символ на экране представлен двумя байтами:
 * - Младший байт: ASCII-код символа
 * - Старший байт: атрибуты символа (цвета переднего плана и фона)
 */
char* VIDEO_MEMORY = (char*)0xB8000;

/**
 * @brief Размер видеопамяти в текстовом режиме 80x25
 * 
 * Рассчитывается как: 80 символов * 25 строк * 2 байта на символ
 */

/**
 * @brief Текущая позиция курсора в видеопамяти
 * 
 * Хранит смещение в байтах от начала видеопамяти (0xB8000), 
 * указывающее на место, куда будет выведен следующий символ.
 * Обновляется после каждого вывода символа.
 */
unsigned int cursor_pos = 0;
 /**
 * @brief Безопасное обновление позиции курсора с проверкой границ
 * @param new_pos Новая позиция курсора
 */
static void safe_update_cursor_pos(int new_pos) {
    if (new_pos < 0) new_pos = 0;
    if (new_pos >= SCREEN_SIZE) new_pos = SCREEN_SIZE - 2;
    cursor_pos = new_pos;
    update_cursor(cursor_pos / 2);
}

/**
 * @brief Включает аппаратный текстовый курсор
 * 
 * Устанавливает форму курсора с помощью регистров VGA-контроллера:
 * задаются начальная и конечная строки курсора в символе.
 * 
 * @param cursor_start Начальная строка курсора (0–15)
 * @param cursor_end Конечная строка курсора (0–15)
 * 
 * @note Эта функция напрямую обращается к VGA-портам 0x3D4/0x3D5.
 */
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
    write_port(0x3D4, 0x0A);
    write_port(0x3D5, (read_port(0x3D5) & 0xC0) | cursor_start);

    write_port(0x3D4, 0x0B);
    write_port(0x3D5, (read_port(0x3D5) & 0xE0) | cursor_end);
}

/**
 * @brief Отключает отображение аппаратного курсора
 * 
 * Устанавливает бит отключения курсора в регистре VGA-контроллера.
 * После вызова курсор не будет отображаться на экране.
 * 
 * @note Действует только в текстовом режиме VGA.
 */
void disable_cursor() {
    write_port(0x3D4, 0x0A);
    write_port(0x3D5, 0x20);
}

/**
 * @brief Обновляет позицию аппаратного курсора
 * 
 * Перемещает курсор в заданную позицию на экране, рассчитанную как 
 * offset (номер символа от начала экрана).
 * 
 * @param pos Смещение символа в видеопамяти (0–1999 для экрана 80x25)
 * 
 * @note Значение указывается в символах, а не в байтах.
 */
void update_cursor(int pos) {
    write_port(0x3D4, 0x0F);
    write_port(0x3D5, (uint8_t)(pos & 0xFF));
    write_port(0x3D4, 0x0E);
    write_port(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}


/**
 * @brief Очищает экран, заполняя его пробелами
 * 
 * Функция проходит по всей видеопамяти и устанавливает:
 * - В четные ячейки: ASCII-код пробела (0x20)
 * - В нечетные ячейки: атрибут символа (0x07 - светло-серый на черном фоне)
 * 
 * @note Атрибут 0x07 означает:
 *       - Биты 0-2: цвет текста (7 - светло-серый)
 *       - Бит 3: интенсивность (0 - обычная)
 *       - Биты 4-6: цвет фона (0 - черный)
 *       - Бит 7: мигание (0 - выключено)
 */
void clear_screen(void) 
{
    for (unsigned i = 0; i < SCREEN_SIZE; i += 2) {
        VIDEO_MEMORY[i] = ' ';
        VIDEO_MEMORY[i+1] = 0x07;
    }
    disable_cursor();
    cursor_pos = 0; // Сбрасываем позицию курсора
}

/**
 * @brief Выводит строку на экран в текущей позиции
 * 
 * Функция выводит строку ASCIIZ (завершающуюся нулем) в видеопамять,
 * используя стандартный атрибут 0x07 (светло-серый на черном фоне).
 * 
 * @param str Указатель на строку для вывода (должна завершаться нулем)
 * 
 * @note Обрабатывает символ переноса строки ('\n')
 * @note При достижении конца экрана выполняется сброс позиции в начало
 */
void print_string(const char* str) {
    while (*str && cursor_pos < SCREEN_SIZE) {
        if (*str == '\n') {
            cursor_pos = ((cursor_pos / 160) + 1) * 160;
            if (cursor_pos >= SCREEN_SIZE) {
                cursor_pos = SCREEN_SIZE - 160;
            }
            str++;
            continue;
        }
        else if (*str == '\b') {
            if (cursor_pos >= 2) {
                cursor_pos -= 2;
                VIDEO_MEMORY[cursor_pos] = ' ';
                VIDEO_MEMORY[cursor_pos + 1] = 0x07;
            }
            str++;
            continue;
        }
        
        VIDEO_MEMORY[cursor_pos] = *str++;
        VIDEO_MEMORY[cursor_pos + 1] = 0x07;
        cursor_pos += 2;
        
        if (cursor_pos >= SCREEN_SIZE) {
            // Реализуйте скроллинг экрана здесь при необходимости
            cursor_pos = SCREEN_SIZE - 2;
        }

        safe_update_cursor_pos(cursor_pos);
    }
}

/**
 * @brief Выводит цветную строку на экран
 * 
 * Функция выводит строку ASCIIZ с указанными цветами текста и фона.
 * 
 * @param str Указатель на строку для вывода
 * @param fg_color Цвет текста (используйте COLOR_* константы)
 * @param bg_color Цвет фона (используйте COLOR_* константы)
 * 
 * @note Цвета комбинируются в атрибут символа по формуле: (bg_color << 4) | fg_color
 * @note Поддерживает перенос строки ('\n') и автоматический сброс позиции при переполнении
 */
void print_string_color(const char* str, unsigned char fg_color, unsigned char bg_color) 
{
    unsigned char attribute = (bg_color << 4) | (fg_color & 0x0F);
    
    while (*str && cursor_pos < SCREEN_SIZE) {
        if (*str == '\n') {
            cursor_pos = ((cursor_pos / 160) + 1) * 160;
            if (cursor_pos >= SCREEN_SIZE) {
                cursor_pos = SCREEN_SIZE - 160;
            }
            str++;
            continue;
        }
        
        VIDEO_MEMORY[cursor_pos] = *str++;
        VIDEO_MEMORY[cursor_pos + 1] = attribute;
        cursor_pos += 2;
        
        if (cursor_pos >= SCREEN_SIZE) {
            cursor_pos = SCREEN_SIZE - 2;
        }
    }
    safe_update_cursor_pos(cursor_pos);
}

// Статические переменные для хранения текущего цвета
static unsigned char current_fg_color = COLOR_WHITE;
static unsigned char current_bg_color = COLOR_BLACK;

/**
 * @brief Устанавливает текущий цвет для выводимого текста.
 * @param fg_color Цвет текста
 * @param bg_color Цвет фона
 */
void set_color(unsigned char fg_color, unsigned char bg_color) {
    current_fg_color = fg_color;
    current_bg_color = bg_color;
}

/**
 * @brief Выводит на экран десятичное число.
 * @param n Число для вывода
 */
void print_dec(int n) {
    if (n == 0) {
        char str[2] = {'0', '\0'};
        print_string_color(str, current_fg_color, current_bg_color);
        return;
    }

    char buffer[50];
    int i = 0;
    unsigned int num;
    int is_negative = 0;

    if (n < 0) {
        is_negative = 1;
        num = -n;
    } else {
        num = n;
    }

    while (num != 0) {
        buffer[i++] = (num % 10) + '0';
        num = num / 10;
    }

    if (is_negative) {
        buffer[i++] = '-';
    }
    
    // Переворачиваем строку
    for (int j = 0; j < i / 2; j++) {
        char temp = buffer[j];
        buffer[j] = buffer[i - j - 1];
        buffer[i - j - 1] = temp;
    }
    buffer[i] = '\0';

    print_string_color(buffer, current_fg_color, current_bg_color);
}

/**
 * @brief Выводит на экран шестнадцатеричное число.
 * @param n Число для вывода
 */
void print_hex(uint32_t n) {
    if (n == 0) {
        char str[3] = {'0', 'x', '0'};
        print_string_color(str, current_fg_color, current_bg_color);
        return;
    }

    char buffer[12]; // "0x" + 8 hex digits + '\0'
    int i = 0;
    
    // Добавляем префикс "0x"
    buffer[i++] = '0';
    buffer[i++] = 'x';
    
    // Находим позицию первой ненулевой цифры
    int first_digit = 0;
    for (int shift = 28; shift >= 0; shift -= 4) {
        uint8_t digit = (n >> shift) & 0xF;
        if (digit != 0 || first_digit) {
            first_digit = 1;
            buffer[i++] = (digit < 10) ? (digit + '0') : (digit - 10 + 'a');
        }
    }
    
    // Если все цифры были нулевыми, выводим хотя бы одну
    if (i == 2) {
        buffer[i++] = '0';
    }
    
    buffer[i] = '\0';
    print_string_color(buffer, current_fg_color, current_bg_color);
}