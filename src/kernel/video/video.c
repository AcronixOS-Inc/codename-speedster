// video.c
#include "video.h"

#define VIDEO_MEMORY ((char*)0xB8000)
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT * 2)

// Текущая позиция курсора
static int cursor_x = 0;
static int cursor_y = 0;

// Вывод одного символа
void print_char(char c) {
    if (c == '\n') {  // Перенос строки
        cursor_x = 0;
        cursor_y++;
    } else {
        int pos = (cursor_y * SCREEN_WIDTH + cursor_x) * 2;
        VIDEO_MEMORY[pos] = c;
        VIDEO_MEMORY[pos + 1] = 0x07;  // Атрибут: светло-серый на чёрном
        cursor_x++;
    }

    // Если достигли конца строки — перенос
    if (cursor_x >= SCREEN_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }

    // Если достигли конца экрана — скроллинг (пока просто сброс)
    if (cursor_y >= SCREEN_HEIGHT) {
        clear_screen();
        cursor_x = 0;
        cursor_y = 0;
    }
}

// Установка позиции курсора
void set_cursor_pos(int x, int y) {
    cursor_x = x;
    cursor_y = y;
}

// Вывод строки (обновлённая версия)
void print_string(const char* str) {
    while (*str) {
        print_char(*str++);
    }
}

// Вывод приглашения "> "
void show_prompt(void) {
    print_string("> ");
}

// Очистка экрана (остаётся без изменений)
void clear_screen(void) {
    for (unsigned i = 0; i < SCREEN_SIZE; i += 2) {
        VIDEO_MEMORY[i] = ' ';
        VIDEO_MEMORY[i + 1] = 0x07;
    }
    cursor_x = 0;
    cursor_y = 0;
}