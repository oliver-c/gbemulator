#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "GB.h"
#include "CPU.h"
#include "CPU_instructions.h"
#include "MMU.h"

struct CPU {
   GB gb;

   bool IME;
   reg registers[NUM_REGISTERS];
};

/* Arrays of pointers to the functions that will execute the CPU instructions */
int (*instructionMap[0x100])(CPU) = {NULL};
int (*instructionMapCB[0x100])(CPU) = {NULL};

void CPU_initInstructionMap (void);

CPU CPU_init (GB gb) {
   CPU newCPU = (CPU)malloc(sizeof(struct CPU));
   assert (newCPU != NULL);

   newCPU->gb = gb;

   /* Not sure about this one */
   newCPU->IME = FALSE;

   /* Set up an array of function pointers with opcodes as indices */
   CPU_initInstructionMap ();

   return newCPU;
}

void CPU_free (CPU cpu) {
   free (cpu);
}

word CPU_get16bitRegisterValue (CPU cpu, register16 r) {
   assert (0 <= r && r < NUM_REGISTERS);
   return cpu->registers[r].value;
}

void CPU_set16bitRegisterValue (CPU cpu, register16 r, word value) {
   assert (0 <= r && r < NUM_REGISTERS);
   cpu->registers[r].value = value;
}

byte CPU_get8bitRegisterValue (CPU cpu, register8 r) {
   byte value = 0;

   switch (r) {
      case A:
         value = cpu->registers[AF].bytes.high;
         break;
      case F:
         value = cpu->registers[AF].bytes.low;
         break;
      case B:
         value = cpu->registers[BC].bytes.high;
         break;
      case C:
         value = cpu->registers[BC].bytes.low;
         break;
      case D:
         value = cpu->registers[DE].bytes.high;
         break;
      case E:
         value = cpu->registers[DE].bytes.low;
         break;
      case H:
         value = cpu->registers[HL].bytes.high;
         break;
      case L:
         value = cpu->registers[HL].bytes.low;
         break;
      default:
         break;
   }

   return value;
}

void CPU_set8bitRegisterValue (CPU cpu, register8 r, byte value) {
   switch (r) {
      case A:
         cpu->registers[AF].bytes.high = value;
         break;
      case F:
         cpu->registers[AF].bytes.low = value;
         break;
      case B:
         cpu->registers[BC].bytes.high = value;
         break;
      case C:
         cpu->registers[BC].bytes.low = value;
         break;
      case D:
         cpu->registers[DE].bytes.high = value;
         break;
      case E:
         cpu->registers[DE].bytes.low = value;
         break;
      case H:
         cpu->registers[HL].bytes.high = value;
         break;
      case L:
         cpu->registers[HL].bytes.low = value;
         break;
      default:
         break;
   }
}

int CPU_step (CPU cpu) {
   int numCycles = 0;
   byte opcode;
   MMU mmu;

   mmu = GB_getMMU (cpu->gb);
   
   /* Fetch the opcode for the next instruction to execute */
   opcode = MMU_readByte (mmu, cpu->registers[PC].value);

   /* Execute the instruction */
   if (opcode == 0xCB) {
      /* 0xCB prefixed instruction */
      opcode = MMU_readByte (mmu, cpu->registers[PC].value + 1);
      if (instructionMap[opcode] != NULL) 
         numCycles = instructionMapCB[opcode] (cpu);
   } else if (instructionMap[opcode] != NULL) {
      numCycles = instructionMap[opcode] (cpu);
   } else {
      fprintf (stderr, "ERROR: Opcode %x is not implemented\n", opcode);
   }

   return numCycles;
}

void CPU_setIME (CPU cpu, bool enabled) {
   cpu->IME = enabled;
}

bool CPU_getIME (CPU cpu) {
   return (cpu->IME);
}

int CPU_executeInterrupt (CPU cpu, interrupt type) {
   MMU mmu;
   int cycles = 0;

   mmu = GB_getMMU (cpu->gb);

   /* Reset the IME flag */
   cpu->IME = FALSE;

   /* TODO: Replace below code with the instruction functions 
            (e.g. CPU_jmp) once they're implemented */

   /* Push the current program counter onto the stack */
   cpu->registers[SP].value -= 2;   
   MMU_writeWord (mmu, cpu->registers[SP].value, cpu->registers[PC].value);

   /* Jump to starting address of interrupt */
   switch (type) {
      case INT_VBLANK:
         cpu->registers[PC].value = 0x40;
         break;
      case INT_LCDSTAT:
         cpu->registers[PC].value = 0x48;
         break;
      case INT_TIMER:
         cpu->registers[PC].value = 0x50;
         break;
      case INT_SERIAL:
         cpu->registers[PC].value = 0x58;
         break;
      case INT_JOYPAD:
         cpu->registers[PC].value = 0x60;
         break;
   }

   /* Cycles used by the push and the jump */
   cycles = 16+12;

   return cycles;
}

void CPU_setCarry (CPU cpu) {
   (cpu->registers[AF].bytes.low) |= (1 << FLAG_CARRY_BIT);
}

void CPU_setZero (CPU cpu) {
   (cpu->registers[AF].bytes.low) |= (1 << FLAG_ZERO_BIT);
}

void CPU_setHalfCarry (CPU cpu) {
   (cpu->registers[AF].bytes.low) |= (1 << FLAG_HALFCARRY_BIT);
}

void CPU_setSub (CPU cpu) {
   (cpu->registers[AF].bytes.low) |= (1 << FLAG_SUB_BIT);
}

void CPU_clearCarry (CPU cpu) {
   if (((cpu->registers[AF].bytes.low) & (1<<FLAG_CARRY_BIT))) {
      (cpu->registers[AF].bytes.low) ^= (1<<FLAG_CARRY_BIT);
   }
}

void CPU_clearZero (CPU cpu) {
   if (((cpu->registers[AF].bytes.low) & (1<<FLAG_ZERO_BIT))) {
      (cpu->registers[AF].bytes.low) ^= (1<<FLAG_ZERO_BIT);
   }
}

void CPU_clearHalfCarry (CPU cpu) {
   if (((cpu->registers[AF].bytes.low) & (1<<FLAG_HALFCARRY_BIT))) {
      (cpu->registers[AF].bytes.low) ^= (1<<FLAG_HALFCARRY_BIT);
   }
}

void CPU_clearSub (CPU cpu) {
   if (((cpu->registers[AF].bytes.low) & (1<<FLAG_SUB_BIT))) {
      (cpu->registers[AF].bytes.low) ^= (1<<FLAG_SUB_BIT);
   }
}

void CPU_clearFlags (CPU cpu) {
   cpu->registers[AF].bytes.low = 0;
}

bool CPU_isCarrySet (CPU cpu) {
   return (cpu->registers[AF].bytes.low & (1<<FLAG_CARRY_BIT));
}

bool CPU_isZeroSet (CPU cpu) {
   return (cpu->registers[AF].bytes.low & (1<<FLAG_ZERO_BIT));
}

bool CPU_isHalfCarrySet (CPU cpu) {
   return (cpu->registers[AF].bytes.low & (1<<FLAG_HALFCARRY_BIT));
}

bool  CPU_isSubSet (CPU cpu) {
   return (cpu->registers[AF].bytes.low & (1<<FLAG_SUB_BIT));
}

void CPU_initInstructionMap () {
   instructionMap[0x00] = &CPU_NOP;

   instructionMap[0x06] = &CPU_LD_B_n;
   instructionMap[0x0E] = &CPU_LD_C_n;
   instructionMap[0x16] = &CPU_LD_D_n;
   instructionMap[0x1E] = &CPU_LD_E_n;
   instructionMap[0x26] = &CPU_LD_H_n;
   instructionMap[0x2E] = &CPU_LD_L_n;

   instructionMap[0x7F] = &CPU_LD_A_A;
   instructionMap[0x78] = &CPU_LD_A_B;
   instructionMap[0x79] = &CPU_LD_A_C;
   instructionMap[0x7A] = &CPU_LD_A_D;
   instructionMap[0x7B] = &CPU_LD_A_E;
   instructionMap[0x7C] = &CPU_LD_A_H;
   instructionMap[0x7D] = &CPU_LD_A_L;
   instructionMap[0x7E] = &CPU_LD_A_aHL;
   instructionMap[0x0A] = &CPU_LD_A_aBC;
   instructionMap[0x1A] = &CPU_LD_A_aDE;
   instructionMap[0xFA] = &CPU_LD_A_ann;
   instructionMap[0x3E] = &CPU_LD_A_hash; /* wtf is this */

   instructionMap[0x47] = &CPU_LD_B_A;
   instructionMap[0x40] = &CPU_LD_B_B;
   instructionMap[0x41] = &CPU_LD_B_C;
   instructionMap[0x42] = &CPU_LD_B_D;
   instructionMap[0x43] = &CPU_LD_B_E;
   instructionMap[0x44] = &CPU_LD_B_H;
   instructionMap[0x45] = &CPU_LD_B_L;
   instructionMap[0x46] = &CPU_LD_B_aHL;

   instructionMap[0x4F] = &CPU_LD_C_A;
   instructionMap[0x48] = &CPU_LD_C_B;
   instructionMap[0x49] = &CPU_LD_C_C;
   instructionMap[0x4A] = &CPU_LD_C_D;
   instructionMap[0x4B] = &CPU_LD_C_E;
   instructionMap[0x4C] = &CPU_LD_C_H;
   instructionMap[0x4D] = &CPU_LD_C_L;
   instructionMap[0x4E] = &CPU_LD_C_aHL;

   instructionMap[0x57] = &CPU_LD_D_A;
   instructionMap[0x50] = &CPU_LD_D_B;
   instructionMap[0x51] = &CPU_LD_D_C;
   instructionMap[0x52] = &CPU_LD_D_D;
   instructionMap[0x53] = &CPU_LD_D_E;
   instructionMap[0x54] = &CPU_LD_D_H;
   instructionMap[0x55] = &CPU_LD_D_L;
   instructionMap[0x56] = &CPU_LD_D_aHL;

   instructionMap[0x5F] = &CPU_LD_E_A;
   instructionMap[0x58] = &CPU_LD_E_B;
   instructionMap[0x59] = &CPU_LD_E_C;
   instructionMap[0x5A] = &CPU_LD_E_D;
   instructionMap[0x5B] = &CPU_LD_E_E;
   instructionMap[0x5C] = &CPU_LD_E_H;
   instructionMap[0x5D] = &CPU_LD_E_L;
   instructionMap[0x5E] = &CPU_LD_E_aHL;

   instructionMap[0x67] = &CPU_LD_H_A;
   instructionMap[0x60] = &CPU_LD_H_B;
   instructionMap[0x61] = &CPU_LD_H_C;
   instructionMap[0x62] = &CPU_LD_H_D;
   instructionMap[0x63] = &CPU_LD_H_E;
   instructionMap[0x64] = &CPU_LD_H_H;
   instructionMap[0x65] = &CPU_LD_H_L;
   instructionMap[0x66] = &CPU_LD_H_aHL;

   instructionMap[0x6F] = &CPU_LD_L_A;
   instructionMap[0x68] = &CPU_LD_L_B;
   instructionMap[0x69] = &CPU_LD_L_C;
   instructionMap[0x6A] = &CPU_LD_L_D;
   instructionMap[0x6B] = &CPU_LD_L_E;
   instructionMap[0x6C] = &CPU_LD_L_H;
   instructionMap[0x6D] = &CPU_LD_L_L;
   instructionMap[0x6E] = &CPU_LD_L_aHL;

   instructionMap[0x77] = &CPU_aHL_A;
   instructionMap[0x70] = &CPU_aHL_B;
   instructionMap[0x71] = &CPU_aHL_C;
   instructionMap[0x72] = &CPU_aHL_D;
   instructionMap[0x73] = &CPU_aHL_E;
   instructionMap[0x74] = &CPU_aHL_H;
   instructionMap[0x75] = &CPU_aHL_L;
   instructionMap[0x36] = &CPU_aHL_n;

   instructionMap[0x02] = &CPU_aBC_A;
   instructionMap[0x12] = &CPU_aDE_A;
   instructionMap[0xEA] = &CPU_ann_A;

   instructionMap[0xF2] = &CPU_LD_A_aC;
   instructionMap[0xE2] = &CPU_LD_aC_A;

   instructionMap[0x3A] = &CPU_LDD_A_aHL;
   instructionMap[0x32] = &CPU_LDD_aHL_A;
   instructionMap[0x2A] = &CPU_LDI_A_aHL;
   instructionMap[0x22] = &CPU_LDI_aHL_A;

   instructionMap[0xE0] = &CPU_LDH_an_A;
   instructionMap[0xF0] = &CPU_LDH_A_an;

   instructionMap[0x01] = &CPU_LD_BC_nn;
   instructionMap[0x11] = &CPU_LD_DE_nn;
   instructionMap[0x21] = &CPU_LD_HL_nn;
   instructionMap[0x31] = &CPU_LD_SP_nn;

   instructionMap[0xF9] = &CPU_LD_SP_HL;
   instructionMap[0xF8] = &CPU_LDHL_SP_n;
   instructionMap[0x08] = &CPU_LD_ann_SP;

   instructionMap[0xF5] = &CPU_PUSH_AF;
   instructionMap[0xC5] = &CPU_PUSH_BC;
   instructionMap[0xD5] = &CPU_PUSH_DE;
   instructionMap[0xE5] = &CPU_PUSH_HL;

   instructionMap[0xF1] = &CPU_POP_AF;
   instructionMap[0xC1] = &CPU_POP_BC;
   instructionMap[0xD1] = &CPU_POP_DE;
   instructionMap[0xE1] = &CPU_POP_HL;

   instructionMap[0x87] = &CPU_ADD_A_A;
   instructionMap[0x80] = &CPU_ADD_A_B;
   instructionMap[0x81] = &CPU_ADD_A_C;
   instructionMap[0x82] = &CPU_ADD_A_D;
   instructionMap[0x83] = &CPU_ADD_A_E;
   instructionMap[0x84] = &CPU_ADD_A_H;
   instructionMap[0x85] = &CPU_ADD_A_L;
   instructionMap[0x86] = &CPU_ADD_A_aHL;
   instructionMap[0xC6] = &CPU_ADD_A_hash;

   instructionMap[0x8F] = &CPU_ADC_A_A; 
   instructionMap[0x88] = &CPU_ADC_A_B; 
   instructionMap[0x89] = &CPU_ADC_A_C; 
   instructionMap[0x8A] = &CPU_ADC_A_D; 
   instructionMap[0x8B] = &CPU_ADC_A_E; 
   instructionMap[0x8C] = &CPU_ADC_A_H; 
   instructionMap[0x8D] = &CPU_ADC_A_L; 
   instructionMap[0x8E] = &CPU_ADC_A_aHL; 
   instructionMap[0xCE] = &CPU_ADC_A_hash; 

   instructionMap[0x97] = &CPU_SUB_A_A;
   instructionMap[0x90] = &CPU_SUB_A_B;
   instructionMap[0x91] = &CPU_SUB_A_C;
   instructionMap[0x92] = &CPU_SUB_A_D;
   instructionMap[0x93] = &CPU_SUB_A_E;
   instructionMap[0x94] = &CPU_SUB_A_H;
   instructionMap[0x95] = &CPU_SUB_A_L;
   instructionMap[0x96] = &CPU_SUB_A_aHL;
   instructionMap[0xD6] = &CPU_SUB_A_hash;

   instructionMap[0x9F] = &CPU_SBC_A_A;
   instructionMap[0x98] = &CPU_SBC_A_B;
   instructionMap[0x99] = &CPU_SBC_A_C;
   instructionMap[0x9A] = &CPU_SBC_A_D;
   instructionMap[0x9B] = &CPU_SBC_A_E;
   instructionMap[0x9C] = &CPU_SBC_A_H;
   instructionMap[0x9D] = &CPU_SBC_A_L;
   instructionMap[0x9E] = &CPU_SBC_A_aHL;

   instructionMap[0xA7] = &CPU_AND_A_A;
   instructionMap[0xA0] = &CPU_AND_A_B;
   instructionMap[0xA1] = &CPU_AND_A_C;
   instructionMap[0xA2] = &CPU_AND_A_D;
   instructionMap[0xA3] = &CPU_AND_A_E;
   instructionMap[0xA4] = &CPU_AND_A_H;
   instructionMap[0xA5] = &CPU_AND_A_L;
   instructionMap[0xA6] = &CPU_AND_A_aHL;
   instructionMap[0xE6] = &CPU_AND_A_hash;

   instructionMap[0xB7] = &CPU_OR_A_A;
   instructionMap[0xB0] = &CPU_OR_A_B;
   instructionMap[0xB1] = &CPU_OR_A_C;
   instructionMap[0xB2] = &CPU_OR_A_D;
   instructionMap[0xB3] = &CPU_OR_A_E;
   instructionMap[0xB4] = &CPU_OR_A_H;
   instructionMap[0xB5] = &CPU_OR_A_L;
   instructionMap[0xB6] = &CPU_OR_A_aHL;
   instructionMap[0xB6] = &CPU_OR_A_hash;

   instructionMap[0xAF] = &CPU_XOR_A_A;
   instructionMap[0xA8] = &CPU_XOR_A_B;
   instructionMap[0xA9] = &CPU_XOR_A_C;
   instructionMap[0xAA] = &CPU_XOR_A_D;
   instructionMap[0xAB] = &CPU_XOR_A_E;
   instructionMap[0xAC] = &CPU_XOR_A_H;
   instructionMap[0xAD] = &CPU_XOR_A_L;
   instructionMap[0xAE] = &CPU_XOR_A_aHL;
   instructionMap[0xEE] = &CPU_XOR_A_hash;

   instructionMap[0xBF] = &CPU_CP_A_A;
   instructionMap[0xB8] = &CPU_CP_A_B;
   instructionMap[0xB9] = &CPU_CP_A_C;
   instructionMap[0xBA] = &CPU_CP_A_D;
   instructionMap[0xBB] = &CPU_CP_A_E;
   instructionMap[0xBC] = &CPU_CP_A_H;
   instructionMap[0xBD] = &CPU_CP_A_L;
   instructionMap[0xBE] = &CPU_CP_A_aHL;
   instructionMap[0xFE] = &CPU_CP_A_hash;

   instructionMap[0x3C] = &CPU_INC_A;
   instructionMap[0x04] = &CPU_INC_B;
   instructionMap[0x0C] = &CPU_INC_C;
   instructionMap[0x14] = &CPU_INC_D;
   instructionMap[0x1C] = &CPU_INC_E;
   instructionMap[0x24] = &CPU_INC_H;
   instructionMap[0x2C] = &CPU_INC_L;
   instructionMap[0x34] = &CPU_INC_aHL;

   instructionMap[0x3D] = &CPU_DEC_A;
   instructionMap[0x05] = &CPU_DEC_B;
   instructionMap[0x0D] = &CPU_DEC_C;
   instructionMap[0x15] = &CPU_DEC_D;
   instructionMap[0x1D] = &CPU_DEC_E;
   instructionMap[0x25] = &CPU_DEC_H;
   instructionMap[0x2D] = &CPU_DEC_L;
   instructionMap[0x35] = &CPU_DEC_aHL;

   instructionMap[0x09] = &CPU_ADD_HL_BC;
   instructionMap[0x19] = &CPU_ADD_HL_DE;
   instructionMap[0x29] = &CPU_ADD_HL_HL;
   instructionMap[0x39] = &CPU_ADD_HL_SP;
   instructionMap[0xE8] = &CPU_ADD_SP_n;

   instructionMap[0x03] = &CPU_INC_BC;
   instructionMap[0x13] = &CPU_INC_DE;
   instructionMap[0x23] = &CPU_INC_HL;
   instructionMap[0x33] = &CPU_INC_SP;

   instructionMap[0x0B] = &CPU_DEC_BC;
   instructionMap[0x1B] = &CPU_DEC_DE;
   instructionMap[0x2B] = &CPU_DEC_HL;
   instructionMap[0x3B] = &CPU_DEC_SP;

   instructionMap[0x27] = &CPU_DAA;
   instructionMap[0x2F] = &CPU_CPL;
   instructionMap[0x3F] = &CPU_CCF;
   instructionMap[0x37] = &CPU_SCF;
   instructionMap[0x76] = &CPU_HALT;
   instructionMap[0x10] = &CPU_STOP;

   instructionMap[0xF3] = &CPU_DI;
   instructionMap[0xFB] = &CPU_EI;

   instructionMap[0x07] = &CPU_RLCA;
   instructionMap[0x17] = &CPU_RLA;
   instructionMap[0x0F] = &CPU_RRCA;
   instructionMap[0x1F] = &CPU_RRA;

   instructionMap[0xC3] = &CPU_JP_nn;
   instructionMap[0xC2] = &CPU_JPNZ_nn;
   instructionMap[0xCA] = &CPU_JPZ_nn;
   instructionMap[0xD2] = &CPU_JPNC_nn;
   instructionMap[0xDA] = &CPU_JPC_nn;
   instructionMap[0xE9] = &CPU_JP_HL;
   instructionMap[0x18] = &CPU_JR_n;
   instructionMap[0x20] = &CPU_JRNZ_n;
   instructionMap[0x28] = &CPU_JRZ_n;
   instructionMap[0x30] = &CPU_JRNC_n;
   instructionMap[0x38] = &CPU_JRC_n;

   instructionMap[0xCD] = &CPU_CALL_nn; 
   instructionMap[0xC4] = &CPU_CALLNZ_nn; 
   instructionMap[0xCC] = &CPU_CALLZ_nn; 
   instructionMap[0xD4] = &CPU_CALLNC_nn; 
   instructionMap[0xDC] = &CPU_CALLC_nn; 

   instructionMap[0xC7] = &CPU_RST_00H;
   instructionMap[0xCF] = &CPU_RST_08H;
   instructionMap[0xD7] = &CPU_RST_10H;
   instructionMap[0xDF] = &CPU_RST_18H;
   instructionMap[0xE7] = &CPU_RST_20H;
   instructionMap[0xEF] = &CPU_RST_28H;
   instructionMap[0xF7] = &CPU_RST_30H;
   instructionMap[0xFF] = &CPU_RST_38H;

   instructionMapCB[0x37] = &CPU_SWAP_A; 
   instructionMapCB[0x30] = &CPU_SWAP_B; 
   instructionMapCB[0x31] = &CPU_SWAP_C; 
   instructionMapCB[0x32] = &CPU_SWAP_D; 
   instructionMapCB[0x33] = &CPU_SWAP_E; 
   instructionMapCB[0x34] = &CPU_SWAP_H; 
   instructionMapCB[0x35] = &CPU_SWAP_L; 
   instructionMapCB[0x36] = &CPU_SWAP_aHL; 

   instructionMapCB[0x07] = &CPU_RLC_A;
   instructionMapCB[0x00] = &CPU_RLC_B;
   instructionMapCB[0x01] = &CPU_RLC_C;
   instructionMapCB[0x02] = &CPU_RLC_D;
   instructionMapCB[0x03] = &CPU_RLC_E;
   instructionMapCB[0x04] = &CPU_RLC_H;
   instructionMapCB[0x05] = &CPU_RLC_L;
   instructionMapCB[0x06] = &CPU_RLC_aHL;

   instructionMapCB[0x17] = &CPU_RL_A;
   instructionMapCB[0x10] = &CPU_RL_B;
   instructionMapCB[0x11] = &CPU_RL_C;
   instructionMapCB[0x12] = &CPU_RL_D;
   instructionMapCB[0x13] = &CPU_RL_E;
   instructionMapCB[0x14] = &CPU_RL_H;
   instructionMapCB[0x15] = &CPU_RL_L;
   instructionMapCB[0x16] = &CPU_RL_aHL;

   instructionMapCB[0x0F] = &CPU_RRC_A;
   instructionMapCB[0x08] = &CPU_RRC_B;
   instructionMapCB[0x09] = &CPU_RRC_C;
   instructionMapCB[0x0A] = &CPU_RRC_D;
   instructionMapCB[0x0B] = &CPU_RRC_E;
   instructionMapCB[0x0C] = &CPU_RRC_H;
   instructionMapCB[0x0D] = &CPU_RRC_L;
   instructionMapCB[0x0E] = &CPU_RRC_aHL;

   instructionMapCB[0x1F] = &CPU_RR_A;
   instructionMapCB[0x18] = &CPU_RR_B;
   instructionMapCB[0x19] = &CPU_RR_C;
   instructionMapCB[0x1A] = &CPU_RR_D;
   instructionMapCB[0x1B] = &CPU_RR_E;
   instructionMapCB[0x1C] = &CPU_RR_H;
   instructionMapCB[0x1D] = &CPU_RR_L;
   instructionMapCB[0x1E] = &CPU_RR_aHL;

   instructionMapCB[0x27] = &CPU_SLA_A;
   instructionMapCB[0x20] = &CPU_SLA_B;
   instructionMapCB[0x21] = &CPU_SLA_C;
   instructionMapCB[0x22] = &CPU_SLA_D;
   instructionMapCB[0x23] = &CPU_SLA_E;
   instructionMapCB[0x24] = &CPU_SLA_H;
   instructionMapCB[0x25] = &CPU_SLA_L;
   instructionMapCB[0x26] = &CPU_SLA_aHL;

   instructionMapCB[0x2F] = &CPU_SRA_A;
   instructionMapCB[0x28] = &CPU_SRA_B;
   instructionMapCB[0x29] = &CPU_SRA_C;
   instructionMapCB[0x2A] = &CPU_SRA_D;
   instructionMapCB[0x2B] = &CPU_SRA_E;
   instructionMapCB[0x2C] = &CPU_SRA_H;
   instructionMapCB[0x2D] = &CPU_SRA_L;
   instructionMapCB[0x2E] = &CPU_SRA_aHL;

   instructionMapCB[0x3F] = &CPU_SRL_A;
   instructionMapCB[0x38] = &CPU_SRL_B;
   instructionMapCB[0x39] = &CPU_SRL_C;
   instructionMapCB[0x3A] = &CPU_SRL_D;
   instructionMapCB[0x3B] = &CPU_SRL_E;
   instructionMapCB[0x3C] = &CPU_SRL_H;
   instructionMapCB[0x3D] = &CPU_SRL_L;
   instructionMapCB[0x3E] = &CPU_SRL_aHL;

   instructionMapCB[0x47] = &CPU_BIT_0_A;
   instructionMapCB[0x40] = &CPU_BIT_0_B;
   instructionMapCB[0x41] = &CPU_BIT_0_C;
   instructionMapCB[0x42] = &CPU_BIT_0_D;
   instructionMapCB[0x43] = &CPU_BIT_0_E;
   instructionMapCB[0x44] = &CPU_BIT_0_H;
   instructionMapCB[0x45] = &CPU_BIT_0_L;
   instructionMapCB[0x46] = &CPU_BIT_0_aHL;
   instructionMapCB[0x4F] = &CPU_BIT_1_A;
   instructionMapCB[0x48] = &CPU_BIT_1_B;
   instructionMapCB[0x49] = &CPU_BIT_1_C;
   instructionMapCB[0x4A] = &CPU_BIT_1_D;
   instructionMapCB[0x4B] = &CPU_BIT_1_E;
   instructionMapCB[0x4C] = &CPU_BIT_1_H;
   instructionMapCB[0x4D] = &CPU_BIT_1_L;
   instructionMapCB[0x4E] = &CPU_BIT_1_aHL;
   instructionMapCB[0x57] = &CPU_BIT_2_A;
   instructionMapCB[0x50] = &CPU_BIT_2_B;
   instructionMapCB[0x51] = &CPU_BIT_2_C;
   instructionMapCB[0x52] = &CPU_BIT_2_D;
   instructionMapCB[0x53] = &CPU_BIT_2_E;
   instructionMapCB[0x54] = &CPU_BIT_2_H;
   instructionMapCB[0x55] = &CPU_BIT_2_L;
   instructionMapCB[0x56] = &CPU_BIT_2_aHL;
   instructionMapCB[0x5F] = &CPU_BIT_3_A;
   instructionMapCB[0x58] = &CPU_BIT_3_B;
   instructionMapCB[0x59] = &CPU_BIT_3_C;
   instructionMapCB[0x5A] = &CPU_BIT_3_D;
   instructionMapCB[0x5B] = &CPU_BIT_3_E;
   instructionMapCB[0x5C] = &CPU_BIT_3_H;
   instructionMapCB[0x5D] = &CPU_BIT_3_L;
   instructionMapCB[0x5E] = &CPU_BIT_3_aHL;
   instructionMapCB[0x67] = &CPU_BIT_4_A;
   instructionMapCB[0x60] = &CPU_BIT_4_B;
   instructionMapCB[0x61] = &CPU_BIT_4_C;
   instructionMapCB[0x62] = &CPU_BIT_4_D;
   instructionMapCB[0x63] = &CPU_BIT_4_E;
   instructionMapCB[0x64] = &CPU_BIT_4_H;
   instructionMapCB[0x65] = &CPU_BIT_4_L;
   instructionMapCB[0x66] = &CPU_BIT_4_aHL;
   instructionMapCB[0x6F] = &CPU_BIT_5_A;
   instructionMapCB[0x68] = &CPU_BIT_5_B;
   instructionMapCB[0x69] = &CPU_BIT_5_C;
   instructionMapCB[0x6A] = &CPU_BIT_5_D;
   instructionMapCB[0x6B] = &CPU_BIT_5_E;
   instructionMapCB[0x6C] = &CPU_BIT_5_H;
   instructionMapCB[0x6D] = &CPU_BIT_5_L;
   instructionMapCB[0x6E] = &CPU_BIT_5_aHL;
   instructionMapCB[0x77] = &CPU_BIT_6_A;
   instructionMapCB[0x70] = &CPU_BIT_6_B;
   instructionMapCB[0x71] = &CPU_BIT_6_C;
   instructionMapCB[0x72] = &CPU_BIT_6_D;
   instructionMapCB[0x73] = &CPU_BIT_6_E;
   instructionMapCB[0x74] = &CPU_BIT_6_H;
   instructionMapCB[0x75] = &CPU_BIT_6_L;
   instructionMapCB[0x76] = &CPU_BIT_6_aHL;
   instructionMapCB[0x7F] = &CPU_BIT_7_A;
   instructionMapCB[0x78] = &CPU_BIT_7_B;
   instructionMapCB[0x79] = &CPU_BIT_7_C;
   instructionMapCB[0x7A] = &CPU_BIT_7_D;
   instructionMapCB[0x7B] = &CPU_BIT_7_E;
   instructionMapCB[0x7C] = &CPU_BIT_7_H;
   instructionMapCB[0x7D] = &CPU_BIT_7_L;
   instructionMapCB[0x7E] = &CPU_BIT_7_aHL;

   instructionMapCB[0x87] = &CPU_RES_0_A;
   instructionMapCB[0x80] = &CPU_RES_0_B;
   instructionMapCB[0x81] = &CPU_RES_0_C;
   instructionMapCB[0x82] = &CPU_RES_0_D;
   instructionMapCB[0x83] = &CPU_RES_0_E;
   instructionMapCB[0x84] = &CPU_RES_0_H;
   instructionMapCB[0x85] = &CPU_RES_0_L;
   instructionMapCB[0x86] = &CPU_RES_0_aHL;
   instructionMapCB[0x8F] = &CPU_RES_1_A;
   instructionMapCB[0x88] = &CPU_RES_1_B;
   instructionMapCB[0x89] = &CPU_RES_1_C;
   instructionMapCB[0x8A] = &CPU_RES_1_D;
   instructionMapCB[0x8B] = &CPU_RES_1_E;
   instructionMapCB[0x8C] = &CPU_RES_1_H;
   instructionMapCB[0x8D] = &CPU_RES_1_L;
   instructionMapCB[0x8E] = &CPU_RES_1_aHL;
   instructionMapCB[0x97] = &CPU_RES_2_A;
   instructionMapCB[0x90] = &CPU_RES_2_B;
   instructionMapCB[0x91] = &CPU_RES_2_C;
   instructionMapCB[0x92] = &CPU_RES_2_D;
   instructionMapCB[0x93] = &CPU_RES_2_E;
   instructionMapCB[0x94] = &CPU_RES_2_H;
   instructionMapCB[0x95] = &CPU_RES_2_L;
   instructionMapCB[0x96] = &CPU_RES_2_aHL;
   instructionMapCB[0x9F] = &CPU_RES_3_A;
   instructionMapCB[0x98] = &CPU_RES_3_B;
   instructionMapCB[0x99] = &CPU_RES_3_C;
   instructionMapCB[0x9A] = &CPU_RES_3_D;
   instructionMapCB[0x9B] = &CPU_RES_3_E;
   instructionMapCB[0x9C] = &CPU_RES_3_H;
   instructionMapCB[0x9D] = &CPU_RES_3_L;
   instructionMapCB[0x9E] = &CPU_RES_3_aHL;
   instructionMapCB[0xA7] = &CPU_RES_4_A;
   instructionMapCB[0xA0] = &CPU_RES_4_B;
   instructionMapCB[0xA1] = &CPU_RES_4_C;
   instructionMapCB[0xA2] = &CPU_RES_4_D;
   instructionMapCB[0xA3] = &CPU_RES_4_E;
   instructionMapCB[0xA4] = &CPU_RES_4_H;
   instructionMapCB[0xA5] = &CPU_RES_4_L;
   instructionMapCB[0xA6] = &CPU_RES_4_aHL;
   instructionMapCB[0xAF] = &CPU_RES_5_A;
   instructionMapCB[0xA8] = &CPU_RES_5_B;
   instructionMapCB[0xA9] = &CPU_RES_5_C;
   instructionMapCB[0xAA] = &CPU_RES_5_D;
   instructionMapCB[0xAB] = &CPU_RES_5_E;
   instructionMapCB[0xAC] = &CPU_RES_5_H;
   instructionMapCB[0xAD] = &CPU_RES_5_L;
   instructionMapCB[0xAE] = &CPU_RES_5_aHL;
   instructionMapCB[0xB7] = &CPU_RES_6_A;
   instructionMapCB[0xB0] = &CPU_RES_6_B;
   instructionMapCB[0xB1] = &CPU_RES_6_C;
   instructionMapCB[0xB2] = &CPU_RES_6_D;
   instructionMapCB[0xB3] = &CPU_RES_6_E;
   instructionMapCB[0xB4] = &CPU_RES_6_H;
   instructionMapCB[0xB5] = &CPU_RES_6_L;
   instructionMapCB[0xB6] = &CPU_RES_6_aHL;
   instructionMapCB[0xBF] = &CPU_RES_7_A;
   instructionMapCB[0xB8] = &CPU_RES_7_B;
   instructionMapCB[0xB9] = &CPU_RES_7_C;
   instructionMapCB[0xBA] = &CPU_RES_7_D;
   instructionMapCB[0xBB] = &CPU_RES_7_E;
   instructionMapCB[0xBC] = &CPU_RES_7_H;
   instructionMapCB[0xBD] = &CPU_RES_7_L;
   instructionMapCB[0xBE] = &CPU_RES_7_aHL;

   instructionMapCB[0xC7] = &CPU_SET_0_A;
   instructionMapCB[0xC0] = &CPU_SET_0_B;
   instructionMapCB[0xC1] = &CPU_SET_0_C;
   instructionMapCB[0xC2] = &CPU_SET_0_D;
   instructionMapCB[0xC3] = &CPU_SET_0_E;
   instructionMapCB[0xC4] = &CPU_SET_0_H;
   instructionMapCB[0xC5] = &CPU_SET_0_L;
   instructionMapCB[0xC6] = &CPU_SET_0_aHL;
   instructionMapCB[0xCF] = &CPU_SET_1_A;
   instructionMapCB[0xC8] = &CPU_SET_1_B;
   instructionMapCB[0xC9] = &CPU_SET_1_C;
   instructionMapCB[0xCA] = &CPU_SET_1_D;
   instructionMapCB[0xCB] = &CPU_SET_1_E;
   instructionMapCB[0xCC] = &CPU_SET_1_H;
   instructionMapCB[0xCD] = &CPU_SET_1_L;
   instructionMapCB[0xCE] = &CPU_SET_1_aHL;
   instructionMapCB[0xD7] = &CPU_SET_2_A;
   instructionMapCB[0xD0] = &CPU_SET_2_B;
   instructionMapCB[0xD1] = &CPU_SET_2_C;
   instructionMapCB[0xD2] = &CPU_SET_2_D;
   instructionMapCB[0xD3] = &CPU_SET_2_E;
   instructionMapCB[0xD4] = &CPU_SET_2_H;
   instructionMapCB[0xD5] = &CPU_SET_2_L;
   instructionMapCB[0xD6] = &CPU_SET_2_aHL;
   instructionMapCB[0xDF] = &CPU_SET_3_A;
   instructionMapCB[0xD8] = &CPU_SET_3_B;
   instructionMapCB[0xD9] = &CPU_SET_3_C;
   instructionMapCB[0xDA] = &CPU_SET_3_D;
   instructionMapCB[0xDB] = &CPU_SET_3_E;
   instructionMapCB[0xDC] = &CPU_SET_3_H;
   instructionMapCB[0xDD] = &CPU_SET_3_L;
   instructionMapCB[0xDE] = &CPU_SET_3_aHL;
   instructionMapCB[0xE7] = &CPU_SET_4_A;
   instructionMapCB[0xE0] = &CPU_SET_4_B;
   instructionMapCB[0xE1] = &CPU_SET_4_C;
   instructionMapCB[0xE2] = &CPU_SET_4_D;
   instructionMapCB[0xE3] = &CPU_SET_4_E;
   instructionMapCB[0xE4] = &CPU_SET_4_H;
   instructionMapCB[0xE5] = &CPU_SET_4_L;
   instructionMapCB[0xE6] = &CPU_SET_4_aHL;
   instructionMapCB[0xEF] = &CPU_SET_5_A;
   instructionMapCB[0xE8] = &CPU_SET_5_B;
   instructionMapCB[0xE9] = &CPU_SET_5_C;
   instructionMapCB[0xEA] = &CPU_SET_5_D;
   instructionMapCB[0xEB] = &CPU_SET_5_E;
   instructionMapCB[0xEC] = &CPU_SET_5_H;
   instructionMapCB[0xED] = &CPU_SET_5_L;
   instructionMapCB[0xEE] = &CPU_SET_5_aHL;
   instructionMapCB[0xF7] = &CPU_SET_6_A;
   instructionMapCB[0xF0] = &CPU_SET_6_B;
   instructionMapCB[0xF1] = &CPU_SET_6_C;
   instructionMapCB[0xF2] = &CPU_SET_6_D;
   instructionMapCB[0xF3] = &CPU_SET_6_E;
   instructionMapCB[0xF4] = &CPU_SET_6_H;
   instructionMapCB[0xF5] = &CPU_SET_6_L;
   instructionMapCB[0xF6] = &CPU_SET_6_aHL;
   instructionMapCB[0xFF] = &CPU_SET_7_A;
   instructionMapCB[0xF8] = &CPU_SET_7_B;
   instructionMapCB[0xF9] = &CPU_SET_7_C;
   instructionMapCB[0xFA] = &CPU_SET_7_D;
   instructionMapCB[0xFB] = &CPU_SET_7_E;
   instructionMapCB[0xFC] = &CPU_SET_7_H;
   instructionMapCB[0xFD] = &CPU_SET_7_L;
   instructionMapCB[0xFE] = &CPU_SET_7_aHL;
}
