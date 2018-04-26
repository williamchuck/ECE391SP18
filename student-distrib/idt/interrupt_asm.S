/*
 * interrupt_asm.S
 *  assembly linkage for IRQ
 *  Author: Hongxuan Li.
 */
/* Global address of irq interrupt handling */
.global common_interrupt
.global return_to_user
common_interrupt:
	#Save registers for linkage
	pushl $0
	movw %fs, (%esp)
	pushl $0
	movw %es, (%esp)
	pushl $0
	movw %ds, (%esp)
	pushl %eax
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %edx
	pushl %ecx
	pushl %ebx

	#Call do_IRQ
	pushl %esp
	call do_IRQ
	popl %esp

return_to_user:
	#Restore registers from linkage
	popl %ebx
	popl %ecx
	popl %edx
	popl %esi
	popl %edi
	popl %ebp
	popl %eax
	movw (%esp), %ds
	addl $4, %esp
	movw (%esp), %es
	addl $4, %esp	
	movw (%esp), %fs
	addl $4, %esp

	addl $4, %esp
	iret
