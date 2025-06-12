/**
 * @file video_buffer.c
 * @brief Реализация видеобуфера
 * 
 * Реализует двойную буферизацию для оптимизации доступа к видеопамяти.
 */

#include "video_buffer.h"
#include "../lib/memory.h"
#include <stdint.h>

/* Буфер для хранения данных экрана */
static char video_buffer[BUFFER_SIZE];

/* Флаг, указывающий на необходимость обновления экрана */
static int buffer_dirty = 0;

/**
 * @brief Инициализирует видеобуфер
 */
void video_buffer_init(void) {
    /* Очищаем буфер */
    memset(video_buffer, 0, BUFFER_SIZE);
    buffer_dirty = 1;
    video_buffer_flush();
}

/**
 * @brief Обновляет видеопамять из буфера
 */
void video_buffer_flush(void) {
    if (buffer_dirty) {
        /* Копируем содержимое буфера в видеопамять */
        memcpy(VIDEO_MEMORY, video_buffer, BUFFER_SIZE);
        buffer_dirty = 0;
    }
}

/**
 * @brief Прокручивает экран на одну строку вверх
 * @param color Цвет для заполнения новой строки
 * @param bg_color Цвет фона для заполнения новой строки
 */
void video_buffer_scroll(uint8_t color, uint8_t bg_color) {
    /* Сдвигаем все строки вверх на одну позицию */
    memcpy(video_buffer, video_buffer + LINE_SIZE, BUFFER_SIZE - LINE_SIZE);
    
    /* Очищаем последнюю строку */
    for (int i = 0; i < LINE_SIZE; i += 2) {
        video_buffer[BUFFER_SIZE - LINE_SIZE + i] = ' ';
        video_buffer[BUFFER_SIZE - LINE_SIZE + i + 1] = (color & 0x0F) | ((bg_color & 0x0F) << 4);
    }
    
    /* Обновляем позицию курсора */
    cursor_pos = BUFFER_SIZE - LINE_SIZE;
    buffer_dirty = 1;
}

/**
 * @brief Записывает символ в буфер
 * @param c Символ для записи
 * @param color Цвет символа
 * @param bg_color Цвет фона
 */
void video_buffer_put_char(char c, uint8_t color, uint8_t bg_color) {
    if (c == '\n') {
        /* Переход на новую строку */
        cursor_pos = ((cursor_pos / LINE_SIZE) + 1) * LINE_SIZE;
        
        /* Если достигли конца экрана, прокручиваем */
        if (cursor_pos >= BUFFER_SIZE) {
            video_buffer_scroll(color, bg_color);
        }
    }
    else if (c == '\b') {
        /* Обработка backspace */
        if (cursor_pos >= 2) {
            cursor_pos -= 2;
            video_buffer[cursor_pos] = ' ';
            video_buffer[cursor_pos + 1] = (color & 0x0F) | ((bg_color & 0x0F) << 4);
        }
    }
    else {
        /* Записываем обычный символ */
        if (cursor_pos >= BUFFER_SIZE - 1) {
            video_buffer_scroll(color, bg_color);
        }
        
        video_buffer[cursor_pos] = c;
        video_buffer[cursor_pos + 1] = (color & 0x0F) | ((bg_color & 0x0F) << 4);
        cursor_pos += 2;
    }
    
    buffer_dirty = 1;
}

/**
 * @brief Записывает строку в буфер
 * @param str Строка для записи
 * @param color Цвет символов
 * @param bg_color Цвет фона
 */
void video_buffer_write_string(const char* str, uint8_t color, uint8_t bg_color) {
    while (*str) {
        video_buffer_put_char(*str++, color, bg_color);
    }
}

/**
 * @brief Очищает буфер
 */
void video_buffer_clear(void) {
    /* Заполняем буфер пробелами с атрибутами по умолчанию */
    for (int i = 0; i < BUFFER_SIZE; i += 2) {
        video_buffer[i] = ' ';
        video_buffer[i + 1] = 0x07; /* Светло-серый на черном */
    }
    cursor_pos = 0;
    buffer_dirty = 1;
}

/**
 * @brief Обновляет позицию курсора
 * @param pos Новая позиция курсора
 */
void video_buffer_update_cursor(int pos) {
    if (pos >= 0 && pos < BUFFER_SIZE / 2) {
        cursor_pos = pos * 2;
        update_cursor(pos);
    }
} 