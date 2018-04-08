/*
 * syscall.S
 *
 *  Created on: Apr 7, 2018
 *      Author: Dennis
 */

//syscall numbers
#include "ece391sysnum.h"
//generate userland function for syscall
//referred to ece391syscall.S
/*stack:
ret addr
arg1
arg2
arg3
*/

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
  movl 8(%esp),%ebx 	//get user program stack pointer into ebx
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
  pushl %ebx //ESP
  pushl $EXEC_FLAG //eflags
  pushl $USER_CS //CS
  pushl %eax //user prog entry

  iret



//the largest syscall number
#define MAXSYSCALL SYS_CLOSE
//system call linkage
/* Interrupt 128 - System Call */
.GLOBAL syscall_asm
syscall_asm:
  cmpl $MAXSYSCALL,%eax //check within range
  jbe 1f
  cmpl $SYS_HALT,%eax
  jae 1f
  //bad syscall number
  movl $-1,%eax
  iret
  1: //proceed to call
  
  pushl $0
  pushl $0
  pushl $0

  pushl %eax
  pushl %ebp
  pushl %edi
  pushl %esi
  pushl %edx
  pushl %ecx
  pushl %ebx

  //arg 1 2 3 in ebx ecx edx respectively
  //will always push 3 arguments, callee will take appropriate number
  pushl %edx
  pushl %ecx
  pushl %ebx
  call *syscall_table(,%eax,4)
  addl $12,%esp //pop arguments

  movl %eax, 24(%esp)

  popl %ebx
  popl %ecx
  popl %edx
  popl %esi
  popl %edi
  popl %ebp
  popl %eax

  addl $12, %esp

  iret

syscall_table:
  .long 0 //nothing for entry 0
  .long system_halt
  .long system_execute
  .long system_read
  .long system_write
  .long system_open
  .long system_close
  .long 0 //getargs not implemented yet
  .long 0 //vidmap ditto
  .long 0 //set handler ditto
  .long 0 //sigreturn ditto
