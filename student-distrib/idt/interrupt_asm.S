/*
 * interrupt_asm.S
 *
 *  assembly linkage for IRQ
 */

.global common_interrupt
common_interrupt:
	pushl %eax			//save eax
	movl 4(%esp),%eax 	//move irq number to eax
	pushal 				//save all register
	pushl %eax 			//push argument
	call do_IRQ 		//process irq
	popl %eax
	popal
	popl %eax
	addl $4, %esp
	iret
