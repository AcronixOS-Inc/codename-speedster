SHELL := /bin/bash

# Компиляторы и утилиты
NASM := nasm
NASMFLAGS := -f elf32 -g
CC := gcc
CFLAGS := -m32 -c -ffreestanding -nostdlib -Wall -Wextra -g
LD := ld
LDFLAGS := -m elf_i386 -T linker.ld -o kernel
QEMU := qemu-system-i386
QEMUFLAGS_RUN := -kernel
QEMUFLAGS_DEBUG := -kernel kernel -s -S
GDB := gdb

# Директории
SRCDIR := src
BUILDDIR := build
KERNEL_DIR := $(BUILDDIR)/kernel

# Поиск исходников
ASM_SOURCES := $(shell find $(SRCDIR) -type f -name "*.asm")
C_SOURCES := $(shell find $(SRCDIR) -type f -name "*.c")

# Объектные файлы (в build/)
ASM_OBJECTS := $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%, $(ASM_SOURCES:.asm=.o))
C_OBJECTS := $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%, $(C_SOURCES:.c=.o))
OBJECTS := $(ASM_OBJECTS) $(C_OBJECTS)

# Основные цели
.PHONY: all clean run debug build_dir help

# Сборка ядра
all: $(BUILDDIR) $(KERNEL_DIR) $(OBJECTS) kernel
	@echo -e "\n✅ \033[1;32mГотово! Используйте:\033[0m"
	@echo -e "  🚀 \033[1;36mmake run\033[0m   — запустить в QEMU"
	@echo -e "  🐞 \033[1;35mmake debug\033[0m — отладка в GDB"

# Запуск в QEMU
run: kernel
	@echo -e "\n🚀 \033[1;36mЗапуск ядра в QEMU...\033[0m"
	@$(QEMU) $(QEMUFLAGS_RUN) kernel

# Отладка (QEMU + GDB)
debug: kernel
	@echo -e "\n🐞 \033[1;35mОтладка:\033[0m"
	@echo -e "  1. QEMU ждёт подключения GDB на порту :1234"
	@echo -e "  2. В новом терминале выполните: \033[1;33mgdb -x .gdbinit kernel\033[0m"
	@$(QEMU) $(QEMUFLAGS_DEBUG) kernel

# Создание build/
build_dir:
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(KERNEL_DIR)
	@mkdir -p $(dir $(ASM_OBJECTS)) $(dir $(C_OBJECTS))

# Линковка
kernel: $(OBJECTS)
	@echo -e "\n🔗 \033[1;34mЛинковка...\033[0m"
	@$(LD) $(LDFLAGS) $(OBJECTS)

# Правила компиляции
$(BUILDDIR)/%.o: $(SRCDIR)/%.asm | build_dir
	@echo -e "🔨 \033[1;34mASM:\033[0m $< → $@"
	@$(NASM) $(NASMFLAGS) $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | build_dir
	@echo -e "🔧 \033[1;34mC:\033[0m $< → $@"
	@$(CC) $(CFLAGS) $< -o $@

#  Очистка
clean:
	@echo -e "\n🧹 \033[1;31mУдаляю build/ и kernel...\033[0m"
	@rm -rf $(BUILDDIR) kernel

# Помощь
help:
	@echo -e "\n\033[1;35m📜 Помощь:\033[0m"
	@echo -e "  \033[1;36mmake all\033[0m    — собрать ядро"
	@echo -e "  \033[1;36mmake run\033[0m    — запустить в QEMU"
	@echo -e "  \033[1;36mmake debug\033[0m  — отладка (QEMU + GDB)"
	@echo -e "  \033[1;36mmake clean\033[0m  — очистить проект"
	@echo -e "  \033[1;36mmake help\033[0m   — эта справка"
	@echo -e "\n\033[3mУдачи в разработке! 🚀\033[0m"