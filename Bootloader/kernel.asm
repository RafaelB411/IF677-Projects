org 0x7e00
jmp 0x0000:start
 
data:
   ; TEXTO
   titulo db "FORCA", 8
   play db "Press 'space' to start", 8
   quit db "Press 'backspace' to exit", 8
   continue db "Press 'space' to continue", 8
   retry db "Press 'space' to retry", 8
   return db "Press 'backspace' to return", 8
   loser db "Voce nao tem a inteligencia", 8
   loser2 db "necessaria para viver.", 8
   loser3 db "RIP", 8
   winner db "Parabens", 8
   end1 db "Voce ainda ta aqui?", 8
   end2 db "Ja acabou.", 8
   end3 db "Vai pra casa!", 8
 
   ; PALAVRAS
   p1 db "PORTA", 8
   p2 db "HORARIO", 8
   p3 db "EMPATIA", 8
   p4 db "INERENTE", 8
   p5 db "PERSPICAZ", 8
   p6 db "AURELIO", 8
   p7 db "PECULIAR", 8
   p8 db "RESILIENCIA", 8
   p9 db "INTRINSECO", 8
   p10 db "PARCIMONIA", 8
 
   ; DADOS DO JOGO
   w1 db "_____", 8
   tam1 db 5
   w2 db "_______", 8
   tam2 db 7
   w3 db "_______", 8
   tam3 db 7
   w4 db "________", 8
   tam4 db 8
   w5 db "_________", 8
   tam5 db 9
   w6 db "_______", 8
   tam6 db 7
   w7 db "________", 8
   tam7 db 8
   w8 db "___________", 8
   tam8 db 11
   w9 db "__________", 8
   tam9 db 10
   w10 db "__________", 8
   tam10 db 10
   discard times 8 db " ", 8
   erro db 0
   score db 0
 
   ; CORES
   preto db 0
   vermelho db 4
   cinza db 7
   cinza_escuro db 8
   branco db 15
 
   ; DADOS DE PRINT
   minX dw 17
   minY dw 4
   paintInd dw 0
   celX dw 0
   celY dw 0
   offsetX dw 10
   offsetY dw 100
 
   ; MODELOS HANGMAN
   forca db 0, 0, 15, 15, 15, 15, 15, 15, 15, 15, 8, 0, 0, 15, 0, 0, 0, 0, 0, 15, 8, 0, 0, 15, 0, 0, 0, 0, 0, 15, 8, 0, 0, 15, 0, 8, 0, 0, 15, 0, 8, 0, 0, 15, 0, 8, 0, 0, 15, 0, 8, 0, 0, 15, 0, 8, 0, 0, 15, 0, 8, 0, 0, 15, 0, 8, 0, 0, 15, 0, 8, 0, 0, 15, 0, 8, 0, 0, 15, 0, 8, 0, 0, 15, 0, 8, 15, 15, 15, 15, 15, 9
   head db  15, 15, 15, 8, 15, 0, 15, 8, 15, 15, 15, 9
   body db 15, 8, 15, 8, 15, 8, 15, 9
   leftArm db  0,  15, 8,  15,  9
   rightArm db 15, 8, 0, 15, 9
   leftLeg db 15, 8, 15, 8, 15, 9
   rightLeg db 15, 8, 15, 8, 15, 9
 
  
%macro print 4
   ;seta as condicoes de print do texto
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
 
%macro transition 0
   ;executa a transicao entre as telas
   pusha
 
   call limpaTela
 
   mov ah, 0xb
   mov bh, 0
   mov bl, [vermelho]
   int 10h
 
   call delay
 
   mov ah, 0xb
   mov bh, 0
   mov bl, [preto]
   int 10h
 
   popa
%endmacro
 
%macro runGame 4  
   ;executa a logica central do jogo
   pusha
 
   transition
 
   mov cl, [erro] 
   mov dh, [score]
 
   mov di, %1
   call reset
 
   print %1, [branco], 13, 39
   print discard, [vermelho], 25, 38
 
   %%gameloop:
       ;estado central do jogo
       pusha 
       call check_paint 
       popa
 
       call getchar
       mov si, %2 
       mov di, %1 
       call compare 
 
       print %1, [branco], 13, 39
       print discard, [vermelho], 25, 38
 
       cmp cl, 6 
       je %%lose
       cmp dh, [%3] 
       je %%win  
 
       jmp %%gameloop
 
   %%lose:
       ;tela de derrota
       transition
 
       mov si, %1
       mov di, %2
       call recover

       print loser, [branco], 10, 27
       print loser2, [branco], 11, 29
 
       print retry, [cinza], 22, 29
       print return, [cinza], 23, 26

       call delay

       print loser3, [vermelho], 13, 38
          
       %%getlose:
           call getchar
           cmp al, 0x08
           je menu
           cmp al, 0x20
           je %4.retry 
           jmp %%getlose
 
   %%win:
       ;tela de vitoria
       transition
 
       mov si, %1
       mov di, %2
       call recover

       print winner, [branco], 12, 36
 
       print continue, [cinza], 22, 27
       print return, [cinza], 23, 26
 
       %%getwin:
           call getchar
           cmp al, 0x08
           je menu
           cmp al, 0x20
           je %4.move
           jmp %%getwin
 
   popa
%endmacro
 
start:
   xor ax, ax
   mov ds, ax
   mov es, ax
 
   mov ah, 0xb 
   mov bh, 0
   mov bl, [vermelho]
   int 10h
 
   call delay
 
menu:
   call limpaTela
 
   mov ah, 0xb
   mov bh, 0
   mov bl, [preto]
   int 10h
 
   print titulo, [vermelho], 9, 37
   print play, [cinza], 25, 29
   print quit, [cinza], 26, 28
 
   call getchar
   cmp al, 0x08
   je end
   cmp al, 0x20
   je palavra1
   jmp menu
 
palavra1:
   runGame w1, p1, tam1, palavra1 
   .retry:
       jmp palavra1
   .move:
 
palavra2:
   runGame w2, p2, tam2, palavra2
   .retry:
       jmp palavra2
   .move:

palavra3:
   runGame w3, p3, tam3, palavra3
   .retry:
       jmp palavra3
   .move:

palavra4:
   runGame w4, p4, tam4, palavra4
   .retry:
       jmp palavra3
   .move:

palavra5:
   runGame w5, p5, tam5, palavra5
   .retry:
       jmp palavra5
   .move:

palavra6:
   runGame w6, p6, tam6, palavra6
   .retry:
       jmp palavra6
   .move:

palavra7:
   runGame w7, p7, tam7, palavra7
   .retry:
       jmp palavra7
   .move:

palavra8:
   runGame w8, p8, tam8, palavra8
   .retry:
       jmp palavra8
   .move:

palavra9:
   runGame w9, p9, tam9, palavra9
   .retry:
        jmp palavra9
   .move:

palavra10:
   runGame w10, p10, tam10, palavra10
   .retry:
       jmp palavra10
   .move:

call limpaTela

getend:
   times 10 call delay
   print end1, [branco], 11, 30
   times 5 call delay
   print end2, [branco], 12, 35
   times 5 call delay
   print end3, [branco], 13, 33
   call delay
   print return, [cinza], 23, 26
   call getchar
   cmp al, 0x08
   je menu
   jmp getend

end:
   call limpaTela
   jmp $
 
getchar:
   mov ah, 0x00
   int 16h
ret
 
compare:
   ;funcao pra comparar o caractere lido do teclado com as letras da palavra
   mov dl, 0   
   mov bl, al  
   sub bl, 32  
 
   .loopcompare:
       lodsb   
       cmp al, 8 
       je .donecompare
       cmp al, bl
       jne .nextchar 
       inc dh 
       mov byte[di], bl 
       mov byte[si-1], ' '
       inc di 
       mov dl, 1 
       jmp .loopcompare
 
   .nextchar:
       inc di 
       jmp .loopcompare
 
   .donecompare:
       cmp dl, 1 
       je .back  
       mov di, discard 
       add di, cx 
       mov byte[di], bl 
       inc cl 
       add word [paintInd], 1 
       .back:
       ret
 
recover: 
   ;funcao que retorna a palavra fonte ao seu estado original
   xor cx, cx
 
   .looprecover:
       cmp byte[di], 8
       je .donerecover 
       cmp byte[di], ' ' 
       jne .continue 
       mov cl, byte[si]  
       mov byte[di], cl  
       .continue:
       inc di 
       inc si 
       jmp .looprecover
 
   .donerecover:
   ret
 
reset:
   ;reseta os dados do jogo
   xor cx, cx 
   mov word [paintInd], 0 
 
   .loopreset1:
       cmp byte[di], 8
       je .resetdiscard
       mov al, '_' 
       stosb 
       jmp .loopreset1
 
   .resetdiscard:
       mov di, discard 
 
   .loopreset2:
       cmp byte[di], 8
       je .donereset
       mov al, ' '
       stosb  
       jmp .loopreset2
 
   .donereset:
   ret
 
delay: 
   mov bx, 2000
   mov cx, 2000
 
   .delayloop:
       dec bx
       nop
       jnz .delayloop
   dec cx
   jnz .delayloop
   ret
 
check_paint:  
   ;verifica estado do hangman
   cmp word [paintInd], 0
   je .forca
   cmp word [paintInd], 1
   je .head
   cmp word [paintInd], 2
   je .body
   cmp word [paintInd], 3
   je .left_arm
   cmp word [paintInd], 4
   je .right_arm
   cmp word [paintInd], 5
   je .left_leg
      
   call setup_right_leg
   .left_leg:
   call setup_left_leg
   .right_arm:
   call setup_right_arm
   .left_arm:
   call setup_left_arm
   .body:
   call setup_body
   .head:
   call setup_head
   .forca:
   call setup_forca
   ret

setup_forca:
   mov word [minX], 0 
   mov word [minY], 0 
   mov si, forca
   call start_paint
   ret

setup_head:
   mov word [minX], 63
   mov word [minY], 27
   mov si, head
   call start_paint
   ret

setup_body:
   mov word [minX], 72
   mov word [minY], 54
   mov si, body
   call start_paint
   ret

setup_left_arm:
   mov word [minX], 54
   mov word [minY], 63
   mov si, leftArm
   call start_paint
   ret

setup_right_arm:
   mov word [minX], 81
   mov word [minY], 63
   mov si, rightArm
   call start_paint
   ret

setup_left_leg:
   mov word [minX], 63
   mov word [minY], 90
   mov si, leftLeg
   call start_paint
   ret

setup_right_leg:
   mov word [minX], 81
   mov word [minY], 90
   mov si, rightLeg
   call start_paint
   ret

start_paint:
   mov bh, 0
   mov ah, 0ch
   mov cx, [minX]
   mov dx, [minY]
   add cx, word [offsetX]
   add dx, word [offsetY]
   mov [minX], cx
   mov [minY], dx
   call paint
   ret

paint:
   lodsb
   cmp al, 8  
   je .row_done
   cmp al, 9  
   je .paint_done
   mov word [celX], cx
   mov word [celY], dx
   add word [celX], 9 
   add word [celY], 9 
   call paint_pixel
   jmp paint
 
   .row_done:
       add dx, 10 
       mov cx, [minX]
       jmp paint
 
  .paint_done:
       inc cl
       ret

paint_pixel:
   cmp cx, [celX]
   int 10h
   jge .cel_row_done
   inc cx
   jmp paint_pixel
   .cel_row_done:
       cmp dx, [celY]
       jge .cel_done
       inc dx
       sub cx, 9  
       jmp paint_pixel
 
   .cel_done:
       sub dx, 9  
       ret
 
limpaTela:
   ;limpa os caracteres escritos na tela
   mov dx, 0
   mov bh, 0     
   mov ah, 0x2
   int 0x10
 
   mov cx, 2500
   mov bh, 0
   mov al, 0x20
   mov ah, 0x9
   int 0x10
  
   mov dx, 0
   mov bh, 0     
   mov ah, 0x2
   int 0x10
   ret
 
print_string:
   lodsb
   cmp al,8
   je doneprint
 
   mov ah, 0xe
   mov bh, 0
   int 10h
 
   jmp print_string
 
   doneprint:
       mov ah, 0eh
       mov al, 0xd
       int 10h
       mov al, 0xa
       int 10h
       ret
 
 
 

