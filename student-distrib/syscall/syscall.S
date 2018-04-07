#include "syscall.h"

syscall_asm_linkage:
	

	pushad
	cmpl %eax, 2
	je execute

excute:
	
