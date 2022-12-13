org 0x500
jmp 0x0000:start
 
;como o endereço dado para o kernel é 0x7e00, devemos
;utilizar o método de shift left (hexadecimal)
;e somar o offset no adress base, para rodarmos o kernel.

kernel db 'Motando Kernel...', 8
corda db 'Preparando corda...', 8
plateia db 'Esperando o pessoal...', 8
condenado db 'Amarrando o infeliz...', 8
titulo db 'FORCA', 8
preto db 0
verde db 2
vermelho db 4

%macro print 4
    pusha

    mov ah, 02h
    mov bh, 0
    mov dh, %3
    mov dl, %4
    int 10h

    mov si, %1
	mov bl, %2
    call print_string

    popa
%endmacro

delay:
    cmp al, '.'
    mov bp, 1500
	mov cx, 1500
    je .delaydot

    mov bp, 250
	mov cx, 250
	.delayloop:
		dec bp
		nop
		jnz .delayloop
	dec cx
	jnz .delayloop
    ret

    .delaydot:
        dec bp
        nop
        jnz .delaydot
    dec cx
    jnz .delaydot
    ret

limpaTela:
;; Limpa a tela dos caracteres colocados pela BIOS
	; Set the cursor to top left-most corner of screen
	mov dx, 0 
    mov bh, 0      
    mov ah, 0x2
    int 0x10

    ; print 2000 blanck chars to clean  
    mov cx, 2000 
    mov bh, 0
    mov al, 0x20 ; blank char
    mov ah, 0x9
    int 0x10
    
    ;Reset cursor to ton left-most corner of screen
    mov dx, 0 
    mov bh, 0      
    mov ah, 0x2
    int 0x10
ret

print_string:
	lodsb
	cmp al,8
	je end

	call delay

    mov ah, 0xe
    mov bh, 0
	int 10h

	jmp print_string

	end:
		mov ah, 0eh
		mov al, 0xd
		int 10h
		mov al, 0xa
		int 10h
		ret

start:
    xor ax, ax
    mov ds, ax
    mov es, ax

    ;parte pra printar as mensagens que quisermos
    mov ah, 0
    mov al, 12h
    int 10h

    mov ah, 0xb
    mov bh, 0
    mov bl, [preto]
    int 10h

    call limpaTela

    print kernel, [verde], 1, 2
    print corda, [verde], 2, 2
    print plateia, [verde], 3, 2
    print condenado, [verde], 4, 2

    call delay

    mov al, '.'
    call delay
    
    call limpaTela

    mov ah, 0xb
    mov bh, 0
    mov bl, [preto]
    int 10h

    ; print titulo, [vermelho], 9, 37

    mov al, '.'
    call delay

    reset:
        mov ah, 00h ;reseta o controlador de disco
        mov dl, 0   ;floppy disk
        int 13h

        jc reset    ;se o acesso falhar, tenta novamente

        jmp load_kernel

    load_kernel:
        ;Setando a posição do disco onde kernel.asm foi armazenado(ES:BX = [0x7E00:0x0])
        mov ax,0x7E0	;0x7E0<<1 + 0 = 0x7E00
        mov es,ax
        xor bx,bx		;Zerando o offset

        mov ah, 0x02 ;le o setor do disco
        mov al, 20  ;porção de setores ocupados pelo kernel.asm
        mov ch, 0   ;track 0
        mov cl, 3   ;setor 3
        mov dh, 0   ;head 0
        mov dl, 0   ;drive 0
        int 13h

        jc load_kernel ;se o acesso falhar, tenta novamente

        jmp 0x7e00  ;pula para o setor de endereco 0x7e00, que é o kernel

  


    times 510-($-$$) db 0 ;512 bytes
    dw 0xaa55	