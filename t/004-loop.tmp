	.intel_syntax
	.globl main
main:
	push	%rbp
	mov	%rbp, %rsp
	sub	%rsp, 16
	mov	DWORD PTR [%rbp-4], %edi
	mov	QWORD PTR [%rbp-16], %rsi
L10:
	.comm	X, 8
	mov	%rax, 1
	mov	X, %rax
	mov	%rax, 10
	push	%rax
	mov	%rax, 1
	push	%rax
	test	%rax, %rax
	mov	%rcx, 1
	cmovg	%rax, %rcx
	mov	%rcx, -1
	cmovl	%rax, %rcx
	push	%rax
	jmp	G0
G1:
L20:
	mov	%rax, X
	call	print
L30:
	mov	%rax, QWORD PTR [%rsp+8]
	add	X, %rax
G0:
	mov	%rax, QWORD PTR [%rsp+16]
	cmp	X, %rax
	mov	%rcx, 1
	cmovg	%rax, %rcx
	mov	%rcx, 0
	cmove	%rax, %rcx
	mov	%rcx, -1
	cmovl	%rax, %rcx
	cmp	%rax, QWORD PTR [%rsp]
	jnz	G1
	add	%rsp, 24
	mov	%eax, 0
	leave
	ret
