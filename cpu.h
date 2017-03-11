#ifndef CPU_H
#define CPU_H

void cpu_init();
void power_up(unsigned char *);
void cpu_cycle();
unsigned char *get_display();

#endif