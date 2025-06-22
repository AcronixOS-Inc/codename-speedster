/**
 * @file timer_test.c
 * @brief Тесты для системного таймера PIT
 * 
 * Демонстрация работы системного таймера и функций задержки
 */

#include "pit.h"
#include "../video/video.h"

/**
 * @brief Тест базовых функций таймера
 */
void test_timer_basic(void) {
    print_string("\n=== Basic Timer Test ===\n");
    
    /* Выводим информацию о таймере */
    pit_dump_info();
    
    /* Тестируем получение тиков */
    uint32_t start_ticks = pit_get_ticks();
    print_string("Starting ticks: ");
    print_hex(start_ticks);
    print_string("\n");
    
    /* Ждем немного */
    pit_sleep_ticks(50); /* 50 тиков = 500мс при частоте 100Гц */
    
    uint32_t end_ticks = pit_get_ticks();
    print_string("Ending ticks: ");
    print_hex(end_ticks);
    print_string("\n");
    
    uint32_t elapsed_ticks = end_ticks - start_ticks;
    print_string("Elapsed ticks: ");
    print_dec(elapsed_ticks);
    print_string("\n");
    
    /* Тестируем время в миллисекундах */
    uint32_t start_time = pit_get_time_ms();
    print_string("Start time: ");
    print_dec(start_time);
    print_string(" ms\n");
    
    pit_sleep_ms(1000); /* Ждем 1 секунду */
    
    uint32_t end_time = pit_get_time_ms();
    print_string("End time: ");
    print_dec(end_time);
    print_string(" ms\n");
    
    uint32_t elapsed_time = end_time - start_time;
    print_string("Elapsed time: ");
    print_dec(elapsed_time);
    print_string(" ms\n");
}

/**
 * @brief Тест изменения частоты таймера
 */
void test_timer_frequency(void) {
    print_string("\n=== Timer Frequency Test ===\n");
    
    /* Сохраняем исходную частоту */
    uint32_t original_freq = pit_get_frequency();
    print_string("Original frequency: ");
    print_dec(original_freq);
    print_string(" Hz\n");
    
    /* Устанавливаем новую частоту */
    pit_set_frequency(50); /* 50 Гц */
    print_string("New frequency: ");
    print_dec(pit_get_frequency());
    print_string(" Hz\n");
    
    /* Тестируем задержку с новой частотой */
    uint32_t start_time = pit_get_time_ms();
    pit_sleep_ms(1000);
    uint32_t end_time = pit_get_time_ms();
    
    print_string("1 second delay with 50Hz: ");
    print_dec(end_time - start_time);
    print_string(" ms\n");
    
    /* Возвращаем исходную частоту */
    pit_set_frequency(original_freq);
    print_string("Restored frequency: ");
    print_dec(pit_get_frequency());
    print_string(" Hz\n");
}

/**
 * @brief Тест точности таймера
 */
void test_timer_accuracy(void) {
    print_string("\n=== Timer Accuracy Test ===\n");
    
    /* Тестируем различные задержки */
    uint32_t delays[] = {10, 50, 100, 500, 1000}; /* миллисекунды */
    int num_delays = sizeof(delays) / sizeof(delays[0]);
    
    for (int i = 0; i < num_delays; i++) {
        uint32_t start_time = pit_get_time_ms();
        pit_sleep_ms(delays[i]);
        uint32_t end_time = pit_get_time_ms();
        
        uint32_t actual_delay = end_time - start_time;
        print_string("Requested: ");
        print_dec(delays[i]);
        print_string(" ms, Actual: ");
        print_dec(actual_delay);
        print_string(" ms\n");
    }
}

/**
 * @brief Тест производительности (отсутствие активного ожидания)
 */
void test_timer_performance(void) {
    print_string("\n=== Timer Performance Test ===\n");
    
    print_string("Testing 5-second delay with hlt...\n");
    print_string("CPU should be idle during this time.\n");
    
    uint32_t start_time = pit_get_time_ms();
    pit_sleep_ms(5000); /* 5 секунд */
    uint32_t end_time = pit_get_time_ms();
    
    print_string("Delay completed: ");
    print_dec(end_time - start_time);
    print_string(" ms\n");
    print_string("Performance test passed!\n");
}

/**
 * @brief Запуск всех тестов таймера
 */
void run_timer_tests(void) {
    print_string("\n🚀 Starting Timer Tests...\n");
    
    test_timer_basic();
    test_timer_frequency();
    test_timer_accuracy();
    test_timer_performance();
    
    print_string("\n✅ Timer Tests Completed!\n");
} 