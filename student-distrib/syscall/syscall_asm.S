/*
 * syscall.S
 *
 *  Created on: Apr 7, 2018
 *      Author: Dennis
 */

#define ASM
#include "ece391sysnum.h"
#include "../x86_desc.h"

/* EFLAGS
31:24    23:16    15:8     7:0
0x00     0x20     0x30     0x02
00000000 00100000 00110000 00000010
 */

#define EXEC_FLAG 0x00203202
	
//take 2 stack argument of EIP and ESP for user program
.GLOBAL jump_to_user
jump_to_user:
	movl 4(%esp),%eax 	//get user program entry point address into eax
	movl 8(%esp),%edx 	//get user program stack pointer into edx

	/*http://www.c-jump.com/CIS77/ASM/Memory/M77_0290_segment_registers_protected.htm
	  segment selector format:
	  15:3  2  1:0
	  index ti rpl
	  ti: 1- GDT, 0- LDT
	 */
	/*iret stack:
	  ret addr
	  long CS
	  eflags
	  esp
	  long SS
	 */
	/*https://en.wikipedia.org/wiki/FLAGS_register
	  EFLAGS:
	  31:22    21 20  19  18 17 16 15 14 13:12 11 10 9  8  7  6  5    4  3    2  1 0
	  reserved ID VIP VIF AC VM RF 0  NT IOPL  OF DF IF TF SF ZF resv AF resv PF 1 CF
	  0        1  0   0   ?0 0  ?0 0  ?0 11    0  0  1  0  0  0   0   0   0   0  1 0
	 */
	//stack grows to lower address (up in a sense)

	//chang segment selectors for user program
	xorl %ecx,%ecx
	movw $USER_DS, %cx
	movw %cx, %ds
	movw %cx, %es
	movw %cx, %fs
	movw %cx, %gs

	//set up stack for iret

	pushl %ecx //SS
	pushl %edx //ESP
	pushl $EXEC_FLAG //eflags
	pushl $USER_CS //CS
	pushl %eax //user prog entry

	iret

//the largest syscall number
#define MAXSYSCALL SYS_VIDMAP
//system call linkage
/* Interrupt 128 - System Call */

.GLOBAL syscall_asm
syscall_asm:
	cmpl $MAXSYSCALL,%eax //check within range
	ja 1f
	cmpl $SYS_HALT,%eax
	jae 2f

	1: //bad syscall number
	movl $-1,%eax
	iret

	2: //proceed to call
	#Save orig_eax
	pushl %eax
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

	#Call do_sys
	pushl %esp
	call do_sys
	popl %esp

	#Context switch to user
	jmp return_to_user

.GLOBAL syscall_table

syscall_table:
.long 0 //nothing for entry 0
.long system_halt
.long system_execute
.long system_read
.long system_write
.long system_open
.long system_close
.long system_getargs
.long system_vidmap
.long 0 //set handler ditto
.long 0 //sigreturn ditto
