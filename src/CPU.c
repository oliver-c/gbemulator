#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "GB.h"
#include "CPU.h"
#include "CPU_instructions.h"
#include "MMU.h"

struct CPU {
   GB gb;

   reg registers[NUM_REGISTERS];
};

/* Instruction map */
typedef int (*instructionFunction)(CPU);
instructionFunction instructionMap[0x100] = {NULL};

void CPU_initInstructionMap (void);

CPU CPU_init (GB gb) {
   CPU newCPU = (CPU)malloc(sizeof(struct CPU));
   assert (newCPU != NULL);

   newCPU->gb = gb;
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
   numCycles = instructionMap[opcode] (cpu);

   return numCycles;
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
}
