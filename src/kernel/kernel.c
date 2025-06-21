/**
 * @file kernel.c
 * @brief Главный файл ядра - точка входа в систему
 */

#include "video/video.h"
#include "idt/idt.h"
#include "drivers/keyboard.h"
#include "memory/memory.h"

/* Внешние символы для определения размера ядра */
extern uint32_t _kernel_start;
extern uint32_t _kernel_end;

/**
 * @brief Точка входа в ядро операционной системы
 */
void kmain(void) 
{
    /* Инициализация видео-подсистемы */
    clear_screen();

    idt_init();         // Настройка таблицы прерываний
    keyboard_init();    // Инициализация драйвера клавиатуры
    
    /* Инициализация менеджера памяти */
    pmm_init((uint32_t)&_kernel_end);
    
    /* Инициализация кучи ядра (начинаем после ядра) */
    uint32_t heap_start = align_up((uint32_t)&_kernel_end + 1024 * 1024, PAGE_SIZE); /* 1MB после ядра */
    uint32_t heap_size = 1024 * 1024; /* 1MB для кучи */
    heap_init(heap_start, heap_size);
    
    /* Вывод информации о ядре */
    const char *kernel_name = "\ncodename speedster\n";
    const char *kernel_msg = "(c) Acronium Foundation 2025\n";
    
    // Название ядра с цветовым оформлением
    print_string_color(kernel_name, COLOR_GREEN, COLOR_RED);
    // Информация о копирайте
    print_string(kernel_msg);

    /* Запуск тестов менеджера памяти */
    //run_memory_tests();

    /**
     * @brief Основной цикл ядра с временным псевдо-терминалом
     * 
     * @note Временный псевдо-терминал доступен ТОЛЬКО в режиме разработки (DevTest)
     *       и будет удален в финальной версии. В production-среде терминальный ввод/вывод
     *       будет доступен исключительно из userspace через системные вызовы.
     */
    while(1) {
        /* Временное приглашение командной строки */
        print_string("$ ");
        
        /**
         * @dev Временная функция чтения ввода (только для DevTest)
         * 
         * @note В production-реализации:
         * - Ввод будет обрабатываться через механизм прерываний
         * - Буферизация будет осуществляться в пространстве пользователя
         * - Доступ к терминалу будет через стандартные дескрипторы (stdin/stdout)
         */
        char* user_input = read_line(128);
        
        if (user_input) {
            /* Здесь должен быть обработчик команд (временный) */
            /* В финальной версии будет переключение контекста */
            
            /* Освобождаем память, выделенную для ввода */
            kfree(user_input);
        }
    }
    
    /* Ядро никогда не должно достигать этой точки */
    __builtin_unreachable();
}