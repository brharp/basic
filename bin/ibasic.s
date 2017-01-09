	.intel_syntax
	.globl	_start

ok:	.ascii	"\nOK\n\0"
NL:     .ascii  "\n"

main:		movw	termx, 0	# Print "OK"
		mov	%rsi, offset flat: ok
		call	printstring
		mov	%rax, -1
		mov	lineno, %rax	# lineno=01
		call	inputline	# Get a line of input
		ret
# 	Next is inputline, which reads a line of user input
# 	into a buffer. We read until a newline character is
# 	read (the user presses 'Enter'), skipping non-
# 	printable characters.
inputline:	mov 	%rsi, offset flat: buffer
inputnext:	call 	inputchar
		cmpb 	[%rsi], '\n'
		jz 	terminateinput
		cmpb 	[%rsi], ' '
		jl	inputnext
		cmpb	[%rsi], '}'
		jg	inputnext
		cmpb	[%rsi], '^C'	# Break
		je	resetinput
		cmpb	[%rsi], '^H'	# Backspace
		je	backspace
	# compare char count to terminal width 
	# if greater than terminal width
	# 	ring bell (set outchar to ascii 7)
	# else
	#	move input char to output char
		call	outchar
		jmp	inputnext
		ret
terminateinput:	movb	[%rsi], 0
		lea	%rsi, buffer-1
newline:	mov	%rsi, NL
		call	outchar
		ret
printstring:	cmpb	[%rsi], 0
	jz	1f
	cmpb	[%rsi], '"'
	jz	1f
	call	outchar
	inc	%rsi
	jmp	printstring
1:	ret




# outchar - prints a character to the terminal
#   %rsi is the address of a character to print

outchar:
	mov	%rax, termx #load X position
	cmp	%rax, 48         #edge of screen?
	jnz	1f
	call	newline
1:	inc	%rax             # update terminal position
	mov	termx, %rax
	mov	%rax, 1          # write(2)
	mov	%rdi, 1	         # fd (1 is stdout)
	mov	%rdx, 1          # count
	syscall
	ret


# inputchar - get one char of input from terminal into buffer at %rsi

inputchar:
	mov	%rax, 0          # read(2)
	mov	%rdi, 0          # fd (0 is stdin)
	mov	%rdx, 1          # count
	syscall
	ret

_start:
	push	%rbp
	mov	%rbp, %rsp
     	xor	%rax, %rax
	call	main
	mov	%rax, 60
	xor	%rdi, %rdi
     	syscall

# ---------------------------------------------------------------------

	.data

termx:	.quad	0
lineno:	.quad	0
	.comm	buffer, 72

