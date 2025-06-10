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
 * Управление светодиодами клавиатуры
 * @param leds Битовая маска светодиодов (LED_CAPS_LOCK, LED_NUM_LOCK, LED_SCROLL_LOCK)
 */
static void keyboard_set_leds(uint8_t leds) {
    // Ждем, пока клавиатура будет готова принять команду
    while (read_port(KEYBOARD_STATUS_PORT) & 0x02);
    write_port(KEYBOARD_DATA_PORT, KEYBOARD_CMD_SET_LEDS);
    
    // Ждем подтверждения
    while (read_port(KEYBOARD_STATUS_PORT) & 0x02);
    write_port(KEYBOARD_DATA_PORT, leds);
}

/**
 * Инициализация клавиатуры
 * Размаскировка прерывания клавиатуры в PIC
 */
void keyboard_init(void) {
    print_string("Keyboard Initialization... ");  // Добавлено: статусное сообщение
    
    unsigned char mask = read_port(0x21) & 0xFD;
    write_port(0x21, mask);

    // Инициализация светодиодов
    keyboard_set_leds(0);  // Все светодиоды выключены
    
    /* Упрощенная проверка - если маска изменилась */
    if ((read_port(0x21) & 0x02) == 0) {
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
    unsigned char status = read_port(KEYBOARD_STATUS_PORT);
    
    if (status & 0x01) {
        unsigned char keycode = read_port(KEYBOARD_DATA_PORT);
        
        // Обработка модификаторов
        if (keycode == KEY_SHIFT_LEFT || keycode == KEY_SHIFT_RIGHT || 
            keycode == (KEY_SHIFT_LEFT | KEY_RELEASED) || 
            keycode == (KEY_SHIFT_RIGHT | KEY_RELEASED)) {
            shift_pressed = !(keycode & KEY_RELEASED);
        }
        else if (keycode == KEY_CAPSLOCK && !(keycode & KEY_RELEASED)) {
            caps_lock = !caps_lock;
            // Обновляем светодиод
            keyboard_set_leds(caps_lock ? LED_CAPS_LOCK : 0);
        }
        // Обработка пробела
        else if (keycode == KEY_SPACE && !(keycode & KEY_RELEASED)) {
            if (buffer_position < sizeof(keyboard_buffer) - 1) {
                keyboard_buffer[buffer_position++] = ' ';
            }
        }
        // Обработка обычных клавиш
        else if (!(keycode & KEY_RELEASED) && keycode < 128) {
            if (keycode == KEY_TAB) {
                // Вставляем 4 пробела
                for (int i = 0; i < 4; i++) {
                    if (buffer_position < sizeof(keyboard_buffer) - 1) {
                        keyboard_buffer[buffer_position++] = ' ';
                    }
                }
            } else {
                char c = shift_pressed || caps_lock ? 
                       keyboard_map_shift[keycode] : 
                       keyboard_map[keycode];
                
                if (c != 0 && buffer_position < sizeof(keyboard_buffer) - 1) {
                    keyboard_buffer[buffer_position++] = c;
                }
            }
        }
    }
    
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
char* read_line(unsigned int max_length) {
    static char buffer[256];  // Статический буфер для хранения ввода
    unsigned int pos = 0;

    enable_cursor(0, 15);
    update_cursor(cursor_pos / 2);
    
    while(1) {
        char input = keyboard_read();
        if (input != 0) {
            if (input == '\n') {
                if (pos < max_length - 1) {
                    buffer[pos] = '\0';
                } else {
                    buffer[max_length - 1] = '\0';
                }
                print_string("\n");
                disable_cursor();
                return buffer;  // Возвращаем указатель на буфер
            } 
            else if (input == '\b') {
                if (pos > 0) {
                    pos--;
                    if (cursor_pos >= 2) {
                        cursor_pos -= 2;
                        VIDEO_MEMORY[cursor_pos] = ' ';
                        VIDEO_MEMORY[cursor_pos + 1] = 0x07;
                    } else {
                        cursor_pos = 0;
                    }
                }
                update_cursor(cursor_pos / 2);
            }
            else if (pos < max_length - 1) {
                buffer[pos++] = input;
                char str[2] = {input, '\0'};
                print_string(str);
                if (cursor_pos >= SCREEN_SIZE){
                    cursor_pos = SCREEN_SIZE - 2;
                }
                update_cursor(cursor_pos / 2);
            }
        }
    }
}