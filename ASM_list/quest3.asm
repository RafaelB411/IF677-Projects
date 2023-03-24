org 0x7c00            
jmp main

%macro compare 1
    pusha

    mov di, string
    mov si, %1
    call strcmp

    popa
%endmacro

%macro set 2

    mov si, %1
    mov bh, 0
    mov bl, %2
    call printString
    call end

%endmacro

data:
    string db "palavra", 0
    azul db "azul", 0
    vermelho db "vermelho", 0
    amarelo db "amarelo", 0
    verde db "verde", 0
    nonexiste db "nao existe", 0

main:
xor ax, ax
mov ds, ax
mov es, ax

mov ah, 0
mov al, 12h
int 10h

mov ah, 0xb
mov bh, 0
mov bl, 0h
int 10h

mov di, string
call gets
compare vermelho
je setred
compare verde
je setgrn
compare azul
je setblu
compare amarelo
je setyel
jmp setnon

setred:
    set vermelho, 0x4

setgrn:
    set verde, 0x2

setblu:
    set azul, 0x1

setyel:
    set amarelo, 0xe

setnon:
    set nonexiste, 0x5

printString:
    .loopprint:
        lodsb
        cmp al, 0
        je .endloopprint
        cmp al, ' '
        je .space
        sub ax, 32
        call putchar
        jmp .loopprint
    
    .space:
        call putchar
        jmp .loopprint

    .endloopprint:
        ret


strcmp:
	.loopstrcmp:
		lodsb
		cmp byte[di], 0
		jne .continue
		cmp al, 0
		jne .donestrcmp
		stc
		jmp .donestrcmp
		
	.continue:
		cmp al, byte[di]
    	jne .donestrcmp
		clc
		inc di
		jmp .loopstrcmp

	.donestrcmp:
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
        mov bh, 0
        mov bl, 0xf
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