org 0x7c00
jmp start

data:
    string db "numero", 0

start:
xor ax, ax
mov ds, ax
mov es, ax

mov di, string
call gets

mov si, string
call StoI

call tAdd

mov di, string
call toString

mov si, string
call printString

call fim


tAdd:
    mov dx, ax
    mov cx, 0
    mov ax, 0

    .loopadd:
        cmp cx, dx
        je .endloopadd
        inc cx
        add ax, cx
        jmp .loopadd

    .endloopadd:
        ret


printString:
    .loopprint:
        lodsb
        cmp al, 0
        je .endloopprint
        call putchar
        jmp .loopprint

    .endloopprint:
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
        call endL
        
    ret


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


toString:
    push di
    .looptostr:
        cmp ax, 0
        je .endlooptostr
        xor dx, dx
        mov bx, 10
        div bx
        xchg ax, dx
        add ax, 48
        stosb
        xchg ax, dx
        jmp .looptostr

    .endlooptostr:
        pop si
        cmp si, di
        jne .donetostr
        mov al, 48
        stosb

    .donetostr:
        mov al, 0
        stosb
        call reverse
        ret


reverse:
    mov di, si
    xor cx, cx
    .looprvs1:
        lodsb
        cmp al, 0
        je .endlooprvs1
        inc cl
        push ax
        jmp .looprvs1
    
    .endlooprvs1:

    .looprvs2:
        cmp cl, 0
        je .endlooprvs2
        dec cl
        pop ax
        stosb
        jmp .looprvs2
    
    .endlooprvs2:
        ret


endL:
    mov al, 0x0a
    call putchar
    mov al, 0x0d
    call putchar
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


fim:
    jmp $


times 510-($-$$) db 0
dw 0xaa55