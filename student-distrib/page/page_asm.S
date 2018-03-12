/*
 * page_asm.S
 * Implementations of paging functions (enable page, flush TLB) in x86.
 * Author: Yichi Zhang.
 */
.text
# Set global functions
.global enablePage
.global flush_TLB

# Write to cr0,cr3,cr4 register
enablePage:
	# Set up Frame
	pushl %ebp
	movl %ebp, %esp

	# Load addr of page directory to cr3
	leal page_dir, %eax
	movl %eax, %cr3

	# Enable large paging
	movl %cr4, %eax
	orl 0x10, %eax
	movl %eax, %cr4

	# Enable paging
	movl %cr0, %eax
	orl $0x80000000, %eax
	movl %eax, %cr0

	# Restore frame and return
	leave
	ret

# Write addr of page diectory to TLB for cache
flush_TLB:

	# Set up Frame
	pushl %ebp
	movl %ebp, %esp

	# Reload addr of page dirctory to cr3 to flush TLB
	leal page_dir, %eax
	movl %eax, %cr3

	# Restore frame and return
	leave
	ret
.end
