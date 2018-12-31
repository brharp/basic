	.intel_syntax
	.globl main
main:
	push	%rbp
	mov	%rbp, %rsp
	sub	%rsp, 16
	mov	DWORD PTR [%rbp-4], %edi
	mov	QWORD PTR [%rbp-16], %rsi
L10:
	mov	%rax, 0
	mov	%rdi, 0
	sub	%rsp, 2
	mov	%rsi, %rsp
	mov	%rdx, 1
	syscall
	xor	%rbx, %rbx
	pop	%dx
	mov	%bl, %dl
	.comm	I, 8
	lea	%rax, I
	mov	[%rax], %rbx
L20:
	mov	%rax, I
	call	print
L100:
	.comm	X, 8
	mov	%rax, 42
	mov	X, %rax
L110:
	mov	%rax, X
	call	print
L120:
	mov	%rax, X
	push	%rax
	mov	%rax, 1
	pop	%rdx
	add	%rax, %rdx
	mov	X, %rax
L130:
	mov	%rax, X
	push	%rax
	mov	%rax, 10
	pop	%rdx
	cmp	%rdx, %rax
	jnl	J100
	jmp	L100
J100:
L140:
	.comm	Y, 8
	mov	%rax, 1
	mov	Y, %rax
	mov	%rax, 10
	push	%rax
	test	%rax, %rax
	cmovg	%rax, 1
	cmovl	%rax, -1
	push	%rax
	jmp	G0
G1:
L150:
	.comm	Z, 8
	mov	%rax, Z
	push	%rax
	mov	%rax, 1
	pop	%rdx
	add	%rax, %rdx
	mov	Z, %rax
L160:
	add	Y, QWORD PTR [%rsp+8]
G0:
	cmp	Y, QWORD PTR [%rsp+16]
	mov	%rcx, 1
	cmovg	%rax, %rcx
	mov	%rcx, 0
	cmove	%rax, %rcx
	mov	%rcx, -1
	cmovl	%rax, %rcx
	cmp	%rax, QWORD PTR [%rsp]
	jnz	G1
	add	%rsp, 24
L170:
	.comm	A, 792
L180:
	mov	%rax, 42
	mov	QWORD PTR A[%rip+56], %rax
L190:
	mov	%rax, A[%rip+56]
	push	%rax
	mov	%rax, 1
	pop	%rdx
	add	%rax, %rdx
	call	print
	mov	%eax, 0
	leave
	ret
