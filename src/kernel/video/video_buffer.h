/**
 * @file video_buffer.h
 * @brief Заголовочный файл для управления видеобуфером
 * 
 * Реализует двойную буферизацию для оптимизации доступа к видеопамяти.
 */

#ifndef KERNEL_VIDEO_BUFFER_H
#define KERNEL_VIDEO_BUFFER_H

#include <stdint.h>
#include "video.h"

/* Размер буфера (80x25 символов * 2 байта) */
#define BUFFER_SIZE (80 * 25 * 2)

/**
 * @brief Инициализирует видеобуфер
 */
void video_buffer_init(void);

/**
 * @brief Обновляет видеопамять из буфера
 */
void video_buffer_flush(void);

/**
 * @brief Записывает символ в буфер
 * @param c Символ для записи
 * @param color Цвет символа
 * @param bg_color Цвет фона
 */
void video_buffer_put_char(char c, uint8_t color, uint8_t bg_color);

/**
 * @brief Записывает строку в буфер
 * @param str Строка для записи
 * @param color Цвет символов
 * @param bg_color Цвет фона
 */
void video_buffer_write_string(const char* str, uint8_t color, uint8_t bg_color);

/**
 * @brief Очищает буфер
 */
void video_buffer_clear(void);

/**
 * @brief Обновляет позицию курсора
 * @param pos Новая позиция курсора
 */
void video_buffer_update_cursor(int pos);

#endif /* KERNEL_VIDEO_BUFFER_H */ 