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

int CPU_ADD_A_A (CPU cpu);
int CPU_ADD_A_B (CPU cpu);
int CPU_ADD_A_C (CPU cpu);
int CPU_ADD_A_D (CPU cpu);
int CPU_ADD_A_E (CPU cpu);
int CPU_ADD_A_H (CPU cpu);
int CPU_ADD_A_L (CPU cpu);
int CPU_ADD_A_aHL (CPU cpu);
int CPU_ADD_A_hash (CPU cpu);

int CPU_ADC_A_A (CPU cpu);
int CPU_ADC_A_B (CPU cpu);
int CPU_ADC_A_C (CPU cpu);
int CPU_ADC_A_D (CPU cpu);
int CPU_ADC_A_E (CPU cpu);
int CPU_ADC_A_H (CPU cpu);
int CPU_ADC_A_L (CPU cpu);
int CPU_ADC_A_aHL (CPU cpu);
int CPU_ADC_A_hash (CPU cpu);

int CPU_SUB_A_A (CPU cpu);
int CPU_SUB_A_B (CPU cpu);
int CPU_SUB_A_C (CPU cpu);
int CPU_SUB_A_D (CPU cpu);
int CPU_SUB_A_E (CPU cpu);
int CPU_SUB_A_H (CPU cpu);
int CPU_SUB_A_L (CPU cpu);
int CPU_SUB_A_aHL (CPU cpu);
int CPU_SUB_A_hash (CPU cpu);

int CPU_SBC_A_A (CPU cpu);
int CPU_SBC_A_B (CPU cpu);
int CPU_SBC_A_C (CPU cpu);
int CPU_SBC_A_D (CPU cpu);
int CPU_SBC_A_E (CPU cpu);
int CPU_SBC_A_H (CPU cpu);
int CPU_SBC_A_L (CPU cpu);
int CPU_SBC_A_aHL (CPU cpu);

int CPU_AND_A_A (CPU cpu);
int CPU_AND_A_B (CPU cpu);
int CPU_AND_A_C (CPU cpu);
int CPU_AND_A_D (CPU cpu);
int CPU_AND_A_E (CPU cpu);
int CPU_AND_A_H (CPU cpu);
int CPU_AND_A_L (CPU cpu);
int CPU_AND_A_aHL (CPU cpu);
int CPU_AND_A_hash (CPU cpu);

int CPU_OR_A_A (CPU cpu);
int CPU_OR_A_B (CPU cpu);
int CPU_OR_A_C (CPU cpu);
int CPU_OR_A_D (CPU cpu);
int CPU_OR_A_E (CPU cpu);
int CPU_OR_A_H (CPU cpu);
int CPU_OR_A_L (CPU cpu);
int CPU_OR_A_aHL (CPU cpu);
int CPU_OR_A_hash (CPU cpu);

int CPU_XOR_A_A (CPU cpu);
int CPU_XOR_A_B (CPU cpu);
int CPU_XOR_A_C (CPU cpu);
int CPU_XOR_A_D (CPU cpu);
int CPU_XOR_A_E (CPU cpu);
int CPU_XOR_A_H (CPU cpu);
int CPU_XOR_A_L (CPU cpu);
int CPU_XOR_A_aHL (CPU cpu);
int CPU_XOR_A_hash (CPU cpu);

int CPU_CP_A_A (CPU cpu);
int CPU_CP_A_B (CPU cpu);
int CPU_CP_A_C (CPU cpu);
int CPU_CP_A_D (CPU cpu);
int CPU_CP_A_E (CPU cpu);
int CPU_CP_A_H (CPU cpu);
int CPU_CP_A_L (CPU cpu);
int CPU_CP_A_aHL (CPU cpu);
int CPU_CP_A_hash (CPU cpu);

int CPU_INC_A (CPU cpu);
int CPU_INC_B (CPU cpu);
int CPU_INC_C (CPU cpu);
int CPU_INC_D (CPU cpu);
int CPU_INC_E (CPU cpu);
int CPU_INC_H (CPU cpu);
int CPU_INC_L (CPU cpu);
int CPU_INC_aHL (CPU cpu);

int CPU_DEC_A (CPU cpu);
int CPU_DEC_B (CPU cpu);
int CPU_DEC_C (CPU cpu);
int CPU_DEC_D (CPU cpu);
int CPU_DEC_E (CPU cpu);
int CPU_DEC_H (CPU cpu);
int CPU_DEC_L (CPU cpu);
int CPU_DEC_aHL (CPU cpu);

int CPU_ADD_HL_BC (CPU cpu);
int CPU_ADD_HL_DE (CPU cpu);
int CPU_ADD_HL_HL (CPU cpu);
int CPU_ADD_HL_SP (CPU cpu);
int CPU_ADD_SP_n (CPU cpu);

int CPU_INC_BC (CPU cpu);
int CPU_INC_DE (CPU cpu);
int CPU_INC_HL (CPU cpu);
int CPU_INC_SP (CPU cpu);

int CPU_DEC_BC (CPU cpu);
int CPU_DEC_DE (CPU cpu);
int CPU_DEC_HL (CPU cpu);
int CPU_DEC_SP (CPU cpu);

int CPU_DAA (CPU cpu);
int CPU_CPL (CPU cpu);
int CPU_CCF (CPU cpu);
int CPU_SCF (CPU cpu);
int CPU_HALT (CPU cpu);
int CPU_STOP (CPU cpu);

int CPU_DI (CPU cpu);
int CPU_EI (CPU cpu);

int CPU_SWAP_A (CPU cpu);
int CPU_SWAP_B (CPU cpu);
int CPU_SWAP_C (CPU cpu);
int CPU_SWAP_D (CPU cpu);
int CPU_SWAP_E (CPU cpu);
int CPU_SWAP_H (CPU cpu);
int CPU_SWAP_L (CPU cpu);
int CPU_SWAP_aHL (CPU cpu);

#endif
