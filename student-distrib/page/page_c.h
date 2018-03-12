#ifndef PAGE_H_
#define PAGE_H_

extern void fill_page_dir_table();
extern void set_PDE(unsigned long i, unsigned long input);
extern void fill_page_table();
extern void setPTE(unsigned long i, unsigned long input);

#endif
