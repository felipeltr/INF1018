
.globl foo
foo:
	push %ebp
	mov %esp, %ebp
	jmp L1

	subl $40, %esp

	movl $0, 4(%ebp)
	movl $0, 8(%ebp)
	movl $0, 12(%ebp)
	jmp L1


RMOV:
	movl $0, %ecx
	movl $1, %ecx
	movl $10000, %ecx
	movl -4(%ebp), %ecx
	movl 8(%ebp), %ecx
RADD:
	addl $0, %ecx
	addl $10000, %ecx
	addl $10, %ecx
	addl -4(%ebp), %ecx
	addl 8(%ebp), %ecx
RSUB:
	subl $0, %ecx
	subl $10000, %ecx
	subl $10, %ecx
	subl -4(%ebp), %ecx
	subl 8(%ebp), %ecx
RMUL:
	imul $0, %ecx
	imul $10000, %ecx
	imul $10, %ecx
	imul -4(%ebp), %ecx
	imul 8(%ebp), %ecx
RMOVBACK:
	mov %ecx, 8(%ebp)
	mov %ecx, -4(%ebp)

RMOVEAX:
	movl %eax, 8(%ebp)
	movl %eax, -4(%ebp)

RCMP:
	cmpl $0, 8(%ebp)
	jne L1
	cmpl $0, -4(%ebp)
	jne L1
RTEST:
	cmpl $0, 4(%ebp)
	jne LAF
	movl 8(%ebp), %eax
	jmp L1
LAF:


	pushl $10000
	pushl -4(%ebp)
	pushl 8(%ebp)
TST:
	call L1
	call TST

	addl $8, %esp




	cmpl $0, 4(%ebp)
	je L1
L1:
	mov %ebp, %esp
	pop %ebp
	ret
	jmp L1
