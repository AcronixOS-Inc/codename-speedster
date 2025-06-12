/**
 * @file memory.h
 * @brief Базовые функции для работы с памятью
 */

#ifndef KERNEL_MEMORY_H
#define KERNEL_MEMORY_H

#include <stddef.h>

/**
 * @brief Копирует n байт из src в dest
 * @param dest Указатель на место назначения
 * @param src Указатель на источник
 * @param n Количество байт для копирования
 * @return Указатель на dest
 */
void* memcpy(void* dest, const void* src, size_t n);

/**
 * @brief Заполняет n байт начиная с dest значением c
 * @param dest Указатель на начало области
 * @param c Значение для заполнения
 * @param n Количество байт для заполнения
 * @return Указатель на dest
 */
void* memset(void* dest, int c, size_t n);

#endif /* KERNEL_MEMORY_H */ 