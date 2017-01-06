szOK:
	.string	"\nOK\n"
main:	
	call	PrintString
PrintString:
	mov	%rax, 1	; write(2)
	mov	%rdi, 1	; file descriptor 1
	mov	%rsi, OFFSET FLAT: szOK	; ptr to char
	mov	%rdx, ;  length of string 
	syscall
	.globl	main

