#ifndef CPU_H
#define CPU_H

#include "config.h"

void cpu_init(struct config *config);
void power_up(unsigned char *);
void set_key(int index, int val);
void cpu_cycle();
unsigned char *get_display();

#endif