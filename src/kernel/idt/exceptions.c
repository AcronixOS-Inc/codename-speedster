/**
 * @file exceptions.c
 * @brief Реализация обработчика исключений процессора.
 */

#include "exceptions.h"
#include "../video/video.h"

// Сообщения для каждого типа исключений
const char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

/**
 * @brief Основной обработчик исключений, вызываемый из ассемблерных заглушек.
 * 
 * Выводит на экран информацию об исключении и останавливает систему.
 * @param regs Сохраненные регистры.
 */
void exception_handler(registers_t *regs)
{
    // Установка красного цвета для сообщения об ошибке
    set_color(COLOR_RED, COLOR_BLACK);
    
    print_string("\nEXCEPTION: ");
    
    if (regs->int_no < 32) {
        print_string((char*)exception_messages[regs->int_no]);
    } else {
        print_string("Unknown Exception");
    }

    print_string(" (");
    print_dec(regs->int_no);
    print_string(")\n");
    print_string("System Halted!\n");

    // Остановка системы
    for(;;);
} 