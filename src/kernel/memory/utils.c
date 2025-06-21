/**
 * @file utils.c
 * @brief Вспомогательные функции для работы с памятью
 * 
 * Реализация базовых операций с памятью: копирование, заполнение,
 * выравнивание адресов и размеров
 */

#include "memory.h"

/**
 * @brief Выравнивание адреса вверх
 * @param addr Адрес для выравнивания
 * @param align Граница выравнивания
 * @return Выровненный адрес
 */
uint32_t align_up(uint32_t addr, uint32_t align) {
    return (addr + align - 1) & ~(align - 1);
}

/**
 * @brief Выравнивание адреса вниз
 * @param addr Адрес для выравнивания
 * @param align Граница выравнивания
 * @return Выровненный адрес
 */
uint32_t align_down(uint32_t addr, uint32_t align) {
    return addr & ~(align - 1);
}

/**
 * @brief Заполнение области памяти значением
 * @param dest Указатель на начало области
 * @param val Значение для заполнения
 * @param count Количество байт для заполнения
 */
void memory_set(void* dest, uint8_t val, size_t count) {
    uint8_t* ptr = (uint8_t*)dest;
    for (size_t i = 0; i < count; i++) {
        ptr[i] = val;
    }
}

/**
 * @brief Копирование области памяти
 * @param dest Указатель на назначение
 * @param src Указатель на источник
 * @param count Количество байт для копирования
 */
void memory_copy(void* dest, const void* src, size_t count) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    
    /* Проверяем перекрытие областей */
    if (d < s || d >= s + count) {
        /* Копируем вперед */
        for (size_t i = 0; i < count; i++) {
            d[i] = s[i];
        }
    } else {
        /* Копируем назад для избежания перекрытия */
        for (size_t i = count; i > 0; i--) {
            d[i-1] = s[i-1];
        }
    }
}

/**
 * @brief Сравнение областей памяти
 * @param ptr1 Указатель на первую область
 * @param ptr2 Указатель на вторую область
 * @param count Количество байт для сравнения
 * @return 0 если области равны, отрицательное значение если ptr1 < ptr2,
 *         положительное значение если ptr1 > ptr2
 */
int memory_compare(const void* ptr1, const void* ptr2, size_t count) {
    const uint8_t* p1 = (const uint8_t*)ptr1;
    const uint8_t* p2 = (const uint8_t*)ptr2;
    
    for (size_t i = 0; i < count; i++) {
        if (p1[i] != p2[i]) {
            return (int)p1[i] - (int)p2[i];
        }
    }
    
    return 0;
}

/**
 * @brief Поиск символа в области памяти
 * @param ptr Указатель на область памяти
 * @param value Искомое значение
 * @param count Размер области в байтах
 * @return Указатель на найденное значение или NULL
 */
void* memory_find(const void* ptr, uint8_t value, size_t count) {
    const uint8_t* p = (const uint8_t*)ptr;
    
    for (size_t i = 0; i < count; i++) {
        if (p[i] == value) {
            return (void*)(p + i);
        }
    }
    
    return NULL;
}

/**
 * @brief Проверка, является ли адрес выровненным
 * @param addr Адрес для проверки
 * @param align Граница выравнивания
 * @return 1 если адрес выровнен, 0 в противном случае
 */
int is_aligned(uint32_t addr, uint32_t align) {
    return (addr & (align - 1)) == 0;
}

/**
 * @brief Получение размера страницы для адреса
 * @param addr Адрес
 * @return Размер страницы, содержащей этот адрес
 */
uint32_t get_page_size(void) {
    return PAGE_SIZE;
}

/**
 * @brief Получение номера страницы для адреса
 * @param addr Адрес
 * @return Номер страницы
 */
uint32_t get_page_number(uint32_t addr) {
    return addr >> PAGE_SHIFT;
}

/**
 * @brief Получение смещения в странице
 * @param addr Адрес
 * @return Смещение в странице
 */
uint32_t get_page_offset(uint32_t addr) {
    return addr & (PAGE_SIZE - 1);
}

/**
 * @brief Получение адреса начала страницы
 * @param addr Адрес
 * @return Адрес начала страницы
 */
uint32_t get_page_start(uint32_t addr) {
    return addr & PAGE_MASK;
}

/**
 * @brief Получение адреса конца страницы
 * @param addr Адрес
 * @return Адрес конца страницы
 */
uint32_t get_page_end(uint32_t addr) {
    return (addr & PAGE_MASK) + PAGE_SIZE - 1;
} 