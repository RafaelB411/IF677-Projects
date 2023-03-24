org 0x7c00            
jmp main

data:
    string db "palavra", 0
    numero db "numero", 0

main:
xor ax, ax
mov ds, ax
mov es, ax

mov di, string
call gets

mov di, numero
call gets

mov si, numero
call StoI

mov si, string
dec al
add si, ax
lodsb
call putchar

call end

StoI:
    xor cx, cx
    xor ax, ax

    .loopstoi:
        push ax
        lodsb
        mov cl, al
        pop ax
        cmp cl, 0
        je .endloopstoi
        sub cl, 48
        mov bx, 10
        mul bx
        add ax, cx
        jmp .loopstoi

    .endloopstoi:
        ret


gets:
    xor cx, cx
    .loopgets:
        call getchar
        cmp al, 0x08
        je .backspace
        cmp al, 0x0d
        je .donegets
        cmp cl, 50
        je .loopgets
        stosb
        inc cl
        call putchar
        jmp .loopgets

    .backspace:
        cmp cl, 0
        je .loopgets
        dec di
        dec cl
        mov byte[di], 0
        call delchar
        jmp .loopgets

    .donegets:
        mov al, 0
        stosb
        call endl
    ret


delchar:
    mov al, 0x08
    call putchar
    mov al, ' '
    call putchar
    mov al, 0x08
    call putchar
    ret


getchar:
    mov ah, 0x00
    int 16h
    ret


putchar:
    mov ah, 0x0e
    int 10h
    ret


endl: 
    mov al, 0x0a
    call putchar
    mov al, 0x0d
    call putchar
    ret


end:
    jmp $

times 510-($-$$) db 0
dw 0xaa55