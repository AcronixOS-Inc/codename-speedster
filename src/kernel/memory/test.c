/**
 * @file test.c
 * @brief Тесты для менеджера памяти
 * 
 * Демонстрация работы Physical Memory Manager и Kernel Heap
 */

#include "memory.h"
#include "../video/video.h"

/**
 * @brief Тест Physical Memory Manager
 */
void test_pmm(void) {
    print_string("\n=== Physical Memory Manager Test ===\n");
    
    /* Выводим информацию о PMM */
    pmm_dump_info();
    
    /* Тестируем выделение страниц */
    print_string("\nAllocating pages...\n");
    uint32_t page1 = pmm_alloc_page();
    uint32_t page2 = pmm_alloc_page();
    uint32_t page3 = pmm_alloc_page();
    
    if (page1 && page2 && page3) {
        print_string("  - Page 1: 0x");
        print_hex(page1);
        print_string("\n  - Page 2: 0x");
        print_hex(page2);
        print_string("\n  - Page 3: 0x");
        print_hex(page3);
        print_string("\n");
        
        /* Заполняем страницы данными */
        memory_set((void*)page1, 0xAA, PAGE_SIZE);
        memory_set((void*)page2, 0xBB, PAGE_SIZE);
        memory_set((void*)page3, 0xCC, PAGE_SIZE);
        
        print_string("Pages filled with test data\n");
        
        /* Освобождаем страницы */
        print_string("Freeing pages...\n");
        pmm_free_page(page2);
        pmm_free_page(page1);
        pmm_free_page(page3);
        
        print_string("Pages freed successfully\n");
    } else {
        print_string_color("Failed to allocate pages!\n", COLOR_RED, COLOR_BLACK);
    }
    
    /* Выводим финальную информацию */
    print_string("Final PMM status:\n");
    pmm_dump_info();
}

/**
 * @brief Тест Kernel Heap
 */
void test_heap(void) {
    print_string("\n=== Kernel Heap Test ===\n");
    
    /* Выводим информацию о куче */
    heap_dump_info();
    
    /* Тестируем kmalloc */
    print_string("\nTesting kmalloc...\n");
    
    char* str1 = (char*)kmalloc(64);
    char* str2 = (char*)kmalloc(128);
    char* str3 = (char*)kmalloc(256);
    
    if (str1 && str2 && str3) {
        print_string("  - Allocated 64 bytes at 0x");
        print_hex((uint32_t)str1);
        print_string("\n  - Allocated 128 bytes at 0x");
        print_hex((uint32_t)str2);
        print_string("\n  - Allocated 256 bytes at 0x");
        print_hex((uint32_t)str3);
        print_string("\n");
        
        /* Заполняем строки */
        memory_copy(str1, "Hello from kernel heap!", 22);
        memory_copy(str2, "This is a longer string to test heap allocation", 45);
        memory_copy(str3, "And this is an even longer string to test larger allocations", 58);
        
        print_string("  - String 1: ");
        print_string(str1);
        print_string("\n  - String 2: ");
        print_string(str2);
        print_string("\n  - String 3: ");
        print_string(str3);
        print_string("\n");
        
        /* Тестируем krealloc */
        print_string("Testing krealloc...\n");
        char* new_str2 = (char*)krealloc(str2, 200);
        if (new_str2) {
            print_string("  - Reallocated to 200 bytes at 0x");
            print_hex((uint32_t)new_str2);
            print_string("\n  - Content: ");
            print_string(new_str2);
            print_string("\n");
            str2 = new_str2;
        }
        
        /* Освобождаем память */
        print_string("Freeing memory...\n");
        kfree(str1);
        kfree(str2);
        kfree(str3);
        
        print_string("Memory freed successfully\n");
    } else {
        print_string_color("Failed to allocate memory!\n", COLOR_RED, COLOR_BLACK);
    }
    
    /* Выводим финальную информацию */
    print_string("Final heap status:\n");
    heap_dump_info();
}

/**
 * @brief Запуск всех тестов менеджера памяти
 */
void run_memory_tests(void) {
    print_string("\n🚀 Starting Memory Manager Tests...\n");
    
    test_pmm();
    test_heap();
    
    print_string("\n✅ Memory Manager Tests Completed!\n");
} 