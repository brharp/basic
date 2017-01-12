	.intel_syntax
	.globl	_start

ok:		.ascii	"\nOK\n\0"
NL:    		.ascii  "\n"
nextchar:	# Return next char from input buffer. Set carry flag
		# if character is numeric. Set zero flag if char is null.
		inc	%rsi
		movb	%al, [%rsi]	
		cmp	%al, ':'
		jl	1f
		ret
1:		cmp	%al, ' '
		je	nextchar	# Skip spaces
		cmp	%al, '0'
		jl	1f
		stc			# Set carry if numeric
		inc	%al		# Test for null
		dec	%al		#   0+1-1=0 (sets zero flag)
  1: ret
main:
  # Print "OK"
  movw termx, 0 
  mov %rsi, offset flat: ok
  call printstring
getnonblankline:
  mov %rax, -1
  mov lineno, %rax # lineno=01
  # get a line of input
  call inputline
  call nextchar
  inc %al
  dec %al
  jz getnonblankline
  pushf # save carry flag
  call LineNumberFromStr
  ret
# 	Next is inputline, which reads a line of user input
# 	into a buffer. We read until a newline character is
# 	read (the user presses 'Enter'), skipping non-
# 	printable characters.
backspace:	dec	%bl
		dec	%rsi
		call	outchar
		jnz	inputnext
resetinput:	call	outchar
		call	newline
inputline:	lea 	%rsi, buffer
		movb	%bl, 1
inputnext:	call 	inputchar
		cmpb 	%al, '\n'
		jz 	terminateinput
		cmpb 	%al, ' '
		jl	inputnext
		cmpb	%al, '}'
		jg	inputnext
		cmpb	%al, 3		# Break
		je	resetinput
		cmpb	%al, 8		# Backspace
		je	backspace
		cmpb	%bl, 48		# compare char count to terminal width 
		movb	%al, 07		# ascii bell
		je	1f
		movb	[%rsi], %al
		inc	%rsi
		inc	%bl
1:		call	outchar
		jmp	inputnext
terminateinput:	movb	[%rsi], 0
		lea	%rsi, buffer-1
newline:	
  push %rsi
  mov %rsi, NL
  call outchar
  pop %rsi
  ret
printstring:	cmpb	[%rsi], 0
		jz	1f
		cmpb	[%rsi], '"'
		jz	1f
		call	outchar
		inc	%rsi
		jmp	printstring
1:		ret




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
	movb	%al, [%rsi]
	ret

LineNumberFromStr:
	dec	%rdi
	mov	%rax, 0
NextLineNumChar:
	call	nextchar
	jnc	ReturnFromLineNum
	push	%rdi
	pushf
	cmp	%rax, 65529
	jg	SyntaxError
ReturnFromLineNum:
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

