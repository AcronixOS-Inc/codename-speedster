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
    
    // Выводим приветственное сообщение с цветами (зеленый текст на красном фоне)
    const char *welcome_msg = "Hello, World!";
    print_string_color(welcome_msg, COLOR_GREEN, COLOR_RED);
    
    return;
}