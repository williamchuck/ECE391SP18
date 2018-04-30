#ifndef PIT_H_
#define PIT_H_

#include "../types.h"

/* Initialize PIT for interrupt */
extern void init_pit();

/* Change frequency of PIT */
extern void change_freq(uint32_t new_freq);

#endif
