/**
 * @file kernel.c
 * @brief Главный файл ядра, точка входа
 */

#include "video/video.h"

/**
 * @brief Точка входа в ядро
 * 
 * Инициализирует необходимые подсистемы ядра и выводит
 * приветственное сообщение на экран.
 */
void kmain(void)
{
    // Очищаем экран
    clear_screen();
    
    // Выводим приветственное сообщение
    const char *welcome_msg = "Hello, World!";
    print_string(welcome_msg);
    
    return;
}