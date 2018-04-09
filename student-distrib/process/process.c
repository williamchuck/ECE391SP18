#include "process.h"

process_desc_t process_desc_arr[6];

void init_process(){
	int i;
	for(i = 0; i < 6; i++)
		process_desc_arr[i].flag = 0;
}
