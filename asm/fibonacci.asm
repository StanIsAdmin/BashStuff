;Computes the nth value of the Fibonacci series, with starting values a and b

EXTERN printf		; C function

SECTION .data
a:		dq	0		; 0th value of the series (as long int)
b:		dq	1		; 1st value of the series
n:		dq	22		; nth value to compute
fmt:	db "a: %ld, b: %ld, n: %ld, result: %ld", 10, 0	; printf format


SECTION .text
GLOBAL main			; entry point
main:
	MOV  RAX, [a]	; read parameters
	MOV  RBX, [b]
	MOV  RCX, [n]
	
	CMP  RCX, 0		; if n equals 0
	JE 	 preend		; the answer is in RAX (0)

loop:
	DEC  RCX		; decrement the counter (n)
	CMP  RCX, 0		; if RCX (counter) is null
	JE   end		; the answer is in RBX
	XCHG RAX, RBX	; n-1th value becomes n-2th value
	ADD  RBX, RAX	; add together n-1th and n-2th value
	JMP  loop		; start over

preend:
	XCHG RBX, RAX	; move answer from RAX to RBX
	
end:	
	PUSH RBP		; set up stack frame
	
	MOV  RDI, fmt	; arg 1 : printf format
	MOV  RSI, [a]	; arg 2 : parameter a
	MOV  RDX, [b]	; arg 3 : parameter b
	MOV  RCX, [n]	; arg 4 : parameter n
	MOV	 R8, RBX	; arg 5 : result (contained in B)
	MOV  RAX, 0		; no xmm used
	CALL printf		; display the result
	
	POP RBP			; restore stack
	MOV  RAX, 0		; system call number: sys_exit (return value)
	RET				; return

