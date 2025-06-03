/**
 * @file keyboard.c
 * @brief Драйвер клавиатуры PS/2
 * 
 * Реализация драйвера клавиатуры с поддержкой:
 * - Основных символов
 * - Backspace
 * - Shift + символы
 * - Caps Lock
 */

#include "keyboard.h"
#include "../video/video.h"
#include "../idt/idt.h"

/* Порт данных клавиатуры */
#define KEYBOARD_DATA_PORT 0x60
/* Порт статуса клавиатуры */
#define KEYBOARD_STATUS_PORT 0x64

/* Буфер для хранения нажатых клавиш */
static char keyboard_buffer[256];
/* Текущая позиция в буфере */
static unsigned int buffer_position = 0;
/* Флаг нажатия Shift */
static int shift_pressed = 0;
/* Флаг состояния Caps Lock */
static int caps_lock = 0;

/**
 * Основная карта символов (без модификаторов)
 * Индекс - скан-код, значение - ASCII символ
 */
static const char keyboard_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-',
    '4', '5', '6', '+', '1', '2', '3', '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/**
 * Карта символов с нажатым Shift
 * Индекс - скан-код, значение - ASCII символ
 */
static const char keyboard_map_shift[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-',
    '4', '5', '6', '+', '1', '2', '3', '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/**
 * Чтение байта из порта ввода-вывода
 * @param port Номер порта
 * @return Прочитанный байт
 */
static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/**
 * Инициализация клавиатуры
 * Размаскировка прерывания клавиатуры в PIC
 */
void keyboard_init(void) {
    print_string("Keyboard Initialization... ");  // Добавлено: статусное сообщение
    
    unsigned char mask = inb(0x21) & 0xFD;
    write_port(0x21, mask);
    
    /* Упрощенная проверка - если маска изменилась */
    if ((inb(0x21) & 0x02) == 0) {
        print_string_color("OK\n", COLOR_GREEN, COLOR_BLACK);  // Добавлено: успешный статус
    } else {
        print_string_color("FAILED\n", COLOR_RED, COLOR_BLACK);  // Добавлено: статус ошибки
    }
}

/**
 * Обработчик прерывания клавиатуры
 * 
 * Читает скан-код нажатой клавиши, обрабатывает модификаторы
 * (Shift, Caps Lock) и помещает символ в буфер
 */
void keyboard_handler_main(void) {
    /* Чтение статуса клавиатуры */
    unsigned char status = inb(KEYBOARD_STATUS_PORT);
    
    /* Если есть данные для чтения */
    if (status & 0x01) {
        /* Чтение скан-кода */
        unsigned char keycode = inb(KEYBOARD_DATA_PORT);
        
        /* Обработка модификаторов */
        if (keycode == KEY_SHIFT || keycode == (KEY_SHIFT | KEY_RELEASED)) {
            /* Установка флага Shift */
            shift_pressed = (keycode != (KEY_SHIFT | KEY_RELEASED));
        }
        else if (keycode == KEY_CAPSLOCK && !(keycode & KEY_RELEASED)) {
            /* Переключение Caps Lock */
            caps_lock = !caps_lock;
        }
        /* Обработка обычных клавиш (только нажатие) */
        else if (!(keycode & KEY_RELEASED)) {
            char c = 0;
            
            /* Выбор карты символов в зависимости от модификаторов */
            if (shift_pressed || caps_lock) {
                c = keyboard_map_shift[keycode];
            } else {
                c = keyboard_map[keycode];
            }
            
            /* Добавление символа в буфер */
            if (c != 0) {
                keyboard_buffer[buffer_position++] = c;
                /* Защита от переполнения буфера */
                if (buffer_position >= sizeof(keyboard_buffer)) {
                    buffer_position = 0;
                }
            }
        }
    }
    
    /* Отправка сигнала EOI (End Of Interrupt) в PIC */
    write_port(0x20, 0x20);
}

/**
 * Чтение символа из буфера клавиатуры
 * @return Символ или 0, если буфер пуст
 */
char keyboard_read(void) {
    if (buffer_position > 0) {
        /* Извлечение символа из начала буфера */
        char key = keyboard_buffer[0];
        /* Сдвиг буфера */
        for (unsigned int i = 1; i < buffer_position; i++) {
            keyboard_buffer[i-1] = keyboard_buffer[i];
        }
        buffer_position--;
        return key;
    }
    return 0;
}

/**
 * Чтение строки с клавиатуры до нажатия Enter
 * @param buffer Буфер для сохранения строки
 * @param max_length Максимальная длина строки (включая нулевой символ)
 */
void read_line(char *buffer, unsigned int max_length) {
    unsigned int pos = 0;
    
    while(1) {
        char input = keyboard_read();
        if (input != 0) {
            if (input == '\n') {
                buffer[pos] = '\0'; // Завершаем строку
                print_string("\n"); // Переводим строку
                return;
            } 
            else if (input == '\b') {
                if (pos > 0) {
                    pos--;
                    print_string("\b \b"); // Стираем символ
                }
            }
            else if (pos < max_length - 1) {
                buffer[pos++] = input;
                char str[2] = {input, '\0'};
                print_string(str);
            }
        }
    }
}

