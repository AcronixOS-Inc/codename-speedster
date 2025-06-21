/**
 * @file heap.c
 * @brief Kernel Heap Allocator - динамическое выделение памяти для ядра
 * 
 * Реализация кучи ядра с поддержкой функций kmalloc(), kfree() и krealloc()
 * Использует связанный список блоков для управления памятью
 */

#include "memory.h"
#include "../video/video.h"

/* Глобальный экземпляр кучи ядра */
heap_t kernel_heap;

/* Минимальный размер блока (включая заголовок) */
#define MIN_BLOCK_SIZE (sizeof(heap_block_t) + 8)

/**
 * @brief Инициализация кучи ядра
 * @param start_addr Начальный адрес кучи
 * @param size Размер кучи в байтах
 */
void heap_init(uint32_t start_addr, uint32_t size) {
    print_string("Heap Initialization... ");
    
    /* Выравниваем адрес и размер */
    start_addr = align_up(start_addr, 8);
    size = align_down(size, 8);
    
    /* Инициализация структуры кучи */
    kernel_heap.start_addr = start_addr;
    kernel_heap.end_addr = start_addr + size;
    kernel_heap.total_size = size;
    kernel_heap.used_size = 0;
    
    /* Создаем первый свободный блок */
    heap_block_t *first_block = (heap_block_t*)start_addr;
    first_block->size = size - sizeof(heap_block_t);
    first_block->used = 0;
    first_block->next = NULL;
    first_block->prev = NULL;
    
    kernel_heap.first_block = first_block;
    
    print_string_color("OK\n", COLOR_GREEN, COLOR_BLACK);
    print_string("  - Start: 0x");
    print_hex(start_addr);
    print_string("\n  - Size: ");
    print_hex(size);
    print_string(" bytes\n");
}

/**
 * @brief Поиск подходящего блока для выделения
 * @param size Требуемый размер
 * @return Указатель на подходящий блок или NULL
 */
static heap_block_t* find_free_block(size_t size) {
    heap_block_t *current = kernel_heap.first_block;
    
    while (current != NULL) {
        if (!current->used && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

/**
 * @brief Разделение блока на два части
 * @param block Блок для разделения
 * @param size Размер первой части
 */
static void split_block(heap_block_t *block, size_t size) {
    if (block->size < size + MIN_BLOCK_SIZE) {
        return; /* Блок слишком мал для разделения */
    }
    
    /* Создаем новый блок */
    heap_block_t *new_block = (heap_block_t*)((uint8_t*)block + sizeof(heap_block_t) + size);
    new_block->size = block->size - size - sizeof(heap_block_t);
    new_block->used = 0;
    new_block->next = block->next;
    new_block->prev = block;
    
    /* Обновляем связи */
    if (block->next) {
        block->next->prev = new_block;
    }
    block->next = new_block;
    block->size = size;
}

/**
 * @brief Объединение блока с соседними свободными блоками
 * @param block Блок для объединения
 */
static void merge_blocks(heap_block_t *block) {
    /* Объединяем с следующим блоком */
    if (block->next && !block->next->used) {
        block->size += sizeof(heap_block_t) + block->next->size;
        block->next = block->next->next;
        if (block->next) {
            block->next->prev = block;
        }
    }
    
    /* Объединяем с предыдущим блоком */
    if (block->prev && !block->prev->used) {
        block->prev->size += sizeof(heap_block_t) + block->size;
        block->prev->next = block->next;
        if (block->next) {
            block->next->prev = block->prev;
        }
    }
}

/**
 * @brief Выделение памяти в куче ядра
 * @param size Размер для выделения
 * @return Указатель на выделенную память или NULL при ошибке
 */
void* kmalloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    
    /* Выравниваем размер */
    size = align_up(size, 8);
    
    /* Ищем подходящий блок */
    heap_block_t *block = find_free_block(size);
    if (!block) {
        return NULL; /* Нет свободного места */
    }
    
    /* Разделяем блок, если нужно */
    split_block(block, size);
    
    /* Помечаем блок как занятый */
    block->used = 1;
    kernel_heap.used_size += block->size;
    
    /* Возвращаем указатель на данные блока */
    return (void*)((uint8_t*)block + sizeof(heap_block_t));
}

/**
 * @brief Освобождение памяти в куче ядра
 * @param ptr Указатель на память для освобождения
 */
void kfree(void* ptr) {
    if (!ptr) {
        return;
    }
    
    /* Получаем блок из указателя */
    heap_block_t *block = (heap_block_t*)((uint8_t*)ptr - sizeof(heap_block_t));
    
    /* Проверяем, что блок находится в пределах кучи */
    if ((uint8_t*)block < (uint8_t*)kernel_heap.start_addr || 
        (uint8_t*)block >= (uint8_t*)kernel_heap.end_addr) {
        return;
    }
    
    /* Проверяем, что блок был занят */
    if (!block->used) {
        return;
    }
    
    /* Освобождаем блок */
    block->used = 0;
    kernel_heap.used_size -= block->size;
    
    /* Очищаем содержимое блока */
    memory_set(ptr, 0, block->size);
    
    /* Объединяем с соседними свободными блоками */
    merge_blocks(block);
}

/**
 * @brief Изменение размера выделенной памяти
 * @param ptr Указатель на память
 * @param new_size Новый размер
 * @return Указатель на память с новым размером или NULL при ошибке
 */
void* krealloc(void* ptr, size_t new_size) {
    if (!ptr) {
        return kmalloc(new_size);
    }
    
    if (new_size == 0) {
        kfree(ptr);
        return NULL;
    }
    
    /* Получаем текущий блок */
    heap_block_t *block = (heap_block_t*)((uint8_t*)ptr - sizeof(heap_block_t));
    
    /* Выравниваем новый размер */
    new_size = align_up(new_size, 8);
    
    /* Если новый размер меньше или равен текущему */
    if (new_size <= block->size) {
        return ptr;
    }
    
    /* Пытаемся расширить блок */
    if (block->next && !block->next->used) {
        size_t total_size = block->size + sizeof(heap_block_t) + block->next->size;
        if (total_size >= new_size) {
            /* Можем расширить блок */
            block->size = new_size;
            kernel_heap.used_size += (new_size - block->size);
            
            /* Обновляем следующий блок */
            if (total_size - new_size >= MIN_BLOCK_SIZE) {
                heap_block_t *new_next = (heap_block_t*)((uint8_t*)block + sizeof(heap_block_t) + new_size);
                new_next->size = total_size - new_size - sizeof(heap_block_t);
                new_next->used = 0;
                new_next->next = block->next->next;
                new_next->prev = block;
                
                if (block->next->next) {
                    block->next->next->prev = new_next;
                }
                block->next = new_next;
            } else {
                /* Следующий блок слишком мал, объединяем */
                block->size = total_size;
                block->next = block->next->next;
                if (block->next) {
                    block->next->prev = block;
                }
            }
            return ptr;
        }
    }
    
    /* Не можем расширить, выделяем новый блок */
    void* new_ptr = kmalloc(new_size);
    if (new_ptr) {
        memory_copy(new_ptr, ptr, block->size);
        kfree(ptr);
    }
    
    return new_ptr;
}

/**
 * @brief Вывод информации о состоянии кучи
 */
void heap_dump_info(void) {
    print_string("Kernel Heap Info:\n");
    print_string("  - Start: 0x");
    print_hex(kernel_heap.start_addr);
    print_string("\n  - End: 0x");
    print_hex(kernel_heap.end_addr);
    print_string("\n  - Total size: ");
    print_hex(kernel_heap.total_size);
    print_string(" bytes\n");
    print_string("  - Used size: ");
    print_hex(kernel_heap.used_size);
    print_string(" bytes\n");
    print_string("  - Free size: ");
    print_hex(kernel_heap.total_size - kernel_heap.used_size);
    print_string(" bytes\n");
    
    /* Подсчитываем количество блоков */
    uint32_t total_blocks = 0;
    uint32_t used_blocks = 0;
    heap_block_t *current = kernel_heap.first_block;
    
    while (current != NULL) {
        total_blocks++;
        if (current->used) {
            used_blocks++;
        }
        current = current->next;
    }
    
    print_string("  - Total blocks: ");
    print_hex(total_blocks);
    print_string("\n  - Used blocks: ");
    print_hex(used_blocks);
    print_string("\n  - Free blocks: ");
    print_hex(total_blocks - used_blocks);
    print_string("\n");
} 