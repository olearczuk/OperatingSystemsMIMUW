global proberen, verhogen, proberen_time

extern get_os_time

section .text

;arguments: semaphore (edi), value (esi)
verhogen:
	lock add [edi], esi
	ret

;arguments: semaphore (edi), value (esi)
proberen: 
	cmp [edi], esi
	jl proberen 				;spinlock until semaphore >= value

	mov edx, esi				;edx = value
	neg esi					;esi = -value

	lock add [edi], esi   			;esi = semaphore, semaphore -= value
	js proberen_aux				;semaphore is < 0 so we need to reset it and go back to spinlock
	ret 			

proberen_aux:
	mov esi, edx				;esi = value
	lock add [edi], esi			;setting semaphore back to original value
	jmp proberen 				;back to spinlock

;arguments: semaphore (edi), value (esi)
;returns: amount of waiting time
proberen_time:
	push r15				;storing non-volatile register
	push rsi
	push rdi				;storing values of esi and edi

	call get_os_time
	mov r15, rax				;storing start time
	pop rdi
	pop rsi					;getting back values of esi and edi	
	
	call proberen
	
	call get_os_time			;getting end time
	sub rax, r15				;duration = end time - start time
	pop r15					;getting back value of non-volatile register
	ret
