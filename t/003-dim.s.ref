	.intel_syntax
	.globl main
main:
	push	%rbp
	mov	%rbp, %rsp
	sub	%rsp, 16
	mov	DWORD PTR [%rbp-4], %edi
	mov	QWORD PTR [%rbp-16], %rsi
L10:
	.comm	X, 800
L20:
	mov	%rax, 42
	mov	QWORD PTR X[%rip+56], %rax
L30:
	mov	%rax, X[%rip+56]
	call	print
	mov	%eax, 0
	leave
	ret
