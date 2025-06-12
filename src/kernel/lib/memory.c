/**
 * @file memory.c
 * @brief Реализация базовых функций для работы с памятью
 */

#include "memory.h"

void* memcpy(void* dest, const void* src, size_t n) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    
    while (n--) {
        *d++ = *s++;
    }
    
    return dest;
}

void* memset(void* dest, int c, size_t n) {
    unsigned char* d = (unsigned char*)dest;
    
    while (n--) {
        *d++ = (unsigned char)c;
    }
    
    return dest;
} 