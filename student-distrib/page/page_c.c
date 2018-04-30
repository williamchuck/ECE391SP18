/*
 * page.c
 * Implementations of paging functions.
 * Author: Yichi Zhang.
 */
#include "page.h"

/*
 * physical memory layout
 * 0x00000000
 * 0x000b8000 (page 184)
 * VMEM
 * 0x000b9000 (page 185)
 * 0x00400000 (4 MB)
 * KERNEL IMAGE & STACK
 * 0x00800000 (8 MB)
 * pid0 not used for user programs, storing page tables here
 * 0x00C00000 (12MB)
 * USER PROG IMAGE & STACK (4MB*7) pid1
 * 0x01000000 (16MB)
 * pid2
 * 0x01400000 (20MB)
 * pid3
 * 0x01800000 (24MB)
 * pid4
 * 0x01C00000 (28MB)
 * pid5
 * 0x02000000 (32MB)
 * pid6
 * 0x02400000 (36 MB)
 * 0x02400000 (Term 0 vmem page)
 * 0x02401000 (Term 1 vmem page)
 * 0x02402000 (Term 2 vmem page)
 */
/* virtual memory layout
 * 0x00000000
 * not present
 * 0x000b8000 (page 184)
 * VMEM [lvl 0]
 * 0x000b9000 (page 185)
 * not present
 * 0x00400000 (4 MB)
 * KERNEL IMAGE & STACK [lvl 0]
 * 0x00800000 (8 MB)
 * page tables [lvl 0]
 * 0x00C00000 (12MB)
 * not present
 * 0x08000000 (128MB)
 * USER PROG IMAGE & STACK [lvl 3]
 * 0x08400000 (132MB)
 * user VMEM [lvl 3]
 * 0x08401000 (132MB+4KB)
 */
/* Page directory */
uint32_t  page_dir[TABLE_SIZE] __attribute__((aligned(PAGE_SIZE)));

typedef struct{
    uint32_t entry [TABLE_SIZE];
} page_table_t;


//page table array pointer
page_table_t * page_table_arr=(page_table_t*)(2*_4MB);

/*
 * fill_page_dir:
 * Description: Fill page directory table with zeros
 * Input: None
 * Output: None
 * Effect: Fill page directory table with zeros
 */
void fill_page_dir(){
	int i;
	/* Set all PDE to zero*/
	for(i = 0; i < TABLE_SIZE; i++){
	    //mark all page as 4MB first,
	    //so set_4KB will erase page table when setting 4KB page for the first time
		page_dir[i] = 0x0 | EN_PS;
	}
}

/*
 * fill_page_table:
 * Description: Fill page table with zeros
 * Input: None
 * Output: None
 * Effect: Fill page table with zeros
 */
void fill_page_table(page_table_t * page_table){
	int i;
	/* Set all PTE to zero */
	for(i = 0; i < TABLE_SIZE; i++){
		page_table->entry[i] = 0x0;
	}
}

/*
 * set_4MB:
 * Description: Set up 4MB page
 * Input: 4MB aligned physical memory we want to map to and a virtual memory
 *    we want to map from
 * Output: None
 * Effect: A 4MB page only accessable by kernel is setuped
 */
void set_4MB(uint32_t phys_mem, uint32_t virt_mem, uint8_t level){
	uint32_t pde_idx;

	/* Get first 10-bits of virtual memory as the index into page directory */
	pde_idx = virt_mem >> 22;
	/* Directly map a 4MB page into page directory */
    page_dir[pde_idx] = (phys_mem & _4MB_ADDR_MASK) | EN_P | EN_RW | EN_PS | ((level==3)? EN_US:0);

	/* Flush TLB for cache */
	flush_TLB();
}

/*
 * set_4KB:
 * Description: Set up 4KB page
 * Input: 4KB aligned physical memory we want to map to and a virtual memory
 *     we want to map from
 * Output: None
 * Effect: A 4KB page is set up
 */
void set_4KB(uint32_t phys_mem, uint32_t virt_mem, uint8_t level){
	uint32_t pde_idx, pte_idx;

	/* Get first 10-bits of virtual memory as the index into page directory */
	pde_idx = virt_mem >> 22;
	/* Get next 10-bits of virtual memory as the index into page table */
	pte_idx = (virt_mem & 0x003FFFFF) >> 12;

	//get corresponding page table for this page directory
	page_table_t* page_table=&page_table_arr[pde_idx];

	//if this was previously set as 4MB page, zero out corresponding page table first
	if(page_dir[pde_idx]&EN_PS)fill_page_table(page_table);

    /* Set PDE points to page table addr */
    page_dir[pde_idx] = ((uint32_t)page_table & PAGE_ADDR_MASK) | EN_P | EN_RW | ((level==3)? EN_US:0);
    /* Set PTE points to the pysical memory we want to map */
    page_table->entry[pte_idx] = (phys_mem & PAGE_ADDR_MASK) | EN_P | EN_RW | ((level==3)? EN_US:0);

	/* Flush TLB for cache */
	flush_TLB();
}

/*
 * free_4KB:
 * Description: Free a 4KB page
 * Input: virtual memory in that page
 * Output: None
 * Effect: Page contains the virtual memory is freed
 */
void free_4KB(uint32_t virt_mem){
    uint32_t pde_idx, pte_idx;

    /* Get first 10-bits of virtual memory as the index into page directory */
    pde_idx = virt_mem >> 22;
    /* Get next 10-bits of virtual memory as the index into page table */
    pte_idx = (virt_mem & 0x003FFFFF) >> 12;

    page_table_arr[pde_idx].entry[pte_idx]=0;

}


/*
 * free_4MB:
 * Description: Free a 4MB page
 * Input: virtual memory in that page
 * Output: None
 * Effect: Page contains the virtual memory is freed
 */
void free_4MB(uint32_t virt_mem){
    uint32_t pde_idx;

    /* Get first 10-bits of virtual memory as the index into page directory */
    pde_idx = virt_mem >> 22;
    page_dir[pde_idx]=0|EN_PS;//clear page directory entry, but maintaining 4MB page setting
}

/*
 * page_present:
 * Description: Decide if a page is present
 * Input: virtual memory in that page
 * Output: 1 for page is present, 0 for page not present
 * Effect: None
 */
int32_t page_present(const void* virt_mem){
    uint32_t pde_idx, pte_idx;

    uint32_t vmem=(uint32_t)virt_mem;
    /* Get first 10-bits of virtual memory as the index into page directory */
    pde_idx = vmem >> 22;
    /* Get next 10-bits of virtual memory as the index into page table */
    pte_idx = (vmem & 0x003FFFFF) >> 12;

    return (page_dir[pde_idx] & EN_P) && //page directory entry present
           ( (page_dir[pde_idx] & EN_PS) ? //4MB page?
              1 : //4MB page: if pde present then page present
             (page_table_arr[pde_idx].entry[pte_idx] & EN_P) );//4KB page: check pte present

}


/*
 * page_user:
 * Description: Decide if a page is in user mode
 * Input: virtual memory in that page
 * Output: 1 for page is in user mode, 0 for page not in user mode
 * Effect: None
 */
int32_t page_user(const void* virt_mem){
    uint32_t pde_idx, pte_idx;

    /* Check if the page is present first */
    if(!page_present(virt_mem))
        return 0;

    uint32_t vmem=(uint32_t)virt_mem;
    /* Get first 10-bits of virtual memory as the index into page directory */
    pde_idx = vmem >> 22;
    /* Get next 10-bits of virtual memory as the index into page table */
    pte_idx = (vmem & 0x003FFFFF) >> 12;

    return (page_dir[pde_idx] & EN_US) && //page directory entry for user
           ( (page_dir[pde_idx] & EN_PS) ? //4MB page?
              1 : //4MB page: if pde is in user than it is in user
             (page_table_arr[pde_idx].entry[pte_idx] & EN_US) );//4KB page: check pte is in user mode

}

/*
 * setup_page:
 * Description: Set up paging for kernel page and video memory
 * Input: None
 * Output: None
 * Effect: Two page is setup: One for kernel which is 4MB,
 *     one for video memory which is 4KB
 */
void setup_page(){
	/* Initialize page directory */
	fill_page_dir();

	/* Set up a 4MB page for kernel */
	set_4MB(KERNEL_MEM, KERNEL_MEM, 0);

	//set up 4MB page for page tables
	set_4MB(2*_4MB,2*_4MB,0);

	/* Set up a 4KB page for video memory */
	set_4KB(VIDEO_MEM, VIDEO_MEM, 0);

	/* Enable page */
	enablePage();
}
