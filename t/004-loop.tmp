	.intel_syntax
	.globl main
main:
	push	%rbp
	mov	%rbp, %rsp
	sub	%rsp, 16
	mov	DWORD PTR [%rbp-4], %edi
	mov	QWORD PTR [%rbp-16], %rsi
L10:
	push %rbp
	mov %rbp, %rsp
	sub %rsp, 24
	.comm	X, 8
	mov	%rax, 1
	mov X, %rax
	mov	%rax, 10
	mov QWORD PTR [%rbp-8], %rax
	mov %rax, 1
	mov QWORD PTR [%rbp-16], %rax
	mov %rax, QWORD PTR [%rbp-8]
	cmp %rax, X
	setg %al
	movzx %rdx, %al
	setl %al
	movzx %rax, %al
	sub %rdx, %rax
	mov QWORD PTR [%rbp-24], %rdx
G0:
L20:
	mov	%rax, X
	call	print
L30:
	mov %rax, X
	add %rax, QWORD PTR [%rbp-16]
	mov X, %rax
	cmp %rax, QWORD PTR [%rbp-8]
	setg %al
	movzx %rdx, %al
	setl %al
	movzx %rax, %al
	sub %rdx, %rax
	mov %rax, %rdx
	sub %rax, QWORD PTR [%rbp-24]
	jnz G0
	mov %rsp, %rbp
	pop %rbp
	mov	%eax, 0
	leave
	ret
