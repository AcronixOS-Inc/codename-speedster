/**
 * @file apic.c
 * @brief Реализация поддержки APIC/IOAPIC
 * 
 * Содержит функции для работы с современными контроллерами прерываний
 * APIC (Advanced Programmable Interrupt Controller) и IOAPIC.
 */

#include "apic.h"
#include "../video/video.h"
#include <stdint.h>

/* Глобальные структуры для хранения информации об APIC и IOAPIC */
static struct apic_info local_apic;
static struct ioapic_info io_apic;

/**
 * @brief Читает регистр APIC
 * @param reg Смещение регистра
 * @return Значение регистра
 */
uint32_t apic_read(uint32_t reg) {
    volatile uint32_t* apic_base = (volatile uint32_t*)local_apic.base_address;
    return apic_base[reg >> 2];
}

/**
 * @brief Записывает значение в регистр APIC
 * @param reg Смещение регистра
 * @param value Значение для записи
 */
void apic_write(uint32_t reg, uint32_t value) {
    volatile uint32_t* apic_base = (volatile uint32_t*)local_apic.base_address;
    apic_base[reg >> 2] = value;
}

/**
 * @brief Читает регистр IOAPIC
 * @param reg Смещение регистра
 * @return Значение регистра
 */
uint32_t ioapic_read(uint32_t reg) {
    volatile uint32_t* ioapic_base = (volatile uint32_t*)io_apic.base_address;
    ioapic_base[0] = reg;
    return ioapic_base[4];
}

/**
 * @brief Записывает значение в регистр IOAPIC
 * @param reg Смещение регистра
 * @param value Значение для записи
 */
void ioapic_write(uint32_t reg, uint32_t value) {
    volatile uint32_t* ioapic_base = (volatile uint32_t*)io_apic.base_address;
    ioapic_base[0] = reg;
    ioapic_base[4] = value;
}

/**
 * @brief Отправляет EOI (End of Interrupt) в APIC
 */
void apic_send_eoi(void) {
    apic_write(APIC_EOI_REG, APIC_EOI);
}

/**
 * @brief Настраивает вектор прерывания в IOAPIC
 * @param irq Номер IRQ
 * @param vector Вектор прерывания
 * @param flags Флаги настройки
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int ioapic_set_irq(uint8_t irq, uint8_t vector, uint32_t flags) {
    if (irq >= io_apic.max_redirection) {
        return -1;
    }

    uint32_t low = vector | flags;
    uint32_t high = 0; // Направляем на локальный APIC 0

    ioapic_write(IOAPIC_REDIR_TABLE_BASE + irq * 2, low);
    ioapic_write(IOAPIC_REDIR_TABLE_BASE + irq * 2 + 1, high);

    return 0;
}

/**
 * @brief Маскирует прерывание в IOAPIC
 * @param irq Номер IRQ
 */
void ioapic_mask_irq(uint8_t irq) {
    if (irq >= io_apic.max_redirection) {
        return;
    }

    uint32_t value = ioapic_read(IOAPIC_REDIR_TABLE_BASE + irq * 2);
    value |= IOAPIC_MASKED;
    ioapic_write(IOAPIC_REDIR_TABLE_BASE + irq * 2, value);
}

/**
 * @brief Размаскирует прерывание в IOAPIC
 * @param irq Номер IRQ
 */
void ioapic_unmask_irq(uint8_t irq) {
    if (irq >= io_apic.max_redirection) {
        return;
    }

    uint32_t value = ioapic_read(IOAPIC_REDIR_TABLE_BASE + irq * 2);
    value &= ~IOAPIC_MASKED;
    ioapic_write(IOAPIC_REDIR_TABLE_BASE + irq * 2, value);
}

/**
 * @brief Инициализирует APIC и IOAPIC
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int apic_init(void) {
    print_string("APIC Initialization... ");

    // TODO: В реальной системе нужно получить эти адреса из ACPI таблиц
    // Здесь используем фиксированные адреса для примера
    local_apic.base_address = 0xFEE00000;
    io_apic.base_address = 0xFEC00000;

    // Проверяем наличие APIC
    uint32_t version = apic_read(APIC_VERSION_REG);
    if ((version & 0xFF) == 0) {
        print_string_color("FAILED (APIC not found)\n", COLOR_RED, COLOR_BLACK);
        return -1;
    }

    // Получаем информацию о локальном APIC
    local_apic.id = (apic_read(APIC_ID_REG) >> 24) & 0xFF;
    local_apic.version = version & 0xFF;
    local_apic.max_lvt = ((version >> 16) & 0xFF) + 1;

    // Получаем информацию об IOAPIC
    io_apic.id = (ioapic_read(IOAPIC_ID_REG) >> 24) & 0xFF;
    io_apic.version = ioapic_read(IOAPIC_VERSION_REG) & 0xFF;
    io_apic.max_redirection = ((ioapic_read(IOAPIC_VERSION_REG) >> 16) & 0xFF) + 1;

    // Включаем APIC
    uint32_t svr = apic_read(APIC_SIVR_REG);
    apic_write(APIC_SIVR_REG, svr | APIC_SOFTWARE_ENABLED);

    // Настраиваем базовые прерывания
    for (int i = 0; i < io_apic.max_redirection; i++) {
        // Маскируем все прерывания по умолчанию
        ioapic_mask_irq(i);
    }

    // Настраиваем прерывание клавиатуры (IRQ1)
    // Используем вектор 0x21 (33) для совместимости с PIC
    print_string("Configuring keyboard interrupt (IRQ1)... ");
    ioapic_set_irq(1, 0x21, APIC_FIXED | IOAPIC_EDGE_TRIGGERED | IOAPIC_ACTIVE_HIGH);
    ioapic_unmask_irq(1);
    print_string_color("OK\n", COLOR_GREEN, COLOR_BLACK);

    print_string_color("APIC Initialization complete\n", COLOR_GREEN, COLOR_BLACK);
    return 0;
} 