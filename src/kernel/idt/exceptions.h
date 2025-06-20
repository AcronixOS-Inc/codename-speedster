/**
 * @file exceptions.h
 * @brief Заголовочный файл для обработчиков исключений процессора.
 */

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdint.h>

/**
 * @struct registers_t
 * @brief Структура для сохранения состояния регистров при прерывании.
 * 
 * Эта структура соответствует порядку, в котором регистры сохраняются
 * в ассемблерной заглушке `common_isr_stub`.
 */
typedef struct {
    uint32_t ds;                                    // Сегмент данных
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // pusha
    uint32_t int_no, err_code;                      // Номер прерывания и код ошибки
    uint32_t eip, cs, eflags, useresp, ss;          // Сохраняется процессором автоматически
} registers_t;

/**
 * @brief Основной обработчик исключений на C.
 * 
 * Вызывается из ассемблерных заглушек.
 * @param regs Указатель на структуру с сохраненными регистрами.
 */
void exception_handler(registers_t *regs);

#endif // EXCEPTIONS_H 