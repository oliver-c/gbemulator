#include "GB.h"
#include "MMU.h"
#include "CPU.h"
#include "CPU_instructions.h"

#define REG_PC (cpu->registers[PC].value)
#define REG_SP (cpu->registers[SP].value)
#define REG_AF (cpu->registers[AF].value)
#define REG_BC (cpu->registers[BC].value)
#define REG_DE (cpu->registers[DE].value)
#define REG_HL (cpu->registers[HL].value)
#define REG_A  (cpu->registers[AF].bytes.high)
#define REG_F  (cpu->registers[AF].bytes.low)
#define REG_B  (cpu->registers[BC].bytes.high)
#define REG_C  (cpu->registers[BC].bytes.low)
#define REG_D  (cpu->registers[DE].bytes.high)
#define REG_E  (cpu->registers[DE].bytes.low)
#define REG_H  (cpu->registers[HL].bytes.high)
#define REG_L  (cpu->registers[HL].bytes.low)

#define ADD 0
#define SUB 1

struct CPU {
   GB gb;

   bool IME;
   reg registers[NUM_REGISTERS];
};

void CPU_8bitUpdateHalfCarry (CPU cpu, int oldValue, int newValue, int type) {
   int oldNibble = oldValue & 0xF;
   int newNibble = newValue & 0xF;

   if (type == ADD) {
      if (newNibble < oldNibble) CPU_setHalfCarry (cpu);   
   } else {
      if (newNibble > oldNibble) CPU_setHalfCarry (cpu);
   }
}

void CPU_16bitUpdateHalfCarry (CPU cpu, int oldValue, int newValue, int type) {
   int oldNibble = oldValue & 0xF00;
   int newNibble = newValue & 0xF00;

   if (type == ADD) {
      if (newNibble < oldNibble) CPU_setHalfCarry (cpu);   
   } else {
      if (newNibble > oldNibble) CPU_setHalfCarry (cpu);
   }
}

void CPU_8bitADD (CPU cpu, byte *dest, byte *toAdd) {
   int result;
   result = (int)*dest + (int)*toAdd;

   CPU_clearFlags (cpu);
   if (result == 0) CPU_setZero (cpu);
   if (result > 0xFF || result < 0) CPU_setCarry (cpu);
   CPU_8bitUpdateHalfCarry (cpu, *dest, result, ADD);

   *dest += *toAdd;
}

void CPU_8bitADC (CPU cpu, byte *dest, byte *toAdd) {
   int result;
   int C = 0;
   
   if (CPU_isCarrySet (cpu)) C = 1;

   result = (int)*dest + (int)*toAdd + C;

   CPU_clearFlags (cpu);
   if (result == 0) CPU_setZero (cpu);
   if (result > 0xFF || result < 0) CPU_setCarry (cpu);
   CPU_8bitUpdateHalfCarry (cpu, *dest, result, ADD);

   *dest += *toAdd + C;
}

void CPU_8bitSUB (CPU cpu, byte *dest, byte *toSub) {
   int result; 

   CPU_clearFlags (cpu);
   CPU_setSub (cpu);

   result = (int)*dest - (int)*toSub;

   if (result == 0) CPU_setZero (cpu);
   if (result > 0xFF || result < 0) CPU_setCarry (cpu);
   CPU_8bitUpdateHalfCarry (cpu, *dest, result, SUB);

   *dest -= *toSub;
}

void CPU_8bitSBC (CPU cpu, byte *dest, byte *toSub) {
   int result; 
   int C = 0;

   if (CPU_isCarrySet (cpu)) C = 1;

   CPU_clearFlags (cpu);
   CPU_setSub (cpu);

   result = (int)*dest - (int)*toSub - C;

   if (result == 0) CPU_setZero (cpu);
   if (result > 0xFF || result < 0) CPU_setCarry (cpu);
   CPU_8bitUpdateHalfCarry (cpu, *dest, result, SUB);

   *dest -= *toSub + C;
}

void CPU_8bitAND (CPU cpu, byte *dest, byte *toAnd) {
   CPU_clearFlags (cpu);
   CPU_setHalfCarry (cpu);

   *dest &= *toAnd;

   if (*dest == 0) CPU_setZero (cpu);
}

void CPU_8bitOR (CPU cpu, byte *dest, byte *toOr) {
   CPU_clearFlags (cpu);

   *dest |= *toOr;

   if (*dest == 0) CPU_setZero (cpu);
}

void CPU_8bitXOR (CPU cpu, byte *dest, byte *toXor) {
   CPU_clearFlags (cpu);

   *dest ^= *toXor;

   if (*dest == 0) CPU_setZero (cpu);
}

void CPU_8bitCP (CPU cpu, byte *dest, byte *toCp) {
   int result; 

   CPU_clearFlags (cpu);
   CPU_setSub (cpu);

   result = (int)*dest - (int)*toCp;

   if (result == 0) CPU_setZero (cpu);
   if (result > 0xFF || result < 0) CPU_setCarry (cpu);
   CPU_8bitUpdateHalfCarry (cpu, *dest, result, SUB);
}

void CPU_8bitINC (CPU cpu, byte *dest) {
   int result;
   
   CPU_clearZero (cpu); 
   CPU_clearSub (cpu); 
   CPU_clearHalfCarry (cpu);
   result = (int)*dest + 1;

   if (result == 0) CPU_setZero (cpu);
   CPU_8bitUpdateHalfCarry (cpu, *dest, result, ADD);

   (*dest)++;
}

void CPU_8bitDEC (CPU cpu, byte *dest) {
   int result;
   
   CPU_clearZero (cpu); 
   CPU_clearSub (cpu); 
   CPU_clearHalfCarry (cpu);
   CPU_setSub (cpu);

   result = (int)*dest - 1;

   if (result == 0) CPU_setZero (cpu);
   CPU_8bitUpdateHalfCarry (cpu, *dest, result, SUB);

   (*dest)--;
}

void CPU_16bitADD (CPU cpu, word *dest, word *toAdd) {
   int result;

   CPU_clearSub (cpu);
   CPU_clearCarry (cpu);
   CPU_clearHalfCarry (cpu);

   result = (int)*dest + (int)*toAdd;
   if (result < 0 || result > 0xFFFF) CPU_setCarry (cpu);
   CPU_16bitUpdateHalfCarry (cpu, *dest, result, ADD);

   *dest += *toAdd; 
}

int CPU_NOP (CPU cpu) {
   REG_PC++;
   return 4;
}

int CPU_LD_B_n (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_B = MMU_readByte (mmu, REG_PC + 1);
   REG_PC += 2;
   return 8;
}

int CPU_LD_C_n (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_C = MMU_readByte (mmu, REG_PC + 1);
   REG_PC += 2;
   return 8;
}

int CPU_LD_D_n (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_D = MMU_readByte (mmu, REG_PC + 1);
   REG_PC += 2;
   return 8;
}

int CPU_LD_E_n (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_E = MMU_readByte (mmu, REG_PC + 1);
   REG_PC += 2;
   return 8;
}

int CPU_LD_H_n (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_H = MMU_readByte (mmu, REG_PC + 1);
   REG_PC += 2;
   return 8;
}

int CPU_LD_L_n (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_L = MMU_readByte (mmu, REG_PC + 1);
   REG_PC += 2;
   return 8;
}

int CPU_LD_A_A (CPU cpu) {
   REG_PC++;
   return 4;
}

int CPU_LD_A_B (CPU cpu) {
   REG_A = REG_B;
   REG_PC++;
   return 4;
}

int CPU_LD_A_C (CPU cpu) {
   REG_A = REG_C;
   REG_PC++;
   return 4;
}

int CPU_LD_A_D (CPU cpu) {
   REG_A = REG_D;
   REG_PC++;
   return 4;
}

int CPU_LD_A_E (CPU cpu) {
   REG_A = REG_E;
   REG_PC++;
   return 4;
}

int CPU_LD_A_H (CPU cpu) {
   REG_A = REG_H;
   REG_PC++;
   return 4;
}

int CPU_LD_A_L (CPU cpu) {
   REG_A = REG_L;
   REG_PC++;
   return 4;
}

int CPU_LD_A_aHL (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_A = MMU_readByte (mmu, REG_HL);
   REG_PC++;
   return 8;
}

int CPU_LD_A_aBC (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_A = MMU_readByte (mmu, REG_BC);
   REG_PC++;
   return 8;
}

int CPU_LD_A_aDE (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_A = MMU_readByte (mmu, REG_DE);
   REG_PC++;
   return 8;
}

int CPU_LD_A_ann (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   word immediateWord = MMU_readWord (mmu, REG_PC + 1);
   REG_A = MMU_readByte (mmu, immediateWord);
   REG_PC += 3;
   return 16;
}

int CPU_LD_A_hash (CPU cpu) {
   /* ... */
   REG_PC++;
   return 8;
}

int CPU_LD_B_A (CPU cpu) {
   REG_B = REG_A;
   REG_PC++;
   return 4;
}

int CPU_LD_B_B (CPU cpu) {
   REG_PC++;
   return 4;
}

int CPU_LD_B_C (CPU cpu) {
   REG_B = REG_C;
   REG_PC++;
   return 4;
}

int CPU_LD_B_D (CPU cpu) {
   REG_B = REG_D;
   REG_PC++;
   return 4;
}

int CPU_LD_B_E (CPU cpu) {
   REG_B = REG_E;
   REG_PC++;
   return 4;

}

int CPU_LD_B_H (CPU cpu) {
   REG_B = REG_H;
   REG_PC++;
   return 4;
}

int CPU_LD_B_L (CPU cpu) {
   REG_B = REG_L;
   REG_PC++;
   return 4;
}

int CPU_LD_B_aHL (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_B = MMU_readByte (mmu, REG_HL);
   REG_PC++;
   return 8;
}

int CPU_LD_C_A (CPU cpu) {
   REG_C = REG_A;
   REG_PC++;
   return 4;
}

int CPU_LD_C_B (CPU cpu) {
   REG_C = REG_B;
   REG_PC++;
   return 4;
}

int CPU_LD_C_C (CPU cpu) {
   REG_PC++;
   return 4;
}

int CPU_LD_C_D (CPU cpu) {
   REG_C = REG_D;
   REG_PC++;
   return 4;
}

int CPU_LD_C_E (CPU cpu) {
   REG_C = REG_E;
   REG_PC++;
   return 4;

}

int CPU_LD_C_H (CPU cpu) {
   REG_C = REG_H;
   REG_PC++;
   return 4;
}

int CPU_LD_C_L (CPU cpu) {
   REG_C = REG_L;
   REG_PC++;
   return 4;
}

int CPU_LD_C_aHL (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_C = MMU_readByte (mmu, REG_HL);
   REG_PC++;
   return 8;
}

int CPU_LD_D_A (CPU cpu) {
   REG_D = REG_A;
   REG_PC++;
   return 4;
}

int CPU_LD_D_B (CPU cpu) {
   REG_D = REG_B;
   REG_PC++;
   return 4;
}

int CPU_LD_D_C (CPU cpu) {
   REG_D = REG_C;
   REG_PC++;
   return 4;
}

int CPU_LD_D_D (CPU cpu) {
   REG_PC++;
   return 4;
}

int CPU_LD_D_E (CPU cpu) {
   REG_D = REG_E;
   REG_PC++;
   return 4;
}

int CPU_LD_D_H (CPU cpu) {
   REG_D = REG_H;
   REG_PC++;
   return 4;
}

int CPU_LD_D_L (CPU cpu) {
   REG_D = REG_L;
   REG_PC++;
   return 4;
}

int CPU_LD_D_aHL (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_D = MMU_readByte (mmu, REG_HL);
   REG_PC++;
   return 8;
}

int CPU_LD_E_A (CPU cpu) {
   REG_E = REG_A;
   REG_PC++;
   return 4;
}

int CPU_LD_E_B (CPU cpu) {
   REG_E = REG_B;
   REG_PC++;
   return 4;
}

int CPU_LD_E_C (CPU cpu) {
   REG_E = REG_C;
   REG_PC++;
   return 4;
}

int CPU_LD_E_D (CPU cpu) {
   REG_E = REG_D;
   REG_PC++;
   return 4;
}

int CPU_LD_E_E (CPU cpu) {
   REG_PC++;
   return 4;
}

int CPU_LD_E_H (CPU cpu) {
   REG_E = REG_H;
   REG_PC++;
   return 4;
}

int CPU_LD_E_L (CPU cpu) {
   REG_E = REG_L;
   REG_PC++;
   return 4;
}

int CPU_LD_E_aHL (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_E = MMU_readByte (mmu, REG_HL);
   REG_PC++;
   return 8;
}

int CPU_LD_H_A (CPU cpu) {
   REG_H = REG_A;
   REG_PC++;
   return 4;
}

int CPU_LD_H_B (CPU cpu) {
   REG_H = REG_B;
   REG_PC++;
   return 4;
}

int CPU_LD_H_C (CPU cpu) {
   REG_H = REG_C;
   REG_PC++;
   return 4;
}

int CPU_LD_H_D (CPU cpu) {
   REG_H = REG_D;
   REG_PC++;
   return 4;
}

int CPU_LD_H_E (CPU cpu) {
   REG_H = REG_E;
   REG_PC++;
   return 4;
}

int CPU_LD_H_H (CPU cpu) {
   REG_PC++;
   return 4;
}

int CPU_LD_H_L (CPU cpu) {
   REG_H = REG_L;
   REG_PC++;
   return 4;
}

int CPU_LD_H_aHL (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_H = MMU_readByte (mmu, REG_HL);
   REG_PC++;
   return 8;
}

int CPU_LD_L_A (CPU cpu) {
   REG_L = REG_A;
   REG_PC++;
   return 4;
}

int CPU_LD_L_B (CPU cpu) {
   REG_L = REG_B;
   REG_PC++;
   return 4;
}

int CPU_LD_L_C (CPU cpu) {
   REG_L = REG_C;
   REG_PC++;
   return 4;
}

int CPU_LD_L_D (CPU cpu) {
   REG_L = REG_D;
   REG_PC++;
   return 4;
}

int CPU_LD_L_E (CPU cpu) {
   REG_L = REG_E;
   REG_PC++;
   return 4;
}

int CPU_LD_L_H (CPU cpu) {
   REG_L = REG_H;
   REG_PC++;
   return 4;
}

int CPU_LD_L_L (CPU cpu) {
   REG_PC++;
   return 4;
}

int CPU_LD_L_aHL (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_L = MMU_readByte (mmu, REG_HL);
   REG_PC++;
   return 8;
}

int CPU_aHL_A (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   MMU_writeByte (mmu, REG_HL, REG_A);
   REG_PC++;
   return 8;
}

int CPU_aHL_B (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   MMU_writeByte (mmu, REG_HL, REG_B);
   REG_PC++;
   return 8;
}

int CPU_aHL_C (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   MMU_writeByte (mmu, REG_HL, REG_C);
   REG_PC++;
   return 8;
}

int CPU_aHL_D (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   MMU_writeByte (mmu, REG_HL, REG_D);
   REG_PC++;
   return 8;
}

int CPU_aHL_E (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   MMU_writeByte (mmu, REG_HL, REG_E);
   REG_PC++;
   return 8;
}

int CPU_aHL_H (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   MMU_writeByte (mmu, REG_HL, REG_H);
   REG_PC++;
   return 8;
}

int CPU_aHL_L (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   MMU_writeByte (mmu, REG_HL, REG_L);
   REG_PC++;
   return 8;
}

int CPU_aHL_n (CPU cpu) {
   byte byteToWrite;
   MMU mmu = GB_getMMU (cpu->gb);

   byteToWrite = MMU_readByte (mmu, REG_PC + 1);
   MMU_writeByte (mmu, REG_HL, byteToWrite);

   REG_PC += 2;
   return 12;
}

int CPU_aBC_A (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   MMU_writeByte (mmu, REG_BC, REG_A);
   REG_PC++;
   return 8;
}

int CPU_aDE_A (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   MMU_writeByte (mmu, REG_DE, REG_A);
   REG_PC++;
   return 8;
}

int CPU_ann_A (CPU cpu) {
   word location;
   MMU mmu = GB_getMMU (cpu->gb);

   location = MMU_readWord (mmu, REG_PC + 1);
   MMU_writeByte (mmu, location, REG_A);
   REG_PC += 3;
   return 16;
}

int CPU_LD_A_aC (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_A = MMU_readByte (mmu, 0xFF00+REG_C);
   REG_PC++;
   return 8;
}

int CPU_LD_aC_A (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   MMU_writeByte (mmu, 0xFF00+REG_C, REG_A);
   REG_PC++;
   return 8;
}

int CPU_LDD_A_aHL (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_A = MMU_readByte (mmu, REG_HL);
   REG_HL--;
   REG_PC++;
   return 8;
}

int CPU_LDD_aHL_A (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   MMU_writeByte (mmu, REG_HL, REG_A);
   REG_HL--;
   REG_PC++;
   return 8;
}

int CPU_LDI_A_aHL (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_A = MMU_readByte (mmu, REG_HL);
   REG_HL++;
   REG_PC++;
   return 8;
}

int CPU_LDI_aHL_A (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   MMU_writeByte (mmu, REG_HL, REG_A);
   REG_HL++;
   REG_PC++;
   return 8;
}


int CPU_LDH_an_A (CPU cpu) {
   byte immediate;
   MMU mmu = GB_getMMU (cpu->gb);

   immediate = MMU_readByte (mmu, REG_PC+1);
   MMU_writeByte (mmu, 0xFF00+immediate, REG_A);

   REG_PC += 2;
   return 12;
}

int CPU_LDH_A_an (CPU cpu) {
   byte immediate;
   MMU mmu = GB_getMMU (cpu->gb);

   immediate = MMU_readByte (mmu, REG_PC+1);
   REG_A = MMU_readByte (mmu, 0xFF00+immediate);

   REG_PC += 2;
   return 12;
}

int CPU_LD_BC_nn (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_BC = MMU_readWord (mmu, REG_PC + 1);
   REG_PC += 3;
   return 12;
}

int CPU_LD_DE_nn (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_DE = MMU_readWord (mmu, REG_PC + 1);
   REG_PC += 3;
   return 12;
}

int CPU_LD_HL_nn (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_HL = MMU_readWord (mmu, REG_PC + 1);
   REG_PC += 3;
   return 12;
}

int CPU_LD_SP_nn (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_SP = MMU_readWord (mmu, REG_PC + 1);
   REG_PC += 3;
   return 12;
}

int CPU_LD_SP_HL (CPU cpu) {
   REG_SP = REG_HL;
   REG_PC++;
   return 8;
}

int CPU_LDHL_SP_n (CPU cpu) {
   int result;
   signed_byte immediate;
   MMU mmu = GB_getMMU (cpu->gb);

   immediate = (signed_byte)MMU_readByte (mmu, REG_PC+1);
   result = (int)REG_SP + (int)immediate;
   
   CPU_clearFlags (cpu);
   if (result > 0xFFFF || result < 0) CPU_setCarry (cpu);
   CPU_8bitUpdateHalfCarry (cpu, REG_SP, REG_SP+immediate, ADD);

   REG_HL = REG_SP+immediate;
   REG_PC += 2;
   return 12;
}

int CPU_LD_ann_SP (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   word address = MMU_readWord (mmu, REG_PC + 1);
   MMU_writeWord (mmu, address, REG_SP);

   REG_PC += 3;

   return 20;
}

int CPU_PUSH_AF (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_SP -= 2;
   MMU_writeByte (mmu, REG_SP, REG_AF);
   REG_PC++;
   return 16;
}

int CPU_PUSH_BC (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_SP -= 2;
   MMU_writeByte (mmu, REG_SP, REG_BC);
   REG_PC++;
   return 16;
}

int CPU_PUSH_DE (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_SP -= 2;
   MMU_writeByte (mmu, REG_SP, REG_DE);
   REG_PC++;
   return 16;
}

int CPU_PUSH_HL (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_SP -= 2;
   MMU_writeByte (mmu, REG_SP, REG_HL);
   REG_PC++;
   return 16;
}

int CPU_POP_AF (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_AF = MMU_readByte (mmu, REG_SP);
   REG_SP += 2;
   REG_PC++;
   return 12;
}

int CPU_POP_BC (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_BC = MMU_readByte (mmu, REG_SP);
   REG_SP += 2;
   REG_PC++;
   return 12;
}

int CPU_POP_DE (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_DE = MMU_readByte (mmu, REG_SP);
   REG_SP += 2;
   REG_PC++;
   return 12;
}

int CPU_POP_HL (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_HL = MMU_readByte (mmu, REG_SP);
   REG_SP += 2;
   REG_PC++;
   return 12;
}

int CPU_ADD_A_A (CPU cpu) {
   CPU_8bitADD (cpu, &REG_A, &REG_A);
   REG_PC++;
   return 4;
}

int CPU_ADD_A_B (CPU cpu) {
   CPU_8bitADD (cpu, &REG_A, &REG_B);
   REG_PC++;
   return 4;
}

int CPU_ADD_A_C (CPU cpu) {
   CPU_8bitADD (cpu, &REG_A, &REG_C);
   REG_PC++;
   return 4;
}

int CPU_ADD_A_D (CPU cpu) {
   CPU_8bitADD (cpu, &REG_A, &REG_D);
   REG_PC++;
   return 4;
}

int CPU_ADD_A_E (CPU cpu) {
   CPU_8bitADD (cpu, &REG_A, &REG_E);
   REG_PC++;
   return 4;
}

int CPU_ADD_A_H (CPU cpu) {
   CPU_8bitADD (cpu, &REG_A, &REG_H);
   REG_PC++;
   return 4;
}

int CPU_ADD_A_L (CPU cpu) {
   CPU_8bitADD (cpu, &REG_A, &REG_L);
   REG_PC++;
   return 4;
}

int CPU_ADD_A_aHL (CPU cpu) {
   byte byteToAdd;
   MMU mmu = GB_getMMU (cpu->gb);
   
   byteToAdd = MMU_readByte (mmu, REG_HL);
   CPU_8bitADD (cpu, &REG_A, &byteToAdd);
   
   REG_PC++;
   return 8;
}

int CPU_ADD_A_hash (CPU cpu) {
   /* TODO */
   REG_PC++;
   return 8;
}

int CPU_ADC_A_A (CPU cpu) {
   CPU_8bitADC (cpu, &REG_A, &REG_A);
   REG_PC++;
   return 4;
}

int CPU_ADC_A_B (CPU cpu) {
   CPU_8bitADC (cpu, &REG_A, &REG_B);
   REG_PC++;
   return 4;
}

int CPU_ADC_A_C (CPU cpu) {
   CPU_8bitADC (cpu, &REG_A, &REG_C);
   REG_PC++;
   return 4;
}

int CPU_ADC_A_D (CPU cpu) {
   CPU_8bitADC (cpu, &REG_A, &REG_D);
   REG_PC++;
   return 4;
}

int CPU_ADC_A_E (CPU cpu) {
   CPU_8bitADC (cpu, &REG_A, &REG_E);
   REG_PC++;
   return 4;
}

int CPU_ADC_A_H (CPU cpu) {
   CPU_8bitADC (cpu, &REG_A, &REG_H);
   REG_PC++;
   return 4;
}

int CPU_ADC_A_L (CPU cpu) {
   CPU_8bitADC (cpu, &REG_A, &REG_L);
   REG_PC++;
   return 4;
}

int CPU_ADC_A_aHL (CPU cpu) {
   byte byteToAdd;
   MMU mmu = GB_getMMU (cpu->gb);
   
   byteToAdd = MMU_readByte (mmu, REG_HL);
   CPU_8bitADC (cpu, &REG_A, &byteToAdd);
   
   REG_PC++;
   return 8;
}

int CPU_ADC_A_hash (CPU cpu) {
   /* TODO */
   REG_PC++;
   return 8;
}

int CPU_SUB_A_A (CPU cpu) {
   CPU_8bitSUB (cpu, &REG_A, &REG_A);
   REG_PC++;
   return 4;
}

int CPU_SUB_A_B (CPU cpu) {
   CPU_8bitSUB (cpu, &REG_A, &REG_B);
   REG_PC++;
   return 4;
}

int CPU_SUB_A_C (CPU cpu) {
   CPU_8bitSUB (cpu, &REG_A, &REG_C);
   REG_PC++;
   return 4;
}

int CPU_SUB_A_D (CPU cpu) {
   CPU_8bitSUB (cpu, &REG_A, &REG_D);
   REG_PC++;
   return 4;
}

int CPU_SUB_A_E (CPU cpu) {
   CPU_8bitSUB (cpu, &REG_A, &REG_E);
   REG_PC++;
   return 4;
}

int CPU_SUB_A_H (CPU cpu) {
   CPU_8bitSUB (cpu, &REG_A, &REG_H);
   REG_PC++;
   return 4;
}

int CPU_SUB_A_L (CPU cpu) {
   CPU_8bitSUB (cpu, &REG_A, &REG_L);
   REG_PC++;
   return 4;
}

int CPU_SUB_A_aHL (CPU cpu) {
   byte byteToSub;
   MMU mmu = GB_getMMU (cpu->gb);
   
   byteToSub = MMU_readByte (mmu, REG_HL);
   CPU_8bitSUB (cpu, &REG_A, &byteToSub);
   
   REG_PC++;
   return 8;
}

int CPU_SUB_A_hash (CPU cpu) {
   /* TODO */
   REG_PC++;
   return 8;
}

int CPU_SBC_A_A (CPU cpu) {
   CPU_8bitSBC (cpu, &REG_A, &REG_A);
   REG_PC++;
   return 4;
}

int CPU_SBC_A_B (CPU cpu) {
   CPU_8bitSBC (cpu, &REG_A, &REG_B);
   REG_PC++;
   return 4;
}

int CPU_SBC_A_C (CPU cpu) {
   CPU_8bitSBC (cpu, &REG_A, &REG_C);
   REG_PC++;
   return 4;
}

int CPU_SBC_A_D (CPU cpu) {
   CPU_8bitSBC (cpu, &REG_A, &REG_D);
   REG_PC++;
   return 4;
}

int CPU_SBC_A_E (CPU cpu) {
   CPU_8bitSBC (cpu, &REG_A, &REG_E);
   REG_PC++;
   return 4;
}

int CPU_SBC_A_H (CPU cpu) {
   CPU_8bitSBC (cpu, &REG_A, &REG_H);
   REG_PC++;
   return 4;
}

int CPU_SBC_A_L (CPU cpu) {
   CPU_8bitSBC (cpu, &REG_A, &REG_L);
   REG_PC++;
   return 4;
}

int CPU_SBC_A_aHL (CPU cpu) {
   byte byteToSub;
   MMU mmu = GB_getMMU (cpu->gb);
   
   byteToSub = MMU_readByte (mmu, REG_HL);
   CPU_8bitSBC (cpu, &REG_A, &byteToSub);
   
   REG_PC++;
   return 8;
}

int CPU_AND_A_A (CPU cpu) {
   CPU_8bitAND (cpu, &REG_A, &REG_A);
   REG_PC++;
   return 4;
}

int CPU_AND_A_B (CPU cpu) {
   CPU_8bitAND (cpu, &REG_A, &REG_B);
   REG_PC++;
   return 4;
}

int CPU_AND_A_C (CPU cpu) {
   CPU_8bitAND (cpu, &REG_A, &REG_C);
   REG_PC++;
   return 4;
}

int CPU_AND_A_D (CPU cpu) {
   CPU_8bitAND (cpu, &REG_A, &REG_D);
   REG_PC++;
   return 4;
}

int CPU_AND_A_E (CPU cpu) {
   CPU_8bitAND (cpu, &REG_A, &REG_E);
   REG_PC++;
   return 4;
}

int CPU_AND_A_H (CPU cpu) {
   CPU_8bitAND (cpu, &REG_A, &REG_H);
   REG_PC++;
   return 4;
}

int CPU_AND_A_L (CPU cpu) {
   CPU_8bitAND (cpu, &REG_A, &REG_L);
   REG_PC++;
   return 4;
}

int CPU_AND_A_aHL (CPU cpu) {
   byte byteToAnd;
   MMU mmu = GB_getMMU (cpu->gb);
   
   byteToAnd = MMU_readByte (mmu, REG_HL);
   CPU_8bitAND (cpu, &REG_A, &byteToAnd);
   
   REG_PC++;
   return 8;
}

int CPU_AND_A_hash (CPU cpu) {
   /* TODO */
   REG_PC++;
   return 8;
}

int CPU_OR_A_A (CPU cpu) {
   CPU_8bitOR (cpu, &REG_A, &REG_A);
   REG_PC++;
   return 4;
}

int CPU_OR_A_B (CPU cpu) {
   CPU_8bitOR (cpu, &REG_A, &REG_B);
   REG_PC++;
   return 4;
}

int CPU_OR_A_C (CPU cpu) {
   CPU_8bitOR (cpu, &REG_A, &REG_C);
   REG_PC++;
   return 4;
}

int CPU_OR_A_D (CPU cpu) {
   CPU_8bitOR (cpu, &REG_A, &REG_D);
   REG_PC++;
   return 4;
}

int CPU_OR_A_E (CPU cpu) {
   CPU_8bitOR (cpu, &REG_A, &REG_E);
   REG_PC++;
   return 4;
}

int CPU_OR_A_H (CPU cpu) {
   CPU_8bitOR (cpu, &REG_A, &REG_H);
   REG_PC++;
   return 4;
}

int CPU_OR_A_L (CPU cpu) {
   CPU_8bitOR (cpu, &REG_A, &REG_L);
   REG_PC++;
   return 4;
}

int CPU_OR_A_aHL (CPU cpu) {
   byte byteToOr;
   MMU mmu = GB_getMMU (cpu->gb);
   
   byteToOr = MMU_readByte (mmu, REG_HL);
   CPU_8bitOR (cpu, &REG_A, &byteToOr);
   
   REG_PC++;
   return 8;
}

int CPU_OR_A_hash (CPU cpu) {
   /* TODO */
   REG_PC++;
   return 8;
}

int CPU_XOR_A_A (CPU cpu) {
   CPU_8bitXOR (cpu, &REG_A, &REG_A);
   REG_PC++;
   return 4;
}

int CPU_XOR_A_B (CPU cpu) {
   CPU_8bitXOR (cpu, &REG_A, &REG_B);
   REG_PC++;
   return 4;
}

int CPU_XOR_A_C (CPU cpu) {
   CPU_8bitXOR (cpu, &REG_A, &REG_C);
   REG_PC++;
   return 4;
}

int CPU_XOR_A_D (CPU cpu) {
   CPU_8bitXOR (cpu, &REG_A, &REG_D);
   REG_PC++;
   return 4;
}

int CPU_XOR_A_E (CPU cpu) {
   CPU_8bitXOR (cpu, &REG_A, &REG_E);
   REG_PC++;
   return 4;
}

int CPU_XOR_A_H (CPU cpu) {
   CPU_8bitXOR (cpu, &REG_A, &REG_H);
   REG_PC++;
   return 4;
}

int CPU_XOR_A_L (CPU cpu) {
   CPU_8bitXOR (cpu, &REG_A, &REG_L);
   REG_PC++;
   return 4;
}

int CPU_XOR_A_aHL (CPU cpu) {
   byte byteToXor;
   MMU mmu = GB_getMMU (cpu->gb);
   
   byteToXor = MMU_readByte (mmu, REG_HL);
   CPU_8bitXOR (cpu, &REG_A, &byteToXor);
   
   REG_PC++;
   return 8;
}

int CPU_XOR_A_hash (CPU cpu) {
   /* TODO */
   REG_PC++;
   return 8;
}

int CPU_CP_A_A (CPU cpu) {
   CPU_8bitCP (cpu, &REG_A, &REG_A);
   REG_PC++;
   return 4;
}

int CPU_CP_A_B (CPU cpu) {
   CPU_8bitCP (cpu, &REG_A, &REG_B);
   REG_PC++;
   return 4;
}

int CPU_CP_A_C (CPU cpu) {
   CPU_8bitCP (cpu, &REG_A, &REG_C);
   REG_PC++;
   return 4;
}

int CPU_CP_A_D (CPU cpu) {
   CPU_8bitCP (cpu, &REG_A, &REG_D);
   REG_PC++;
   return 4;
}

int CPU_CP_A_E (CPU cpu) {
   CPU_8bitCP (cpu, &REG_A, &REG_E);
   REG_PC++;
   return 4;
}

int CPU_CP_A_H (CPU cpu) {
   CPU_8bitCP (cpu, &REG_A, &REG_H);
   REG_PC++;
   return 4;
}

int CPU_CP_A_L (CPU cpu) {
   CPU_8bitCP (cpu, &REG_A, &REG_L);
   REG_PC++;
   return 4;
}

int CPU_CP_A_aHL (CPU cpu) {
   byte byteToCp;
   MMU mmu = GB_getMMU (cpu->gb);
   
   byteToCp = MMU_readByte (mmu, REG_HL);
   CPU_8bitCP (cpu, &REG_A, &byteToCp);
   
   REG_PC++;
   return 8;
}

int CPU_CP_A_hash (CPU cpu) {
   /* TODO */
   REG_PC++;
   return 8;
}

int CPU_INC_A (CPU cpu) {
   CPU_8bitINC (cpu, &REG_A);
   REG_PC++;
   return 4;
}

int CPU_INC_B (CPU cpu) {  
   CPU_8bitINC (cpu, &REG_B);
   REG_PC++;
   return 4;
}

int CPU_INC_C (CPU cpu) {
   CPU_8bitINC (cpu, &REG_C);
   REG_PC++;
   return 4;
}

int CPU_INC_D (CPU cpu) {
   CPU_8bitINC (cpu, &REG_D);
   REG_PC++;
   return 4;
}

int CPU_INC_E (CPU cpu) {
   CPU_8bitINC (cpu, &REG_E);
   REG_PC++;
   return 4;
}

int CPU_INC_H (CPU cpu) {
   CPU_8bitINC (cpu, &REG_H);
   REG_PC++;
   return 4;
}

int CPU_INC_L (CPU cpu) {
   CPU_8bitINC (cpu, &REG_L);
   REG_PC++;
   return 4;
}

int CPU_INC_aHL (CPU cpu) {
   byte byteToInc;
   MMU mmu = GB_getMMU (cpu->gb);
   
   byteToInc = MMU_readByte (mmu, REG_HL);
   CPU_8bitINC (cpu, &byteToInc);
   MMU_writeByte (mmu, REG_HL, byteToInc);
   REG_PC++;
   return 12;
}

int CPU_DEC_A (CPU cpu) {
   CPU_8bitDEC (cpu, &REG_A);
   REG_PC++;
   return 4;
}

int CPU_DEC_B (CPU cpu) {  
   CPU_8bitDEC (cpu, &REG_B);
   REG_PC++;
   return 4;
}

int CPU_DEC_C (CPU cpu) {
   CPU_8bitDEC (cpu, &REG_C);
   REG_PC++;
   return 4;
}

int CPU_DEC_D (CPU cpu) {
   CPU_8bitDEC (cpu, &REG_D);
   REG_PC++;
   return 4;
}

int CPU_DEC_E (CPU cpu) {
   CPU_8bitDEC (cpu, &REG_E);
   REG_PC++;
   return 4;
}

int CPU_DEC_H (CPU cpu) {
   CPU_8bitDEC (cpu, &REG_H);
   REG_PC++;
   return 4;
}

int CPU_DEC_L (CPU cpu) {
   CPU_8bitDEC (cpu, &REG_L);
   REG_PC++;
   return 4;
}

int CPU_DEC_aHL (CPU cpu) {
   byte byteToDec;
   MMU mmu = GB_getMMU (cpu->gb);
   
   byteToDec = MMU_readByte (mmu, REG_HL);
   CPU_8bitDEC (cpu, &byteToDec);
   MMU_writeByte (mmu, REG_HL, byteToDec);

   REG_PC++;
   return 12;
}

int CPU_ADD_HL_BC (CPU cpu) {
   CPU_16bitADD (cpu, &REG_HL, &REG_BC);
   REG_PC++;
   return 8;
}

int CPU_ADD_HL_DE (CPU cpu) {
   CPU_16bitADD (cpu, &REG_HL, &REG_DE);
   REG_PC++;
   return 8;
}

int CPU_ADD_HL_HL (CPU cpu) {
   CPU_16bitADD (cpu, &REG_HL, &REG_HL);
   REG_PC++;
   return 8;
}

int CPU_ADD_HL_SP (CPU cpu) {
   CPU_16bitADD (cpu, &REG_HL, &REG_SP);
   REG_PC++;
   return 8;
}

int CPU_ADD_SP_n (CPU cpu) {
   int result;
   signed_byte immediate;
   MMU mmu = GB_getMMU (cpu->gb);

   CPU_clearFlags (cpu);

   immediate = (signed_byte)MMU_readByte (mmu, REG_PC + 1); 
   
   result = (int)REG_SP + (int)immediate;
   if (result < 0 || result > 0xFFFF) CPU_setCarry (cpu);

   if (immediate >= 0) {
      CPU_16bitUpdateHalfCarry (cpu, REG_SP, result, ADD);
   } else {
      CPU_16bitUpdateHalfCarry (cpu, REG_SP, result, SUB);
   }

   REG_PC += 2;
   return 16;
}

int CPU_INC_BC (CPU cpu) {
   REG_BC++;
   REG_PC++;
   return 8;
}

int CPU_INC_DE (CPU cpu) {
   REG_DE++;
   REG_PC++;
   return 8;
}

int CPU_INC_HL (CPU cpu) {
   REG_HL++;
   REG_PC++;
   return 8;
}

int CPU_INC_SP (CPU cpu) {
   REG_SP++;
   REG_PC++;
   return 8;
}

int CPU_DEC_BC (CPU cpu) {
   REG_BC--;
   REG_PC++;
   return 8;
}

int CPU_DEC_DE (CPU cpu) {
   REG_DE--;
   REG_PC++;
   return 8;
}

int CPU_DEC_HL (CPU cpu) {
   REG_HL--;
   REG_PC++;
   return 8;
}

int CPU_DEC_SP (CPU cpu) {
   REG_SP--;
   REG_PC++;
   return 8;
}
