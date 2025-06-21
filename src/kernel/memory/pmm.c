/**
 * @file pmm.c
 * @brief Physical Memory Manager - управление физическими страницами
 * 
 * Реализация менеджера физической памяти с использованием битовой карты
 * для отслеживания свободных и занятых страниц размером 4KB
 */

#include "memory.h"
#include "../video/video.h"

/* Глобальный экземпляр менеджера физической памяти */
pmm_t physical_memory_manager;

/**
 * @brief Инициализация менеджера физической памяти
 * @param kernel_end Адрес конца ядра в памяти
 */
void pmm_init(uint32_t kernel_end) {
    print_string("PMM Initialization... ");
    
    /* Инициализация структуры */
    physical_memory_manager.kernel_end = kernel_end;
    physical_memory_manager.total_pages = MAX_PAGES;
    physical_memory_manager.free_pages = MAX_PAGES;
    
    /* Очистка битовой карты */
    memory_set(physical_memory_manager.bitmap, 0, sizeof(physical_memory_manager.bitmap));
    
    /* Помечаем страницы ядра как занятые */
    uint32_t kernel_pages = (kernel_end + PAGE_SIZE - 1) >> PAGE_SHIFT;
    for (uint32_t i = 0; i < kernel_pages; i++) {
        pmm_mark_page_used(i << PAGE_SHIFT);
    }
    
    /* Помечаем первые 1MB как занятые (для BIOS, видеопамяти и т.д.) */
    uint32_t reserved_pages = 1024 * 1024 / PAGE_SIZE; /* 1MB / 4KB = 256 страниц */
    for (uint32_t i = 0; i < reserved_pages; i++) {
        pmm_mark_page_used(i << PAGE_SHIFT);
    }
    
    print_string_color("OK\n", COLOR_GREEN, COLOR_BLACK);
    print_string("  - Total pages: ");
    print_hex(physical_memory_manager.total_pages);
    print_string("\n  - Free pages: ");
    print_hex(physical_memory_manager.free_pages);
    print_string("\n");
}

/**
 * @brief Поиск свободной страницы в битовой карте
 * @return Индекс свободной страницы или -1, если нет свободных страниц
 */
static int find_free_page(void) {
    for (uint32_t i = 0; i < BITMAP_SIZE; i++) {
        if (physical_memory_manager.bitmap[i] != 0xFFFFFFFF) {
            /* Найдена группа с свободными страницами */
            uint32_t bitmap_entry = physical_memory_manager.bitmap[i];
            for (uint32_t j = 0; j < 32; j++) {
                if (!(bitmap_entry & (1 << j))) {
                    return i * 32 + j;
                }
            }
        }
    }
    return -1; /* Нет свободных страниц */
}

/**
 * @brief Выделение одной физической страницы
 * @return Адрес выделенной страницы или 0 при ошибке
 */
uint32_t pmm_alloc_page(void) {
    if (physical_memory_manager.free_pages == 0) {
        return 0; /* Нет свободных страниц */
    }
    
    int page_index = find_free_page();
    if (page_index == -1) {
        return 0; /* Не удалось найти свободную страницу */
    }
    
    uint32_t page_addr = page_index << PAGE_SHIFT;
    pmm_mark_page_used(page_addr);
    
    return page_addr;
}

/**
 * @brief Освобождение физической страницы
 * @param page_addr Адрес страницы для освобождения
 */
void pmm_free_page(uint32_t page_addr) {
    uint32_t page_index = page_addr >> PAGE_SHIFT;
    uint32_t bitmap_index = page_index / 32;
    uint32_t bit_index = page_index % 32;
    
    if (bitmap_index >= BITMAP_SIZE) {
        return; /* Некорректный адрес */
    }
    
    /* Проверяем, была ли страница занята */
    if (!(physical_memory_manager.bitmap[bitmap_index] & (1 << bit_index))) {
        return; /* Страница уже свободна */
    }
    
    /* Освобождаем страницу */
    physical_memory_manager.bitmap[bitmap_index] &= ~(1 << bit_index);
    physical_memory_manager.free_pages++;
    
    /* Очищаем содержимое страницы */
    memory_set((void*)page_addr, 0, PAGE_SIZE);
}

/**
 * @brief Получение количества свободных страниц
 * @return Количество свободных страниц
 */
uint32_t pmm_get_free_pages_count(void) {
    return physical_memory_manager.free_pages;
}

/**
 * @brief Пометить страницу как занятую
 * @param page_addr Адрес страницы
 */
void pmm_mark_page_used(uint32_t page_addr) {
    uint32_t page_index = page_addr >> PAGE_SHIFT;
    uint32_t bitmap_index = page_index / 32;
    uint32_t bit_index = page_index % 32;
    
    if (bitmap_index >= BITMAP_SIZE) {
        return; /* Некорректный адрес */
    }
    
    /* Проверяем, была ли страница свободна */
    if (!(physical_memory_manager.bitmap[bitmap_index] & (1 << bit_index))) {
        physical_memory_manager.bitmap[bitmap_index] |= (1 << bit_index);
        physical_memory_manager.free_pages--;
    }
}

/**
 * @brief Пометить страницу как свободную
 * @param page_addr Адрес страницы
 */
void pmm_mark_page_free(uint32_t page_addr) {
    uint32_t page_index = page_addr >> PAGE_SHIFT;
    uint32_t bitmap_index = page_index / 32;
    uint32_t bit_index = page_index % 32;
    
    if (bitmap_index >= BITMAP_SIZE) {
        return; /* Некорректный адрес */
    }
    
    /* Проверяем, была ли страница занята */
    if (physical_memory_manager.bitmap[bitmap_index] & (1 << bit_index)) {
        physical_memory_manager.bitmap[bitmap_index] &= ~(1 << bit_index);
        physical_memory_manager.free_pages++;
    }
}

/**
 * @brief Вывод информации о состоянии менеджера физической памяти
 */
void pmm_dump_info(void) {
    print_string("Physical Memory Manager Info:\n");
    print_string("  - Total pages: ");
    print_hex(physical_memory_manager.total_pages);
    print_string("\n  - Free pages: ");
    print_hex(physical_memory_manager.free_pages);
    print_string("\n  - Used pages: ");
    print_hex(physical_memory_manager.total_pages - physical_memory_manager.free_pages);
    print_string("\n  - Kernel end: 0x");
    print_hex(physical_memory_manager.kernel_end);
    print_string("\n");
} 