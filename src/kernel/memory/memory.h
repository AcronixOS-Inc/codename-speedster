/**
 * @file memory.h
 * @brief Менеджер памяти ядра
 * 
 * Реализация двухуровневой системы управления памятью:
 * 1. Physical Memory Manager (PMM) - управление физическими страницами
 * 2. Kernel Heap Allocator - динамическое выделение памяти для ядра
 */

#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stddef.h>

/* Константы памяти */
#define PAGE_SIZE 4096
#define PAGE_SHIFT 12
#define PAGE_MASK 0xFFFFF000

/* Максимальное количество страниц (4GB / 4KB = 1M страниц) */
#define MAX_PAGES 1048576
#define BITMAP_SIZE (MAX_PAGES / 32)

/* Состояния страницы */
#define PAGE_FREE 0
#define PAGE_USED 1

/* Типы выделения памяти */
typedef enum {
    HEAP_SMALL,   /* 1-64 байта */
    HEAP_MEDIUM,  /* 65-512 байт */
    HEAP_LARGE    /* 513+ байт */
} heap_type_t;

/* Структура блока кучи */
typedef struct heap_block {
    uint32_t size;           /* Размер блока */
    uint8_t used;            /* Флаг использования */
    struct heap_block *next; /* Следующий блок */
    struct heap_block *prev; /* Предыдущий блок */
} heap_block_t;

/* Структура кучи */
typedef struct {
    uint32_t start_addr;     /* Начальный адрес кучи */
    uint32_t end_addr;       /* Конечный адрес кучи */
    uint32_t total_size;     /* Общий размер кучи */
    uint32_t used_size;      /* Используемый размер */
    heap_block_t *first_block; /* Первый блок */
} heap_t;

/* Структура менеджера физической памяти */
typedef struct {
    uint32_t bitmap[BITMAP_SIZE]; /* Битовое поле для отслеживания страниц */
    uint32_t total_pages;         /* Общее количество страниц */
    uint32_t free_pages;          /* Количество свободных страниц */
    uint32_t kernel_end;          /* Конец ядра в памяти */
} pmm_t;

/* Глобальные переменные */
extern pmm_t physical_memory_manager;
extern heap_t kernel_heap;

/* Функции Physical Memory Manager */
void pmm_init(uint32_t kernel_end);
uint32_t pmm_alloc_page(void);
void pmm_free_page(uint32_t page_addr);
uint32_t pmm_get_free_pages_count(void);
void pmm_mark_page_used(uint32_t page_addr);
void pmm_mark_page_free(uint32_t page_addr);
void pmm_dump_info(void);

/* Функции Kernel Heap */
void heap_init(uint32_t start_addr, uint32_t size);
void* kmalloc(size_t size);
void kfree(void* ptr);
void* krealloc(void* ptr, size_t size);
void heap_dump_info(void);

/* Вспомогательные функции */
uint32_t align_up(uint32_t addr, uint32_t align);
uint32_t align_down(uint32_t addr, uint32_t align);
void memory_set(void* dest, uint8_t val, size_t count);
void memory_copy(void* dest, const void* src, size_t count);
int memory_compare(const void* ptr1, const void* ptr2, size_t count);
void* memory_find(const void* ptr, uint8_t value, size_t count);
int is_aligned(uint32_t addr, uint32_t align);
uint32_t get_page_size(void);
uint32_t get_page_number(uint32_t addr);
uint32_t get_page_offset(uint32_t addr);
uint32_t get_page_start(uint32_t addr);
uint32_t get_page_end(uint32_t addr);

/* Тестовые функции */
void run_memory_tests(void);

#endif /* MEMORY_H */ 