/**
 * @file apic.h
 * @brief Заголовочный файл для поддержки APIC/IOAPIC
 * 
 * Содержит определения для работы с современными контроллерами прерываний
 * APIC (Advanced Programmable Interrupt Controller) и IOAPIC.
 */

#ifndef KERNEL_APIC_H
#define KERNEL_APIC_H

#include <stdint.h>

/* APIC Register Offsets */
#define APIC_ID_REG              0x20
#define APIC_VERSION_REG         0x30
#define APIC_TPR_REG             0x80
#define APIC_EOI_REG             0xB0
#define APIC_SIVR_REG            0xF0
#define APIC_ICR_LOW_REG         0x300
#define APIC_ICR_HIGH_REG        0x310

/* IOAPIC Register Offsets */
#define IOAPIC_ID_REG            0x00
#define IOAPIC_VERSION_REG       0x01
#define IOAPIC_REDIR_TABLE_BASE  0x10

/* APIC Flags */
#define APIC_ENABLED             0x100
#define APIC_SOFTWARE_ENABLED    0x100
#define APIC_EOI                 0x00

/* IOAPIC Flags */
#define IOAPIC_MASKED            0x10000
#define IOAPIC_EDGE_TRIGGERED    0x8000
#define IOAPIC_ACTIVE_HIGH       0x2000
#define IOAPIC_LOGICAL_DEST      0x800

/* APIC Delivery Modes */
#define APIC_FIXED               0x000
#define APIC_LOWEST_PRIORITY     0x100
#define APIC_SMI                 0x200
#define APIC_NMI                 0x400
#define APIC_INIT                0x500
#define APIC_EXTINT              0x700

/**
 * @brief Структура для хранения информации об APIC
 */
struct apic_info {
    uint32_t base_address;    /**< Базовый адрес APIC */
    uint8_t id;              /**< ID локального APIC */
    uint8_t version;         /**< Версия APIC */
    uint8_t max_lvt;         /**< Максимальное количество LVT */
};

/**
 * @brief Структура для хранения информации об IOAPIC
 */
struct ioapic_info {
    uint32_t base_address;    /**< Базовый адрес IOAPIC */
    uint8_t id;              /**< ID IOAPIC */
    uint8_t version;         /**< Версия IOAPIC */
    uint8_t max_redirection; /**< Максимальное количество записей перенаправления */
};

/**
 * @brief Инициализирует APIC и IOAPIC
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int apic_init(void);

/**
 * @brief Отправляет EOI (End of Interrupt) в APIC
 */
void apic_send_eoi(void);

/**
 * @brief Настраивает вектор прерывания в IOAPIC
 * @param irq Номер IRQ
 * @param vector Вектор прерывания
 * @param flags Флаги настройки
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int ioapic_set_irq(uint8_t irq, uint8_t vector, uint32_t flags);

/**
 * @brief Маскирует прерывание в IOAPIC
 * @param irq Номер IRQ
 */
void ioapic_mask_irq(uint8_t irq);

/**
 * @brief Размаскирует прерывание в IOAPIC
 * @param irq Номер IRQ
 */
void ioapic_unmask_irq(uint8_t irq);

/**
 * @brief Читает регистр APIC
 * @param reg Смещение регистра
 * @return Значение регистра
 */
uint32_t apic_read(uint32_t reg);

/**
 * @brief Записывает значение в регистр APIC
 * @param reg Смещение регистра
 * @param value Значение для записи
 */
void apic_write(uint32_t reg, uint32_t value);

/**
 * @brief Читает регистр IOAPIC
 * @param reg Смещение регистра
 * @return Значение регистра
 */
uint32_t ioapic_read(uint32_t reg);

/**
 * @brief Записывает значение в регистр IOAPIC
 * @param reg Смещение регистра
 * @param value Значение для записи
 */
void ioapic_write(uint32_t reg, uint32_t value);

#endif /* KERNEL_APIC_H */ 