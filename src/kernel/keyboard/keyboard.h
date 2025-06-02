/**
 * @file keyboard.h
 * @brief Драйвер клавиатуры PS/2 (сканирование и обработка нажатий)
 */

#ifndef KERNEL_KEYBOARD_H
#define KERNEL_KEYBOARD_H

// Инициализация клавиатуры
void init_keyboard(void);

// Ожидание ввода (для обработки прерываний)
void keyboard_wait_input(void);

#endif /* KERNEL_KEYBOARD_H */