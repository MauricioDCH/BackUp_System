
section .data
    texto_bin db 'Binario: ', 0
    texto_hex db 'Hexadecimal: ', 0
    salto db 0xA, 0

    registros db 'EAX', 0, 'EBX', 0, 'ECX', 0, 'EDX', 0, 'ESI', 0, 'EDI', 0, 'ESP', 0, 'EBP', 0
    buffer_bin db 33 dup(0)  ; Buffer para binario (32 bits + terminador)
    buffer_hex db 9 dup(0)   ; Buffer para hexadecimal (8 dígitos + terminador)

section .text
    global _start

_start:
    ; Mostrar contenido de los registros
    call mostrar_registro, EAX, 'EAX'
    call mostrar_registro, EBX, 'EBX'
    call mostrar_registro, ECX, 'ECX'
    call mostrar_registro, EDX, 'EDX'
    call mostrar_registro, ESI, 'ESI'
    call mostrar_registro, EDI, 'EDI'
    call mostrar_registro, ESP, 'ESP'
    call mostrar_registro, EBP, 'EBP'

    ; Terminar el programa
    mov eax, 1  ; sys_exit
    xor ebx, ebx
    int 0x80

mostrar_registro:
    ; Imprimir nombre del registro
    mov eax, texto_bin
    call imprimir_texto

    ; Convertir y mostrar binario
    push eax  ; Guardar registro original
    mov ecx, buffer_bin
    call convertir_a_binario
    mov eax, buffer_bin
    call imprimir_texto
    pop eax  ; Restaurar registro original

    ; Convertir y mostrar hexadecimal
    mov eax, texto_hex
    call imprimir_texto
    mov ecx, buffer_hex
    call convertir_a_hexadecimal
    mov eax, buffer_hex
    call imprimir_texto

    ; Salto de línea
    mov eax, salto
    call imprimir_texto
    ret

convertir_a_binario:
    ; Convierte un registro de 32 bits a binario
    mov esi, 32  ; Número de bits
binario_loop:
    dec esi
    mov edx, 0
    shr eax, esi
    and al, 1
    add dl, '0'
    mov [ecx + esi], dl
    test esi, esi
    jnz binario_loop
    mov byte [ecx + 32], 0  ; Terminador
    ret

convertir_a_hexadecimal:
    ; Convierte un registro de 32 bits a hexadecimal
    mov esi, 8  ; Número de dígitos hexadecimales
hexadecimal_loop:
    dec esi
    mov edx, 0
    shr eax, esi * 4
    and al, 0xF
    cmp al, 10
    jl hex_digit
    add al, 'A' - 10
    jmp store_digit
hex_digit:
    add al, '0'
store_digit:
    mov [ecx + esi], al
    test esi, esi
    jnz hexadecimal_loop
    mov byte [ecx + 8], 0  ; Terminador
    ret

imprimir_texto:
    ; Imprime un texto terminado en NULL
    mov edx, 0
count_loop:
    cmp byte [eax + edx], 0
    je print_loop
    inc edx
    jmp count_loop
print_loop:
    mov eax, 4
    mov ebx, 1
    int 0x80
    ret
