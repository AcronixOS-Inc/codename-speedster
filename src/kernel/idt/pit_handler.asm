; Обработчик прерывания системного таймера PIT (IRQ0)
; Вызывается при каждом тике таймера

global pit_handler_asm

extern pit_handler

pit_handler_asm:
    ; Сохраняем регистры
    pushad
    
    ; Вызываем C-обработчик
    call pit_handler
    
    ; Восстанавливаем регистры
    popad
    
    ; Возвращаемся из прерывания
    iret 