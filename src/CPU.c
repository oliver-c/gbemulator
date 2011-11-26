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

/* An array of pointers to the functions that will execute the CPU instructions */
int (*instructionMap[0x100])(CPU) = {NULL};

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
   if (instructionMap[opcode] != NULL) {
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
}
