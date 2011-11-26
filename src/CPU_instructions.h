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
int CPU_LD_A_aBC (CPU cpu);
int CPU_LD_A_aDE (CPU cpu);
int CPU_LD_A_ann (CPU cpu);
int CPU_LD_A_hash (CPU cpu);

int CPU_LD_B_A (CPU cpu);
int CPU_LD_B_B (CPU cpu);
int CPU_LD_B_C (CPU cpu);
int CPU_LD_B_D (CPU cpu);
int CPU_LD_B_E (CPU cpu);
int CPU_LD_B_H (CPU cpu);
int CPU_LD_B_L (CPU cpu);
int CPU_LD_B_aHL (CPU cpu);

int CPU_LD_C_A (CPU cpu);
int CPU_LD_C_B (CPU cpu);
int CPU_LD_C_C (CPU cpu);
int CPU_LD_C_D (CPU cpu);
int CPU_LD_C_E (CPU cpu);
int CPU_LD_C_H (CPU cpu);
int CPU_LD_C_L (CPU cpu);
int CPU_LD_C_aHL (CPU cpu);

int CPU_LD_D_A (CPU cpu);
int CPU_LD_D_B (CPU cpu);
int CPU_LD_D_C (CPU cpu);
int CPU_LD_D_D (CPU cpu);
int CPU_LD_D_E (CPU cpu);
int CPU_LD_D_H (CPU cpu);
int CPU_LD_D_L (CPU cpu);
int CPU_LD_D_aHL (CPU cpu);

int CPU_LD_E_A (CPU cpu);
int CPU_LD_E_B (CPU cpu);
int CPU_LD_E_C (CPU cpu);
int CPU_LD_E_D (CPU cpu);
int CPU_LD_E_E (CPU cpu);
int CPU_LD_E_H (CPU cpu);
int CPU_LD_E_L (CPU cpu);
int CPU_LD_E_aHL (CPU cpu);

int CPU_LD_H_A (CPU cpu);
int CPU_LD_H_B (CPU cpu);
int CPU_LD_H_C (CPU cpu);
int CPU_LD_H_D (CPU cpu);
int CPU_LD_H_E (CPU cpu);
int CPU_LD_H_H (CPU cpu);
int CPU_LD_H_L (CPU cpu);
int CPU_LD_H_aHL (CPU cpu);

int CPU_LD_L_A (CPU cpu);
int CPU_LD_L_B (CPU cpu);
int CPU_LD_L_C (CPU cpu);
int CPU_LD_L_D (CPU cpu);
int CPU_LD_L_E (CPU cpu);
int CPU_LD_L_H (CPU cpu);
int CPU_LD_L_L (CPU cpu);
int CPU_LD_L_aHL (CPU cpu);

int CPU_aHL_A (CPU cpu);
int CPU_aHL_B (CPU cpu);
int CPU_aHL_C (CPU cpu);
int CPU_aHL_D (CPU cpu);
int CPU_aHL_E (CPU cpu);
int CPU_aHL_H (CPU cpu);
int CPU_aHL_L (CPU cpu);
int CPU_aHL_n (CPU cpu);

int CPU_aBC_A (CPU cpu);
int CPU_aDE_A (CPU cpu);
int CPU_ann_A (CPU cpu);

int CPU_LD_A_aC (CPU cpu);
int CPU_LD_aC_A (CPU cpu);

int CPU_LDD_A_aHL (CPU cpu);
int CPU_LDD_aHL_A (CPU cpu);
int CPU_LDI_A_aHL (CPU cpu);
int CPU_LDI_aHL_A (CPU cpu);

int CPU_LDH_an_A (CPU cpu);
int CPU_LDH_A_an (CPU cpu);

int CPU_LD_BC_nn (CPU cpu);
int CPU_LD_DE_nn (CPU cpu);
int CPU_LD_HL_nn (CPU cpu);
int CPU_LD_SP_nn (CPU cpu);

int CPU_LD_SP_HL (CPU cpu);
int CPU_LDHL_SP_n (CPU cpu);
int CPU_LD_ann_SP (CPU cpu);

int CPU_PUSH_AF (CPU cpu);
int CPU_PUSH_BC (CPU cpu);
int CPU_PUSH_DE (CPU cpu);
int CPU_PUSH_HL (CPU cpu);

int CPU_POP_AF (CPU cpu);
int CPU_POP_BC (CPU cpu);
int CPU_POP_DE (CPU cpu);
int CPU_POP_HL (CPU cpu);

#endif
