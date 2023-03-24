org 0x7c00    
jmp main

%macro gets 1
	pusha

	mov di, %1
	xor cx, cx
	call _gets

	popa
%endmacro

%macro psh 3

	lodsb
	cmp %1,0
	je .endloop2
	inc %2
	push %3
	jmp .loop2

%endmacro

%macro poop 2

	cmp %1, 0
	je .endloop3
	dec %1
	pop %2
	stosb
	jmp .loop3

%endmacro

data:
	string db "string", 0

main:
	gets string
	mov si, string 
	call reverse
	mov si, string
	call printString
	jmp fim

_gets:
	.loop1:
		call getchar
		cmp al, 0x08
		je .backspace
		cmp al, 0x0d
		je .done
		cmp cl, 50
		je .loop1
		stosb
		inc cl
		call putchar
		jmp .loop1
	
	.backspace:
		cmp cl, 0
		je .loop1
		dec di
		dec cl
		mov byte[di], 0
		call delchar
		jmp .loop1

	.done:
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

printString:
	.loop:
		lodsb
		cmp al, 0

		je .endloop
		call putchar

		jmp .loop
	.endloop:
	ret

reverse:
	mov di, si
	xor cx, cx

	.loop2:
		psh al, cl, ax

	.endloop2:

	.loop3:
		poop cl, ax

	.endloop3:
	ret

endl:      
	mov al, 0x0a
	call putchar
	mov al, 0x0d
	call putchar
ret

fim:
	jmp $

times 510-($-$$) db 0
dw 0xaa55
