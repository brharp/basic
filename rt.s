	.intel_syntax
	.globl	print
print:
	mov	%rdx, %rax      ; move rax to rdx
	sar	%rdx, 63        ;  and sign extend
	mov	%rcx, 10        ; base 10
	idiv	%rcx            ; rax = quotient, rdx = remainder
	push	%rax            ; save rax
	add	%dl, '0'        ; convert digit to ASCII
	push	%dx             ; push digit on stack
	mov	%rax, 1         ; write(2)
	mov	%rdi, 1         ;   file descriptor = 1
	mov	%rsi, %rsp      ;   pointer to char (top of stack)
	mov	%rdx, 1         ;   write 1 char
	syscall
	add	%rsp, 2         ; remove char from stack
	pop	%rax            ; restore rax
	cmp	%rax, 0         ; loop while
	jnz	print           ;  rax <> 0
	ret

