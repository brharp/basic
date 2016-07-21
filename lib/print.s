	.intel_syntax
	.globl	print
print:
	push	%rbp            # Save base pointer
	mov	%rbp, %rsp      # Set base pointer to stack pointer
print1:
	mov	%rdx, %rax      # move rax to rdx
	sar	%rdx, 63        #  and sign extend
	mov	%rcx, 10        # base 10
	idiv	%rcx            # rax = quotient, rdx = remainder
	add	%dl, '0'        # convert digit to ASCII
	push	%dx             # push digit on stack
	cmp	%rax, 0         # loop while
	jnz	print1          #  rax <> 0
print2:
	mov	%rax, 1         # write(2)
	mov	%rdi, 1         #   file descriptor = 1
	mov	%rsi, %rsp      #   pointer to char (top of stack)
	mov	%rdx, 1         #   write 1 char
	syscall
        add	%rsp, 2         # remove char from stack
	cmp	%rsp, %rbp      # stack empty?
	jnz	print2
	pop	%rbp            # Restore base pointer
	ret

