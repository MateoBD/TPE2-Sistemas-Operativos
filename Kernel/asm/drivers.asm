GLOBAL get_key

EXTERN set_esc_flag

section .text

get_key:
    in al, 64h         ; Leer el estado del 8042
	test al, 1           ; Verificar si el bit 0 (hay datos) está en 1
	jz .exit        ; Si el bit 0 es 1, hay datos en el puerto 0x60h
	in al, 60h         ; Leer los datos desde el puerto 0x60h
    cmp al, 1
    je .save_registers
.exit:
	ret
.save_registers:
    call set_esc_flag
    ret