	.intel_syntax
	.globl main
main:
	push	%rbp
	mov	%rbp, %rsp
	sub	%rsp, 16
	mov	DWORD PTR [%rbp-4], %edi
	mov	QWORD PTR [%rbp-16], %rsi
L100:
	.section .rodata
STR0:
	.string	"WELCOME TO BASIC"
	.section .text
	mov	%rax, 1
	mov	%rdi, 1
	mov	%rsi, OFFSET FLAT: STR0
	mov	%rdx, 16
	syscall
	call	newline
L110:
	.comm	X, 8
	mov	%rax, 42
	mov	X, %rax
L120:
	mov	%rax, X
	call	print
L130:
	mov	%rax, X
	push	%rax
	mov	%rax, 1
	pop	%rdx
	add	%rax, %rdx
	mov	X, %rax
L140:
	mov	%rax, X
	push	%rax
	mov	%rax, 10
	pop	%rdx
	cmp	%rdx, %rax
	jnl	J100
	jmp	L100
J100:
L150:
	push %rbp
	mov %rbp, %rsp
	sub %rsp, 24
	.comm	Y, 8
	mov	%rax, 1
	mov Y, %rax
	mov	%rax, 10
	mov [%rbp-8], %rax
	mov %rax, 1
	mov [%rbp-16], %rax
	mov %rax, Y
	cmp %rax, [%rbp-8]
	setg %al
	movzx %rdx, %al
	setl %al
	movzx %rax, %al
	sub %rdx, %rax
	mov [%rbp-24], %rdx
G0:
L160:
	.comm	Z, 8
	mov	%rax, Z
	push	%rax
	mov	%rax, 1
	pop	%rdx
	add	%rax, %rdx
	mov	Z, %rax
L170:
	mov %rax, Y
	add %rax, [%rbp+16]
	mov Y, %rax
	cmp %rax, [%rbp+8]
	setg %al
	movzx %rdx, %al
	setl %al
	movzx %rax, %al
	sub %rdx, %rax
	mov %rax, %rdx
	sub %rax, [%rbp+24]
	jnz G0
	mov %rsp, %rbp
	pop %rbp
	mov	%eax, 0
	leave
	ret
