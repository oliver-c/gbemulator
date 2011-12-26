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
int CPU_LD_A_n (CPU cpu);

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

int CPU_LD_aHL_A (CPU cpu);
int CPU_LD_aHL_B (CPU cpu);
int CPU_LD_aHL_C (CPU cpu);
int CPU_LD_aHL_D (CPU cpu);
int CPU_LD_aHL_E (CPU cpu);
int CPU_LD_aHL_H (CPU cpu);
int CPU_LD_aHL_L (CPU cpu);
int CPU_LD_aHL_n (CPU cpu);

int CPU_LD_aBC_A (CPU cpu);
int CPU_LD_aDE_A (CPU cpu);
int CPU_LD_ann_A (CPU cpu);

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
int CPU_ADD_A_n (CPU cpu);

int CPU_ADC_A_A (CPU cpu);
int CPU_ADC_A_B (CPU cpu);
int CPU_ADC_A_C (CPU cpu);
int CPU_ADC_A_D (CPU cpu);
int CPU_ADC_A_E (CPU cpu);
int CPU_ADC_A_H (CPU cpu);
int CPU_ADC_A_L (CPU cpu);
int CPU_ADC_A_aHL (CPU cpu);
int CPU_ADC_A_n (CPU cpu);

int CPU_SUB_A_A (CPU cpu);
int CPU_SUB_A_B (CPU cpu);
int CPU_SUB_A_C (CPU cpu);
int CPU_SUB_A_D (CPU cpu);
int CPU_SUB_A_E (CPU cpu);
int CPU_SUB_A_H (CPU cpu);
int CPU_SUB_A_L (CPU cpu);
int CPU_SUB_A_aHL (CPU cpu);
int CPU_SUB_A_n (CPU cpu);

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
int CPU_AND_A_n (CPU cpu);

int CPU_OR_A_A (CPU cpu);
int CPU_OR_A_B (CPU cpu);
int CPU_OR_A_C (CPU cpu);
int CPU_OR_A_D (CPU cpu);
int CPU_OR_A_E (CPU cpu);
int CPU_OR_A_H (CPU cpu);
int CPU_OR_A_L (CPU cpu);
int CPU_OR_A_aHL (CPU cpu);
int CPU_OR_A_n (CPU cpu);

int CPU_XOR_A_A (CPU cpu);
int CPU_XOR_A_B (CPU cpu);
int CPU_XOR_A_C (CPU cpu);
int CPU_XOR_A_D (CPU cpu);
int CPU_XOR_A_E (CPU cpu);
int CPU_XOR_A_H (CPU cpu);
int CPU_XOR_A_L (CPU cpu);
int CPU_XOR_A_aHL (CPU cpu);
int CPU_XOR_A_n (CPU cpu);

int CPU_CP_A_A (CPU cpu);
int CPU_CP_A_B (CPU cpu);
int CPU_CP_A_C (CPU cpu);
int CPU_CP_A_D (CPU cpu);
int CPU_CP_A_E (CPU cpu);
int CPU_CP_A_H (CPU cpu);
int CPU_CP_A_L (CPU cpu);
int CPU_CP_A_aHL (CPU cpu);
int CPU_CP_A_n (CPU cpu);

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

int CPU_RLCA (CPU cpu);
int CPU_RLA (CPU cpu);
int CPU_RRCA (CPU cpu);
int CPU_RRA (CPU cpu);

int CPU_JP_nn (CPU cpu);
int CPU_JPNZ_nn (CPU cpu);
int CPU_JPZ_nn (CPU cpu);
int CPU_JPNC_nn (CPU cpu);
int CPU_JPC_nn (CPU cpu);

int CPU_JP_HL (CPU cpu);
int CPU_JR_n (CPU cpu);
int CPU_JRNZ_n (CPU cpu);
int CPU_JRZ_n (CPU cpu);
int CPU_JRNC_n (CPU cpu);
int CPU_JRC_n (CPU cpu);

int CPU_CALL_nn (CPU cpu);
int CPU_CALLNZ_nn (CPU cpu); 
int CPU_CALLZ_nn (CPU cpu); 
int CPU_CALLNC_nn (CPU cpu); 
int CPU_CALLC_nn (CPU cpu); 

int CPU_RST_00H (CPU cpu);
int CPU_RST_08H (CPU cpu);
int CPU_RST_10H (CPU cpu);
int CPU_RST_18H (CPU cpu);
int CPU_RST_20H (CPU cpu);
int CPU_RST_28H (CPU cpu);
int CPU_RST_30H (CPU cpu);
int CPU_RST_38H (CPU cpu);

int CPU_RET (CPU cpu);
int CPU_RETNZ (CPU cpu);
int CPU_RETZ (CPU cpu);
int CPU_RETNC (CPU cpu);
int CPU_RETC (CPU cpu);
int CPU_RETI (CPU cpu);

int CPU_SWAP_A (CPU cpu);
int CPU_SWAP_B (CPU cpu);
int CPU_SWAP_C (CPU cpu);
int CPU_SWAP_D (CPU cpu);
int CPU_SWAP_E (CPU cpu);
int CPU_SWAP_H (CPU cpu);
int CPU_SWAP_L (CPU cpu);
int CPU_SWAP_aHL (CPU cpu);

int CPU_RLC_A (CPU cpu);
int CPU_RLC_B (CPU cpu);
int CPU_RLC_C (CPU cpu);
int CPU_RLC_D (CPU cpu);
int CPU_RLC_E (CPU cpu);
int CPU_RLC_H (CPU cpu);
int CPU_RLC_L (CPU cpu);
int CPU_RLC_aHL (CPU cpu);

int CPU_RL_A (CPU cpu);
int CPU_RL_B (CPU cpu);
int CPU_RL_C (CPU cpu);
int CPU_RL_D (CPU cpu);
int CPU_RL_E (CPU cpu);
int CPU_RL_H (CPU cpu);
int CPU_RL_L (CPU cpu);
int CPU_RL_aHL (CPU cpu);

int CPU_RRC_A (CPU cpu);
int CPU_RRC_B (CPU cpu);
int CPU_RRC_C (CPU cpu);
int CPU_RRC_D (CPU cpu);
int CPU_RRC_E (CPU cpu);
int CPU_RRC_H (CPU cpu);
int CPU_RRC_L (CPU cpu);
int CPU_RRC_aHL (CPU cpu);

int CPU_RR_A (CPU cpu);
int CPU_RR_B (CPU cpu);
int CPU_RR_C (CPU cpu);
int CPU_RR_D (CPU cpu);
int CPU_RR_E (CPU cpu);
int CPU_RR_H (CPU cpu);
int CPU_RR_L (CPU cpu);
int CPU_RR_aHL (CPU cpu);

int CPU_SLA_A (CPU cpu);
int CPU_SLA_B (CPU cpu);
int CPU_SLA_C (CPU cpu);
int CPU_SLA_D (CPU cpu);
int CPU_SLA_E (CPU cpu);
int CPU_SLA_H (CPU cpu);
int CPU_SLA_L (CPU cpu);
int CPU_SLA_aHL (CPU cpu);

int CPU_SRA_A (CPU cpu);
int CPU_SRA_B (CPU cpu);
int CPU_SRA_C (CPU cpu);
int CPU_SRA_D (CPU cpu);
int CPU_SRA_E (CPU cpu);
int CPU_SRA_H (CPU cpu);
int CPU_SRA_L (CPU cpu);
int CPU_SRA_aHL (CPU cpu);

int CPU_SRL_A (CPU cpu);
int CPU_SRL_B (CPU cpu);
int CPU_SRL_C (CPU cpu);
int CPU_SRL_D (CPU cpu);
int CPU_SRL_E (CPU cpu);
int CPU_SRL_H (CPU cpu);
int CPU_SRL_L (CPU cpu);
int CPU_SRL_aHL (CPU cpu);

int CPU_BIT_0_A (CPU cpu);
int CPU_BIT_0_B (CPU cpu);
int CPU_BIT_0_C (CPU cpu);
int CPU_BIT_0_D (CPU cpu);
int CPU_BIT_0_E (CPU cpu);
int CPU_BIT_0_H (CPU cpu);
int CPU_BIT_0_L (CPU cpu);
int CPU_BIT_0_aHL (CPU cpu);
int CPU_BIT_1_A (CPU cpu);
int CPU_BIT_1_B (CPU cpu);
int CPU_BIT_1_C (CPU cpu);
int CPU_BIT_1_D (CPU cpu);
int CPU_BIT_1_E (CPU cpu);
int CPU_BIT_1_H (CPU cpu);
int CPU_BIT_1_L (CPU cpu);
int CPU_BIT_1_aHL (CPU cpu);
int CPU_BIT_2_A (CPU cpu);
int CPU_BIT_2_B (CPU cpu);
int CPU_BIT_2_C (CPU cpu);
int CPU_BIT_2_D (CPU cpu);
int CPU_BIT_2_E (CPU cpu);
int CPU_BIT_2_H (CPU cpu);
int CPU_BIT_2_L (CPU cpu);
int CPU_BIT_2_aHL (CPU cpu);
int CPU_BIT_3_A (CPU cpu);
int CPU_BIT_3_B (CPU cpu);
int CPU_BIT_3_C (CPU cpu);
int CPU_BIT_3_D (CPU cpu);
int CPU_BIT_3_E (CPU cpu);
int CPU_BIT_3_H (CPU cpu);
int CPU_BIT_3_L (CPU cpu);
int CPU_BIT_3_aHL (CPU cpu);
int CPU_BIT_4_A (CPU cpu);
int CPU_BIT_4_B (CPU cpu);
int CPU_BIT_4_C (CPU cpu);
int CPU_BIT_4_D (CPU cpu);
int CPU_BIT_4_E (CPU cpu);
int CPU_BIT_4_H (CPU cpu);
int CPU_BIT_4_L (CPU cpu);
int CPU_BIT_4_aHL (CPU cpu);
int CPU_BIT_5_A (CPU cpu);
int CPU_BIT_5_B (CPU cpu);
int CPU_BIT_5_C (CPU cpu);
int CPU_BIT_5_D (CPU cpu);
int CPU_BIT_5_E (CPU cpu);
int CPU_BIT_5_H (CPU cpu);
int CPU_BIT_5_L (CPU cpu);
int CPU_BIT_5_aHL (CPU cpu);
int CPU_BIT_6_A (CPU cpu);
int CPU_BIT_6_B (CPU cpu);
int CPU_BIT_6_C (CPU cpu);
int CPU_BIT_6_D (CPU cpu);
int CPU_BIT_6_E (CPU cpu);
int CPU_BIT_6_H (CPU cpu);
int CPU_BIT_6_L (CPU cpu);
int CPU_BIT_6_aHL (CPU cpu);
int CPU_BIT_7_A (CPU cpu);
int CPU_BIT_7_B (CPU cpu);
int CPU_BIT_7_C (CPU cpu);
int CPU_BIT_7_D (CPU cpu);
int CPU_BIT_7_E (CPU cpu);
int CPU_BIT_7_H (CPU cpu);
int CPU_BIT_7_L (CPU cpu);
int CPU_BIT_7_aHL (CPU cpu);

int CPU_RES_0_A (CPU cpu);
int CPU_RES_0_B (CPU cpu);
int CPU_RES_0_C (CPU cpu);
int CPU_RES_0_D (CPU cpu);
int CPU_RES_0_E (CPU cpu);
int CPU_RES_0_H (CPU cpu);
int CPU_RES_0_L (CPU cpu);
int CPU_RES_0_aHL (CPU cpu);
int CPU_RES_1_A (CPU cpu);
int CPU_RES_1_B (CPU cpu);
int CPU_RES_1_C (CPU cpu);
int CPU_RES_1_D (CPU cpu);
int CPU_RES_1_E (CPU cpu);
int CPU_RES_1_H (CPU cpu);
int CPU_RES_1_L (CPU cpu);
int CPU_RES_1_aHL (CPU cpu);
int CPU_RES_2_A (CPU cpu);
int CPU_RES_2_B (CPU cpu);
int CPU_RES_2_C (CPU cpu);
int CPU_RES_2_D (CPU cpu);
int CPU_RES_2_E (CPU cpu);
int CPU_RES_2_H (CPU cpu);
int CPU_RES_2_L (CPU cpu);
int CPU_RES_2_aHL (CPU cpu);
int CPU_RES_3_A (CPU cpu);
int CPU_RES_3_B (CPU cpu);
int CPU_RES_3_C (CPU cpu);
int CPU_RES_3_D (CPU cpu);
int CPU_RES_3_E (CPU cpu);
int CPU_RES_3_H (CPU cpu);
int CPU_RES_3_L (CPU cpu);
int CPU_RES_3_aHL (CPU cpu);
int CPU_RES_4_A (CPU cpu);
int CPU_RES_4_B (CPU cpu);
int CPU_RES_4_C (CPU cpu);
int CPU_RES_4_D (CPU cpu);
int CPU_RES_4_E (CPU cpu);
int CPU_RES_4_H (CPU cpu);
int CPU_RES_4_L (CPU cpu);
int CPU_RES_4_aHL (CPU cpu);
int CPU_RES_5_A (CPU cpu);
int CPU_RES_5_B (CPU cpu);
int CPU_RES_5_C (CPU cpu);
int CPU_RES_5_D (CPU cpu);
int CPU_RES_5_E (CPU cpu);
int CPU_RES_5_H (CPU cpu);
int CPU_RES_5_L (CPU cpu);
int CPU_RES_5_aHL (CPU cpu);
int CPU_RES_6_A (CPU cpu);
int CPU_RES_6_B (CPU cpu);
int CPU_RES_6_C (CPU cpu);
int CPU_RES_6_D (CPU cpu);
int CPU_RES_6_E (CPU cpu);
int CPU_RES_6_H (CPU cpu);
int CPU_RES_6_L (CPU cpu);
int CPU_RES_6_aHL (CPU cpu);
int CPU_RES_7_A (CPU cpu);
int CPU_RES_7_B (CPU cpu);
int CPU_RES_7_C (CPU cpu);
int CPU_RES_7_D (CPU cpu);
int CPU_RES_7_E (CPU cpu);
int CPU_RES_7_H (CPU cpu);
int CPU_RES_7_L (CPU cpu);
int CPU_RES_7_aHL (CPU cpu);

int CPU_SET_0_A (CPU cpu);
int CPU_SET_0_B (CPU cpu);
int CPU_SET_0_C (CPU cpu);
int CPU_SET_0_D (CPU cpu);
int CPU_SET_0_E (CPU cpu);
int CPU_SET_0_H (CPU cpu);
int CPU_SET_0_L (CPU cpu);
int CPU_SET_0_aHL (CPU cpu);
int CPU_SET_1_A (CPU cpu);
int CPU_SET_1_B (CPU cpu);
int CPU_SET_1_C (CPU cpu);
int CPU_SET_1_D (CPU cpu);
int CPU_SET_1_E (CPU cpu);
int CPU_SET_1_H (CPU cpu);
int CPU_SET_1_L (CPU cpu);
int CPU_SET_1_aHL (CPU cpu);
int CPU_SET_2_A (CPU cpu);
int CPU_SET_2_B (CPU cpu);
int CPU_SET_2_C (CPU cpu);
int CPU_SET_2_D (CPU cpu);
int CPU_SET_2_E (CPU cpu);
int CPU_SET_2_H (CPU cpu);
int CPU_SET_2_L (CPU cpu);
int CPU_SET_2_aHL (CPU cpu);
int CPU_SET_3_A (CPU cpu);
int CPU_SET_3_B (CPU cpu);
int CPU_SET_3_C (CPU cpu);
int CPU_SET_3_D (CPU cpu);
int CPU_SET_3_E (CPU cpu);
int CPU_SET_3_H (CPU cpu);
int CPU_SET_3_L (CPU cpu);
int CPU_SET_3_aHL (CPU cpu);
int CPU_SET_4_A (CPU cpu);
int CPU_SET_4_B (CPU cpu);
int CPU_SET_4_C (CPU cpu);
int CPU_SET_4_D (CPU cpu);
int CPU_SET_4_E (CPU cpu);
int CPU_SET_4_H (CPU cpu);
int CPU_SET_4_L (CPU cpu);
int CPU_SET_4_aHL (CPU cpu);
int CPU_SET_5_A (CPU cpu);
int CPU_SET_5_B (CPU cpu);
int CPU_SET_5_C (CPU cpu);
int CPU_SET_5_D (CPU cpu);
int CPU_SET_5_E (CPU cpu);
int CPU_SET_5_H (CPU cpu);
int CPU_SET_5_L (CPU cpu);
int CPU_SET_5_aHL (CPU cpu);
int CPU_SET_6_A (CPU cpu);
int CPU_SET_6_B (CPU cpu);
int CPU_SET_6_C (CPU cpu);
int CPU_SET_6_D (CPU cpu);
int CPU_SET_6_E (CPU cpu);
int CPU_SET_6_H (CPU cpu);
int CPU_SET_6_L (CPU cpu);
int CPU_SET_6_aHL (CPU cpu);
int CPU_SET_7_A (CPU cpu);
int CPU_SET_7_B (CPU cpu);
int CPU_SET_7_C (CPU cpu);
int CPU_SET_7_D (CPU cpu);
int CPU_SET_7_E (CPU cpu);
int CPU_SET_7_H (CPU cpu);
int CPU_SET_7_L (CPU cpu);
int CPU_SET_7_aHL (CPU cpu);
#endif
