SYS_READ equ 0
SYS_OPEN equ 2
SYS_CLOSE equ 3
SYS_EXIT equ 60
O_RDONLY equ 0

section .bss
	buf resb 4096				;bufor
	arr resq 256				;tablica zliczająca wystąpienie każdego znaku

section .text
	global _start

%macro exit 1
	mov rax, SYS_CLOSE
	pop rdi
	syscall

	mov rax, 60
	mov rdi, %1
	syscall
%endmacro

_start:
	pop rdi
	pop rdi
	pop rdi
	mov rax, SYS_OPEN
	mov rsi, O_RDONLY
	mov rdx, 0644o
	syscall

	mov r8, 1 				;numer liczonej permutacji
	mov r9, 0 				;długość pierwszej permutacji
	mov r10, 0 				;długość aktualnej permutacji
	mov r11, 0 				;aktualny indeks w buforze
	mov r12, 0
	mov r13, arr				;zapamiętuje w rejestrze r13 adres arr, żeby przyspieszyć działanie
	push rax				;wrzucam deskryptor pliku na stos
	
_loop_buf:
	mov rdi, [rsp]
	mov rax, SYS_READ
	mov rsi, buf
	mov rdx, 4096
	syscall 				;pobieram dane z pliku do bufora
	
	mov r11, 0 				;ustawiam indeks na zero
	
	cmp rax, 0
	je _exit 				;sprawdzam czy jest koniec pliku

_loop:
	cmp r11, rax				;sprawdzam czy doszedłem do końca bufora
	jge _loop_buf
	
	mov r12b, byte [buf+r11]		;wyciągam bit z bufora
	inc r11					;zwiększam aktualny indeks w buforze

	cmp r8, 1				;sprawdzam czy jestem w pierwszej permutacji
	jne _loopv2

	inc r9					;jeśli tak to zwiększa długość 1 permutacji

_loopv2:
	inc r10 			 	;zwiększam długość aktualnej permutacji
	
	mov r14, [r13+8*r12]
	inc r14
	mov [r13+8*r12], r14 			;zwiększam licznik wystąpienia tego znaku

	cmp r14, r8
	jne _exit_err 				;sprawdzam czy licznik jest równy licznikowi permutacji

	cmp r12b, 0
	jne _loop 				;nie ma znaku nowej permutacji

	cmp r9, r10
	jne _exit_err	 			;sprawdzam czy ta sama długość permutacji akt co początkowej

	inc r8					;zwiększam licznik permutacji
 	mov r10, 0				;akt długość na 0
	jmp _loop 				;liczę następną permutację

_exit:
	cmp r10, 0 				;sprawdzam czy ostatnim znakiem był NULL
	jne _exit_err
	cmp r9, 0
	je _exit_err 				;sprawdzam czy plik był pusty
	exit 0

_exit_err:
	exit 1
