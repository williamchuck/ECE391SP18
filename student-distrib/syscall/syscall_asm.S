/*
 * syscall.S
 *
 *  Created on: Apr 7, 2018
 *      Author: Dennis
 */


#define ASM
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
/*
//doesn't seem like we can link user programs for now so leaving these commented out
#define GEN_CALL(func, num)\
.GLOBAL func;\
func:\
  pushl %ebx;\
  movl $num, %eax;\
  movl 4(%esp), %ebx;\
  movl 8(%esp), %ecx;\
  movl 12(%esp), %edx;\
  int $0x80;\
  popl %ebx;\
  ret

GEN_CALL(ece391_execute,SYS_EXECUTE)
GEN_CALL(ece391_halt,SYS_HALT)
GEN_CALL(ece391_read,SYS_READ)
GEN_CALL(ece391_write,SYS_WRITE)
GEN_CALL(ece391_open,SYS_OPEN)
GEN_CALL(ece391_close,SYS_CLOSE)
GEN_CALL(ece391_getargs, SYS_GETARGS)
*/

//segment descriptors include
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
  movl 12(%esp),%ecx    //get halt back ESP location
  movl 8(%esp),%edx 	//get user program stack pointer into edx
  //save callee saved regs and ebp for later return from halt
  pushl %ebx
  pushl %esi
  pushl %edi
  pushl %ebp
  movl %esp,(%ecx) //save halt back ESP
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

.GLOBAL halt_ret_exec
halt_ret_exec:
  movl 8(%esp),%eax //save status code
  movl 4(%esp),%esp //restore esp to status at jump to user
  //restore callee saved regs
  popl %ebp
  popl %edi
  popl %esi
  popl %ebx
  ret //return to syscall exec

//the largest syscall number
#define MAXSYSCALL SYS_GETARGS
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
  //save all regs except eax for return value
  pushl %ebx
  pushl %ecx
  pushl %edx
  pushl %esi
  pushl %edi

  //arg 1 2 3 in ebx ecx edx respectively
  //will always push 3 arguments, callee will take appropriate number
  pushl %edx
  pushl %ecx
  pushl %ebx
  call *syscall_table(,%eax,4)
  addl $12,%esp //pop arguments

  //restore regs
  popl %edi
  popl %esi
  popl %edx
  popl %ecx
  popl %ebx

  iret

syscall_table:
  .long 0 //nothing for entry 0
  .long system_halt
  .long system_execute
  .long system_read
  .long system_write
  .long system_open
  .long system_close
  .long system_getargs
  .long 0 //vidmap ditto
  .long 0 //set handler ditto
  .long 0 //sigreturn ditto
