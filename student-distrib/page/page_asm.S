.text

.global loadPageDir
.global enablePage
.global flush_TLB

loadPageDir:

	pushl %ebp
	movl %esp, %ebp

	mov 8(%esp), %eax
	mov %eax, %cr3

	leave
	ret

enablePage:
	
	pushl %ebp
	movl %esp, %ebp

	mov %cr0, %eax
	orl $0x80000000, %eax
	mov %eax, %cr0

	leave
	ret

flush_TLB:
	pushl %ebp
	movl %esp, %ebp

	mov %cr3, %eax
	mov %eax, %cr3

	leave
	ret
.end
