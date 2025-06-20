;
; Файл: exception_handlers.asm
; Описание: Обработчики-заглушки для первых 32 исключений процессора.
;

; Объявляем C-функцию как внешнюю, чтобы линковщик мог ее найти.
extern exception_handler

; Макрос для создания обработчика исключения, которое НЕ помещает код ошибки в стек.
%macro ISR_NO_ERR_CODE 1
[GLOBAL isr%1]
isr%1:
    cli          ; Запрещаем прерывания
    push 0       ; Помещаем в стек фиктивный код ошибки для унификации стекового фрейма
    push %1      ; Помещаем в стек номер прерывания
    jmp common_isr_stub
%endmacro

; Макрос для создания обработчика исключения, которое помещает код ошибки в стек.
%macro ISR_ERR_CODE 1
[GLOBAL isr%1]
isr%1:
    cli          ; Запрещаем прерывания
                 ; Код ошибки уже находится в стеке
    push %1      ; Помещаем в стек номер прерывания
    jmp common_isr_stub
%endmacro

;
; Определяем обработчики для всех 32 исключений
;
ISR_NO_ERR_CODE 0   ; 0: Деление на ноль
ISR_NO_ERR_CODE 1   ; 1: Отладка
ISR_NO_ERR_CODE 2   ; 2: Немаскируемое прерывание
ISR_NO_ERR_CODE 3   ; 3: Точка останова
ISR_NO_ERR_CODE 4   ; 4: Переполнение
ISR_NO_ERR_CODE 5   ; 5: Выход за границы
ISR_NO_ERR_CODE 6   ; 6: Неверный код операции (Opcode)
ISR_NO_ERR_CODE 7   ; 7: Устройство недоступно
ISR_ERR_CODE    8   ; 8: Двойная ошибка
ISR_NO_ERR_CODE 9   ; 9: (Зарезервировано)
ISR_ERR_CODE    10  ; 10: Неверный TSS
ISR_ERR_CODE    11  ; 11: Сегмент не найден
ISR_ERR_CODE    12  ; 12: Ошибка стека
ISR_ERR_CODE    13  ; 13: Общая ошибка защиты
ISR_ERR_CODE    14  ; 14: Ошибка страницы
ISR_NO_ERR_CODE 15  ; 15: (Зарезервировано)
ISR_NO_ERR_CODE 16  ; 16: Ошибка FPU
ISR_ERR_CODE    17  ; 17: Ошибка выравнивания
ISR_NO_ERR_CODE 18  ; 18: Ошибка машины
ISR_NO_ERR_CODE 19  ; 19: Ошибка SIMD
ISR_NO_ERR_CODE 20  ; 20: Ошибка виртуализации
ISR_NO_ERR_CODE 21  ; 21-31: (Зарезервировано)
ISR_NO_ERR_CODE 22
ISR_NO_ERR_CODE 23
ISR_NO_ERR_CODE 24
ISR_NO_ERR_CODE 25
ISR_NO_ERR_CODE 26
ISR_NO_ERR_CODE 27
ISR_NO_ERR_CODE 28
ISR_NO_ERR_CODE 29
ISR_NO_ERR_CODE 30
ISR_NO_ERR_CODE 31

;
; Общая заглушка, вызываемая всеми обработчиками
;
common_isr_stub:
    pusha       ; Сохраняем все регистры общего назначения (eax, ecx, edx, ebx, esp, ebp, esi, edi)
    
    mov ax, ds  ; Сохраняем сегмент данных
    push eax
    
    mov ax, 0x10 ; Загружаем селектор сегмента данных ядра (0x10)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Передаем указатель на стек (где теперь лежат регистры) в C-функцию
    push esp
    call exception_handler
    pop esp
    
    pop eax     ; Восстанавливаем исходный сегмент данных
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    popa        ; Восстанавливаем все регистры общего назначения
    add esp, 8  ; Очищаем стек от кода ошибки и номера прерывания
    iret        ; Возврат из прерывания (восстанавливает EIP, CS, EFLAGS) 