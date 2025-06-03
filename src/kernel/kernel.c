/**
 * @file kernel.c
 * @brief Главный файл ядра, точка входа
 */

#include "video/video.h"
#include "idt/idt.h"
#include "drivers/keyboard.h"

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
    
    // Выводим название ядра (зеленый текст на красном фоне)
    const char *kernel_name = "codename speedster\n";
    const char *kernel_msg = "(c) Acronium Foundation\n";
    print_string_color(kernel_name, COLOR_GREEN, COLOR_RED);
    
    // Выводим вторую строку
    print_string(kernel_msg);

    idt_init();
    keyboard_init();

    char input_line[256];
    int pos = 0;
    
    while(1) {
        char input = keyboard_read();
        if (input != 0) {
            if (input == '\n') {
                input_line[pos] = '\0';
                print_string("\n> ");
                pos = 0;
            } 
            else if (input == '\b') {
                if (pos > 0) {
                    pos--;
                    print_string("\b \b"); // Стираем символ
                }
            }
            else if (pos < sizeof(input_line) - 1) {
                input_line[pos++] = input;
                char str[2] = {input, '\0'};
                print_string(str);
            }
        }
    }

    return;
}