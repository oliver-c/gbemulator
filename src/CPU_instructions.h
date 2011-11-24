#ifndef _CPU_INSTRUCTIONS_H_
#define _CPU_INSTRUCTIONS_H_

#include "CPU_type.h"

int CPU_NOP (CPU cpu);

int CPU_LD_B_n (CPU cpu);
int CPU_LD_C_n (CPU cpu);
int CPU_LD_D_n (CPU cpu);
int CPU_LD_E_n (CPU cpu);
int CPU_LD_H_n (CPU cpu);
int CPU_LD_L_n (CPU cpu);

int CPU_LD_A_A (CPU cpu);
int CPU_LD_A_B (CPU cpu);
int CPU_LD_A_C (CPU cpu);
int CPU_LD_A_D (CPU cpu);
int CPU_LD_A_E (CPU cpu);
int CPU_LD_A_H (CPU cpu);
int CPU_LD_A_L (CPU cpu);
int CPU_LD_A_aHL (CPU cpu);

int CPU_LD_B_B (CPU cpu);
int CPU_LD_B_C (CPU cpu);
int CPU_LD_B_D (CPU cpu);
int CPU_LD_B_E (CPU cpu);
int CPU_LD_B_H (CPU cpu);
int CPU_LD_B_L (CPU cpu);
int CPU_LD_B_aHL (CPU cpu);

#endif
