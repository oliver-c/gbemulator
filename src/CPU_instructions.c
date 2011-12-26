#include "GB.h"
#include "MMU.h"
#include "CPU.h"
#include "CPU_instructions.h"
#include "bitOperations.h"

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
   int oldNibble = (oldValue & 0xF00) >> 8;
   int newNibble = (newValue & 0xF00) >> 8;

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
   if (result < 0 || result > 0xFF) CPU_setCarry (cpu);
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
   if (result < 0 || result > 0xFF) CPU_setCarry (cpu);
   CPU_8bitUpdateHalfCarry (cpu, *dest, result, ADD);

   *dest += *toAdd + C;
}

void CPU_8bitSUB (CPU cpu, byte *dest, byte *toSub) {
   int result; 

   CPU_clearFlags (cpu);
   CPU_setSub (cpu);

   result = (int)*dest - (int)*toSub;

   if (result == 0) CPU_setZero (cpu);
   if (result < 0 || result > 0xFF) CPU_setCarry (cpu);
   CPU_8bitUpdateHalfCarry (cpu, *dest, result, SUB);

   *dest -= *toSub;
}

void CPU_8bitSBC (CPU cpu, byte *dest, byte *toSub) {
   int result; 
   int C = 0;

   if (!CPU_isCarrySet (cpu)) C = 1;

   CPU_clearFlags (cpu);
   CPU_setSub (cpu);

   result = (int)*dest - (int)*toSub - C;

   if (result == 0) CPU_setZero (cpu);
   if (result < 0 || result > 0xFF) CPU_setCarry (cpu);
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
   if (result < 0 || result > 0xFF) CPU_setCarry (cpu);
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

void CPU_8bitSWAP (CPU cpu, byte *dest) {
   byte upperNibble;
   byte lowerNibble;

   CPU_clearFlags (cpu);

   upperNibble = (*dest & 0xF0) >> 4;
   lowerNibble = *dest & 0x0F;

   *dest = (lowerNibble << 4) | upperNibble; 

   if (*dest == 0) CPU_setZero (cpu);
}

void CPU_8bitRLC (CPU cpu, byte *dest) {
   CPU_clearFlags (cpu);
   if (testBit(*dest, 7)) CPU_setCarry (cpu);

   *dest <<= 1;
   if (CPU_isCarrySet(cpu)) setBit (dest, 0);
   if (*dest == 0) CPU_setZero (cpu);
}

void CPU_8bitRL (CPU cpu, byte *dest) {
   bool isCarryOriginallySet = FALSE;

   isCarryOriginallySet = CPU_isCarrySet (cpu);
   CPU_clearFlags (cpu);

   if (testBit(*dest, 7)) CPU_setCarry (cpu);

   *dest <<= 1;
   if (isCarryOriginallySet) setBit (dest, 0);
   if (*dest == 0) CPU_setZero (cpu);
}

void CPU_8bitRRC (CPU cpu, byte *dest) {
   CPU_clearFlags (cpu);
   if (testBit(*dest, 0)) CPU_setCarry (cpu);

   *dest >>= 1;
   if (CPU_isCarrySet(cpu)) setBit (dest, 7);
   if (*dest == 0) CPU_setZero (cpu);
}

void CPU_8bitRR (CPU cpu, byte *dest) {
   bool isCarryOriginallySet = FALSE;

   isCarryOriginallySet = CPU_isCarrySet (cpu);
   CPU_clearFlags (cpu);

   if (testBit(*dest, 0)) CPU_setCarry (cpu);

   *dest >>= 1;
   if (isCarryOriginallySet) setBit (dest, 7);
   if (*dest == 0) CPU_setZero (cpu);
}

void CPU_8bitSLA (CPU cpu, byte *dest) {
   CPU_clearFlags (cpu); 
   if (testBit(*dest, 7)) CPU_setCarry (cpu);

   *dest <<= 1;
   if (*dest == 0) CPU_setZero (cpu);
}

void CPU_8bitSRA (CPU cpu, byte *dest) {
   bool isMsbOriginallySet = testBit (*dest, 7);
   CPU_clearFlags (cpu);
   if (testBit(*dest, 0)) CPU_setCarry (cpu);

   *dest >>= 1;
   if (isMsbOriginallySet) setBit (dest, 7);
   if (*dest == 0) CPU_setZero (cpu);
}

void CPU_8bitSRL (CPU cpu, byte *dest) {
   CPU_clearFlags (cpu); 
   if (testBit(*dest, 0)) CPU_setCarry (cpu);

   *dest >>= 1;
   if (*dest == 0) CPU_setZero (cpu);
}

void CPU_8bitBIT (CPU cpu, byte *dest, int bit) {
   CPU_clearZero (cpu);
   CPU_clearSub (cpu);
   CPU_setHalfCarry (cpu);

   if (!testBit(*dest, bit)) CPU_setZero (cpu);
}

void CPU_8bitSET (CPU cpu, byte *dest, int bit) {
   setBit (dest, bit); 
}

void CPU_8bitRES (CPU cpu, byte *dest, int bit) {
   clearBit (dest, bit);
}

void CPU_RST (CPU cpu, byte n) {
   MMU mmu = GB_getMMU (cpu->gb);

   REG_PC++;

   REG_SP -= 2;
   MMU_writeWord (mmu, REG_SP, REG_PC);
   
   REG_PC = n;
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

int CPU_LD_A_n (CPU cpu) {
   /* ... */
   MMU mmu = GB_getMMU (cpu->gb);
   REG_A = MMU_readByte (mmu, REG_PC+1);
   REG_PC += 2;
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

int CPU_LD_aHL_A (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   MMU_writeByte (mmu, REG_HL, REG_A);
   REG_PC++;
   return 8;
}

int CPU_LD_aHL_B (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   MMU_writeByte (mmu, REG_HL, REG_B);
   REG_PC++;
   return 8;
}

int CPU_LD_aHL_C (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   MMU_writeByte (mmu, REG_HL, REG_C);
   REG_PC++;
   return 8;
}

int CPU_LD_aHL_D (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   MMU_writeByte (mmu, REG_HL, REG_D);
   REG_PC++;
   return 8;
}

int CPU_LD_aHL_E (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   MMU_writeByte (mmu, REG_HL, REG_E);
   REG_PC++;
   return 8;
}

int CPU_LD_aHL_H (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   MMU_writeByte (mmu, REG_HL, REG_H);
   REG_PC++;
   return 8;
}

int CPU_LD_aHL_L (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   MMU_writeByte (mmu, REG_HL, REG_L);
   REG_PC++;
   return 8;
}

int CPU_LD_aHL_n (CPU cpu) {
   byte byteToWrite;
   MMU mmu = GB_getMMU (cpu->gb);

   byteToWrite = MMU_readByte (mmu, REG_PC + 1);
   MMU_writeByte (mmu, REG_HL, byteToWrite);

   REG_PC += 2;
   return 12;
}

int CPU_LD_aBC_A (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   MMU_writeByte (mmu, REG_BC, REG_A);
   REG_PC++;
   return 8;
}

int CPU_LD_aDE_A (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   MMU_writeByte (mmu, REG_DE, REG_A);
   REG_PC++;
   return 8;
}

int CPU_LD_ann_A (CPU cpu) {
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
   if (result < 0 || result > 0xFFFF) CPU_setCarry (cpu);
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
   MMU_writeWord (mmu, REG_SP, REG_AF);
   REG_PC++;
   return 16;
}

int CPU_PUSH_BC (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_SP -= 2;
   MMU_writeWord (mmu, REG_SP, REG_BC);
   REG_PC++;
   return 16;
}

int CPU_PUSH_DE (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_SP -= 2;
   MMU_writeWord (mmu, REG_SP, REG_DE);
   REG_PC++;
   return 16;
}

int CPU_PUSH_HL (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_SP -= 2;
   MMU_writeWord (mmu, REG_SP, REG_HL);
   REG_PC++;
   return 16;
}

int CPU_POP_AF (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_AF = MMU_readWord (mmu, REG_SP);
   REG_SP += 2;
   REG_PC++;
   return 12;
}

int CPU_POP_BC (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_BC = MMU_readWord (mmu, REG_SP);
   REG_SP += 2;
   REG_PC++;
   return 12;
}

int CPU_POP_DE (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_DE = MMU_readWord (mmu, REG_SP);
   REG_SP += 2;
   REG_PC++;
   return 12;
}

int CPU_POP_HL (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_HL = MMU_readWord (mmu, REG_SP);
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

int CPU_ADD_A_n (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_A += MMU_readByte (mmu, REG_PC+1);
   REG_PC += 2;
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

int CPU_ADC_A_n (CPU cpu) {
   byte byteToAdd;
   MMU mmu = GB_getMMU (cpu->gb);

   byteToAdd = MMU_readByte (mmu, REG_PC+1);
   CPU_8bitADC (cpu, &REG_A, &byteToAdd);

   REG_PC += 2;
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

int CPU_SUB_A_n (CPU cpu) {
   byte byteToSub;
   MMU mmu = GB_getMMU (cpu->gb);
   
   byteToSub = MMU_readByte (mmu, REG_PC+1);
   CPU_8bitSUB (cpu, &REG_A, &byteToSub);

   REG_PC += 2;
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

int CPU_AND_A_n (CPU cpu) {
   byte byteToAnd;
   MMU mmu = GB_getMMU (cpu->gb);
   
   byteToAnd = MMU_readByte (mmu, REG_PC+1);
   CPU_8bitAND (cpu, &REG_A, &byteToAnd);

   REG_PC += 2;
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

int CPU_OR_A_n (CPU cpu) {
   byte byteToOr;
   MMU mmu = GB_getMMU (cpu->gb);
   
   byteToOr = MMU_readByte (mmu, REG_PC+1);
   CPU_8bitOR (cpu, &REG_A, &byteToOr);

   REG_PC += 2;
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

int CPU_XOR_A_n (CPU cpu) {
   byte byteToXor;
   MMU mmu = GB_getMMU (cpu->gb);
   
   byteToXor = MMU_readByte (mmu, REG_PC+1);
   CPU_8bitXOR (cpu, &REG_A, &byteToXor);

   REG_PC += 2;
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

int CPU_CP_A_n (CPU cpu) {
   byte byteToCp;
   MMU mmu = GB_getMMU (cpu->gb);
   
   byteToCp = MMU_readByte (mmu, REG_PC+1);
   CPU_8bitCP (cpu, &REG_A, &byteToCp);
   
   REG_PC += 2;
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

int CPU_DAA (CPU cpu) {
   /* TODO */
   CPU_clearZero (cpu);
   CPU_clearHalfCarry (cpu);

   if (REG_A == 0) CPU_setZero (cpu);

   REG_PC++;
   return 4;
}

int CPU_CPL (CPU cpu) {
   CPU_setSub (cpu);
   CPU_setHalfCarry (cpu);

   REG_A = ~REG_A;

   REG_PC++;
   return 4;
}

int CPU_CCF (CPU cpu) {
   CPU_clearSub (cpu);
   CPU_clearHalfCarry (cpu);

   if (CPU_isCarrySet(cpu)) {
      CPU_clearCarry (cpu);
   } else {
      CPU_setCarry (cpu);
   }

   REG_PC++;
   return 4;
}

int CPU_SCF (CPU cpu) {
   CPU_clearSub (cpu);
   CPU_clearHalfCarry (cpu);
   CPU_setCarry (cpu);
   
   REG_PC++;
   return 4;
}

int CPU_HALT (CPU cpu) {
   REG_PC++;
   return 4;
}

int CPU_STOP (CPU cpu) {
   REG_PC += 2;
   return 4;
}

int CPU_DI (CPU cpu) {
   CPU_setIME (cpu, FALSE);
   REG_PC++;
   return 4;
}

int CPU_EI (CPU cpu) {
   CPU_setIME (cpu, TRUE);
   REG_PC++;
   return 4;
}

int CPU_RLCA (CPU cpu) {
   CPU_8bitRLC (cpu, &REG_A);
   REG_PC++;
   return 4;
}

int CPU_RLA (CPU cpu) {
   CPU_8bitRL (cpu, &REG_A);
   REG_PC++;
   return 4;
}

int CPU_RRCA (CPU cpu) {
   CPU_8bitRRC (cpu, &REG_A);
   REG_PC++;
   return 4;
}

int CPU_RRA (CPU cpu) {
   CPU_8bitRR (cpu, &REG_A);
   REG_PC++;
   return 4;
}

int CPU_JP_nn (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_PC = MMU_readWord (mmu, REG_PC+1);
   return 12;
}

int CPU_JPNZ_nn (CPU cpu) {
   if (!CPU_isZeroSet(cpu)) {
      CPU_JP_nn (cpu);
   } else {
      REG_PC += 3;
   }

   return 12;
}

int CPU_JPZ_nn (CPU cpu) {
   if (CPU_isZeroSet(cpu)) {
      CPU_JP_nn (cpu);
   } else {
      REG_PC += 3;
   }

   return 12;
}

int CPU_JPNC_nn (CPU cpu) {
   if (!CPU_isCarrySet(cpu)) {
      CPU_JP_nn (cpu);
   } else {
      REG_PC += 3;
   }

   return 12;
}

int CPU_JPC_nn (CPU cpu) {
   if (CPU_isCarrySet(cpu)) {
      CPU_JP_nn (cpu);
   } else {
      REG_PC += 3;
   }

   return 12;
}

int CPU_JP_HL (CPU cpu) {
   REG_PC = REG_HL;
   return 4;
}

int CPU_JR_n (CPU cpu) {
   signed_byte immediate;
   MMU mmu = GB_getMMU (cpu->gb);

   immediate = (signed_byte)MMU_readByte (mmu, REG_PC + 1);
   REG_PC += 2;

   REG_PC += immediate;
   return 8;
}

int CPU_JRNZ_n (CPU cpu) {
   if (!CPU_isZeroSet (cpu)) {
      CPU_JR_n (cpu);
   } else {
      REG_PC += 2;
   }
   return 8;
}

int CPU_JRZ_n (CPU cpu) {
   if (CPU_isZeroSet (cpu)) {
      CPU_JR_n (cpu);
   } else {
      REG_PC += 2;
   }
   return 8;
}

int CPU_JRNC_n (CPU cpu) {
   if (!CPU_isCarrySet (cpu)) {
      CPU_JR_n (cpu);
   } else {
      REG_PC += 2;
   }
   return 8;
}

int CPU_JRC_n (CPU cpu) {
   if (CPU_isCarrySet (cpu)) {
      CPU_JR_n (cpu);
   } else {
      REG_PC += 2;
   }
   return 8;
}

int CPU_CALL_nn (CPU cpu) {
   word jumpTo;
   MMU mmu = GB_getMMU (cpu->gb);
   
   jumpTo = MMU_readWord (mmu, REG_PC+1);

   /* Push return address */
   REG_SP -= 2;
   MMU_writeWord (mmu, REG_SP, REG_PC+3);
   
   REG_PC = jumpTo;

   return 12;
}

int CPU_CALLNZ_nn (CPU cpu) {
   if (!CPU_isZeroSet (cpu)) {
      CPU_CALL_nn (cpu);
   } else {
      REG_PC += 3; 
   }

   return 12;
}

int CPU_CALLZ_nn (CPU cpu) {
   if (CPU_isZeroSet (cpu)) {
      CPU_CALL_nn (cpu);
   } else {
      REG_PC += 3; 
   }

   return 12;
}

int CPU_CALLNC_nn (CPU cpu) {
   if (!CPU_isCarrySet (cpu)) {
      CPU_CALL_nn (cpu);
   } else {
      REG_PC += 3; 
   }

   return 12;
}

int CPU_CALLC_nn (CPU cpu) {
   if (CPU_isCarrySet (cpu)) {
      CPU_CALL_nn (cpu);
   } else {
      REG_PC += 3; 
   }

   return 12;
}

int CPU_RST_00H (CPU cpu) {
   CPU_RST (cpu, 0x00);
   return 32;
}

int CPU_RST_08H (CPU cpu) {
   CPU_RST (cpu, 0x08);
   return 32;
}

int CPU_RST_10H (CPU cpu) {
   CPU_RST (cpu, 0x10);
   return 32;
}

int CPU_RST_18H (CPU cpu) {
   CPU_RST (cpu, 0x18);
   return 32;
}

int CPU_RST_20H (CPU cpu) {
   CPU_RST (cpu, 0x20);
   return 32;
}

int CPU_RST_28H (CPU cpu) {
   CPU_RST (cpu, 0x28);
   return 32;
}

int CPU_RST_30H (CPU cpu) {
   CPU_RST (cpu, 0x30);
   return 32;
}

int CPU_RST_38H (CPU cpu) {
   CPU_RST (cpu, 0x38);
   return 32;
}

int CPU_RET (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   REG_PC = MMU_readWord (mmu, REG_SP);
   REG_SP += 2;

   return 8;
}

int CPU_RETNZ (CPU cpu) {
   if (!CPU_isZeroSet (cpu)) {
      CPU_RET (cpu);
   } else {
      REG_PC++;
   }
   return 8;
}

int CPU_RETZ (CPU cpu) {
   if (CPU_isZeroSet (cpu)) {
      CPU_RET (cpu);
   } else {
      REG_PC++;
   }
   return 8;
}

int CPU_RETNC (CPU cpu) {
   if (!CPU_isCarrySet (cpu)) {
      CPU_RET (cpu);
   } else {
      REG_PC++;
   }
   return 8;
}

int CPU_RETC (CPU cpu) {
   if (CPU_isCarrySet (cpu)) {
      CPU_RET (cpu);
   } else {
      REG_PC++;
   }
   return 8;
}

int CPU_RETI (CPU cpu) {
   CPU_RET (cpu);
   CPU_setIME (cpu, TRUE);
   return 8;
}

int CPU_SWAP_A (CPU cpu) {
   CPU_8bitSWAP (cpu, &REG_A);
   REG_PC += 2;
   return 8;
}

int CPU_SWAP_B (CPU cpu) {
   CPU_8bitSWAP (cpu, &REG_B);
   REG_PC += 2;
   return 8;
}

int CPU_SWAP_C (CPU cpu) {
   CPU_8bitSWAP (cpu, &REG_C);
   REG_PC += 2;
   return 8;
}

int CPU_SWAP_D (CPU cpu) {
   CPU_8bitSWAP (cpu, &REG_D);
   REG_PC += 2;
   return 8;
}

int CPU_SWAP_E (CPU cpu) {
   CPU_8bitSWAP (cpu, &REG_E);
   REG_PC += 2;
   return 8;
}

int CPU_SWAP_H (CPU cpu) {
   CPU_8bitSWAP (cpu, &REG_H);
   REG_PC += 2;
   return 8;
}

int CPU_SWAP_L (CPU cpu) {
   CPU_8bitSWAP (cpu, &REG_L);
   REG_PC += 2;
   return 8;
}

int CPU_SWAP_aHL (CPU cpu) {
   byte byteToSwap;
   MMU mmu = GB_getMMU (cpu->gb);

   byteToSwap = MMU_readByte (mmu, REG_HL);
   CPU_8bitSWAP (cpu, &byteToSwap);
   MMU_writeByte (mmu, REG_HL, byteToSwap);

   REG_PC += 2;
   return 16;
}

int CPU_RLC_A (CPU cpu) {
   CPU_8bitRLC (cpu, &REG_A);
   REG_PC += 2;
   return 8;
}

int CPU_RLC_B (CPU cpu) {
   CPU_8bitRLC (cpu, &REG_B);
   REG_PC += 2;
   return 8;
}

int CPU_RLC_C (CPU cpu) {
   CPU_8bitRLC (cpu, &REG_C);
   REG_PC += 2;
   return 8;
}

int CPU_RLC_D (CPU cpu) {
   CPU_8bitRLC (cpu, &REG_D);
   REG_PC += 2;
   return 8;
}

int CPU_RLC_E (CPU cpu) {
   CPU_8bitRLC (cpu, &REG_E);
   REG_PC += 2;
   return 8;
}

int CPU_RLC_H (CPU cpu) {
   CPU_8bitRLC (cpu, &REG_H);
   REG_PC += 2;
   return 8;
}

int CPU_RLC_L (CPU cpu) {
   CPU_8bitRLC (cpu, &REG_L);
   REG_PC += 2;
   return 8;
}

int CPU_RLC_aHL (CPU cpu) {
   byte byteToRotate;
   MMU mmu = GB_getMMU (cpu->gb);

   byteToRotate = MMU_readByte (mmu, REG_HL);
   CPU_8bitRLC (cpu, &byteToRotate);
   MMU_writeByte (mmu, REG_HL, byteToRotate);

   REG_PC += 2;
   return 16;
}

int CPU_RL_A (CPU cpu) {
   CPU_8bitRL (cpu, &REG_A);
   REG_PC += 2;
   return 8;
}

int CPU_RL_B (CPU cpu) {
   CPU_8bitRL (cpu, &REG_B);
   REG_PC += 2;
   return 8;
}

int CPU_RL_C (CPU cpu) {
   CPU_8bitRL (cpu, &REG_C);
   REG_PC += 2;
   return 8;
}

int CPU_RL_D (CPU cpu) {
   CPU_8bitRL (cpu, &REG_D);
   REG_PC += 2;
   return 8;
}

int CPU_RL_E (CPU cpu) {
   CPU_8bitRL (cpu, &REG_E);
   REG_PC += 2;
   return 8;
}

int CPU_RL_H (CPU cpu) {
   CPU_8bitRL (cpu, &REG_H);
   REG_PC += 2;
   return 8;
}

int CPU_RL_L (CPU cpu) {
   CPU_8bitRL (cpu, &REG_L);
   REG_PC += 2;
   return 8;
}

int CPU_RL_aHL (CPU cpu) {
   byte byteToRotate;
   MMU mmu = GB_getMMU (cpu->gb);

   byteToRotate = MMU_readByte (mmu, REG_HL);
   CPU_8bitRL (cpu, &byteToRotate);
   MMU_writeByte (mmu, REG_HL, byteToRotate);

   REG_PC += 2;
   return 16;
}

int CPU_RRC_A (CPU cpu) {
   CPU_8bitRRC (cpu, &REG_A);
   REG_PC += 2;
   return 8;
}

int CPU_RRC_B (CPU cpu) {
   CPU_8bitRRC (cpu, &REG_B);
   REG_PC += 2;
   return 8;
}

int CPU_RRC_C (CPU cpu) {
   CPU_8bitRRC (cpu, &REG_C);
   REG_PC += 2;
   return 8;
}

int CPU_RRC_D (CPU cpu) {
   CPU_8bitRRC (cpu, &REG_D);
   REG_PC += 2;
   return 8;
}

int CPU_RRC_E (CPU cpu) {
   CPU_8bitRRC (cpu, &REG_E);
   REG_PC += 2;
   return 8;
}

int CPU_RRC_H (CPU cpu) {
   CPU_8bitRRC (cpu, &REG_H);
   REG_PC += 2;
   return 8;
}

int CPU_RRC_L (CPU cpu) {
   CPU_8bitRRC (cpu, &REG_L);
   REG_PC += 2;
   return 8;
}

int CPU_RRC_aHL (CPU cpu) {
   byte byteToRotate;
   MMU mmu = GB_getMMU (cpu->gb);

   byteToRotate = MMU_readByte (mmu, REG_HL);
   CPU_8bitRRC (cpu, &byteToRotate);
   MMU_writeByte (mmu, REG_HL, byteToRotate);

   REG_PC += 2;
   return 16;
}

int CPU_RR_A (CPU cpu) {
   CPU_8bitRR (cpu, &REG_A);
   REG_PC += 2;
   return 8;
}

int CPU_RR_B (CPU cpu) {
   CPU_8bitRR (cpu, &REG_B);
   REG_PC += 2;
   return 8;
}

int CPU_RR_C (CPU cpu) {
   CPU_8bitRR (cpu, &REG_C);
   REG_PC += 2;
   return 8;
}

int CPU_RR_D (CPU cpu) {
   CPU_8bitRR (cpu, &REG_D);
   REG_PC += 2;
   return 8;
}

int CPU_RR_E (CPU cpu) {
   CPU_8bitRR (cpu, &REG_E);
   REG_PC += 2;
   return 8;
}

int CPU_RR_H (CPU cpu) {
   CPU_8bitRR (cpu, &REG_H);
   REG_PC += 2;
   return 8;
}

int CPU_RR_L (CPU cpu) {
   CPU_8bitRR (cpu, &REG_L);
   REG_PC += 2;
   return 8;
}

int CPU_RR_aHL (CPU cpu) {
   byte byteToRotate;
   MMU mmu = GB_getMMU (cpu->gb);

   byteToRotate = MMU_readByte (mmu, REG_HL);
   CPU_8bitRR (cpu, &byteToRotate);
   MMU_writeByte (mmu, REG_HL, byteToRotate);

   REG_PC += 2;
   return 16;
}

int CPU_SLA_A (CPU cpu) {
   CPU_8bitSLA (cpu, &REG_A);
   REG_PC += 2;
   return 8;
}

int CPU_SLA_B (CPU cpu) {
   CPU_8bitSLA (cpu, &REG_B);
   REG_PC += 2;
   return 8;
}

int CPU_SLA_C (CPU cpu) {
   CPU_8bitSLA (cpu, &REG_C);
   REG_PC += 2;
   return 8;
}

int CPU_SLA_D (CPU cpu) {
   CPU_8bitSLA (cpu, &REG_D);
   REG_PC += 2;
   return 8;
}

int CPU_SLA_E (CPU cpu) {
   CPU_8bitSLA (cpu, &REG_E);
   REG_PC += 2;
   return 8;
}

int CPU_SLA_H (CPU cpu) {
   CPU_8bitSLA (cpu, &REG_H);
   REG_PC += 2;
   return 8;
}

int CPU_SLA_L (CPU cpu) {
   CPU_8bitSLA (cpu, &REG_L);
   REG_PC += 2;
   return 8;
}

int CPU_SLA_aHL (CPU cpu) {
   byte byteToShift;
   MMU mmu = GB_getMMU (cpu->gb);

   byteToShift = MMU_readByte (mmu, REG_HL);
   CPU_8bitSLA (cpu, &byteToShift);
   MMU_writeByte (mmu, REG_HL, byteToShift);

   REG_PC += 2;
   return 16;
}

int CPU_SRA_A (CPU cpu) {
   CPU_8bitSRA (cpu, &REG_A);
   REG_PC += 2;
   return 8;
}

int CPU_SRA_B (CPU cpu) {
   CPU_8bitSRA (cpu, &REG_B);
   REG_PC += 2;
   return 8;
}

int CPU_SRA_C (CPU cpu) {
   CPU_8bitSRA (cpu, &REG_C);
   REG_PC += 2;
   return 8;
}

int CPU_SRA_D (CPU cpu) {
   CPU_8bitSRA (cpu, &REG_D);
   REG_PC += 2;
   return 8;
}

int CPU_SRA_E (CPU cpu) {
   CPU_8bitSRA (cpu, &REG_E);
   REG_PC += 2;
   return 8;
}

int CPU_SRA_H (CPU cpu) {
   CPU_8bitSRA (cpu, &REG_H);
   REG_PC += 2;
   return 8;
}

int CPU_SRA_L (CPU cpu) {
   CPU_8bitSRA (cpu, &REG_L);
   REG_PC += 2;
   return 8;
}

int CPU_SRA_aHL (CPU cpu) {
   byte byteToShift;
   MMU mmu = GB_getMMU (cpu->gb);

   byteToShift = MMU_readByte (mmu, REG_HL);
   CPU_8bitSRA (cpu, &byteToShift);
   MMU_writeByte (mmu, REG_HL, byteToShift);

   REG_PC += 2;
   return 16;
}

int CPU_SRL_A (CPU cpu) {
   CPU_8bitSRL (cpu, &REG_A);
   REG_PC += 2;
   return 8;
}

int CPU_SRL_B (CPU cpu) {
   CPU_8bitSRL (cpu, &REG_B);
   REG_PC += 2;
   return 8;
}

int CPU_SRL_C (CPU cpu) {
   CPU_8bitSRL (cpu, &REG_C);
   REG_PC += 2;
   return 8;
}

int CPU_SRL_D (CPU cpu) {
   CPU_8bitSRL (cpu, &REG_D);
   REG_PC += 2;
   return 8;
}

int CPU_SRL_E (CPU cpu) {
   CPU_8bitSRL (cpu, &REG_E);
   REG_PC += 2;
   return 8;
}

int CPU_SRL_H (CPU cpu) {
   CPU_8bitSRL (cpu, &REG_H);
   REG_PC += 2;
   return 8;
}

int CPU_SRL_L (CPU cpu) {
   CPU_8bitSRL (cpu, &REG_L);
   REG_PC += 2;
   return 8;
}

int CPU_SRL_aHL (CPU cpu) {
   byte byteToShift;
   MMU mmu = GB_getMMU (cpu->gb);

   byteToShift = MMU_readByte (mmu, REG_HL);
   CPU_8bitSRL (cpu, &byteToShift);
   MMU_writeByte (mmu, REG_HL, byteToShift);

   REG_PC += 2;
   return 16;
}

int CPU_BIT_0_A (CPU cpu) {CPU_8bitBIT(cpu, &REG_A, 0); REG_PC += 2; return 8;}
int CPU_BIT_0_B (CPU cpu) {CPU_8bitBIT(cpu, &REG_B, 0); REG_PC += 2; return 8;}
int CPU_BIT_0_C (CPU cpu) {CPU_8bitBIT(cpu, &REG_C, 0); REG_PC += 2; return 8;}
int CPU_BIT_0_D (CPU cpu) {CPU_8bitBIT(cpu, &REG_D, 0); REG_PC += 2; return 8;}
int CPU_BIT_0_E (CPU cpu) {CPU_8bitBIT(cpu, &REG_E, 0); REG_PC += 2; return 8;}
int CPU_BIT_0_H (CPU cpu) {CPU_8bitBIT(cpu, &REG_H, 0); REG_PC += 2; return 8;}
int CPU_BIT_0_L (CPU cpu) {CPU_8bitBIT(cpu, &REG_L, 0); REG_PC += 2; return 8;}

int CPU_BIT_0_aHL (CPU cpu) {
   byte byteToTest;
   MMU mmu = GB_getMMU (cpu->gb);
   byteToTest = MMU_readByte (mmu, REG_HL);
   CPU_8bitBIT(cpu, &byteToTest, 0);
   REG_PC += 2; 
   return 16;
}
int CPU_BIT_1_A (CPU cpu) {CPU_8bitBIT(cpu, &REG_A, 1); REG_PC += 2; return 8;}
int CPU_BIT_1_B (CPU cpu) {CPU_8bitBIT(cpu, &REG_B, 1); REG_PC += 2; return 8;}
int CPU_BIT_1_C (CPU cpu) {CPU_8bitBIT(cpu, &REG_C, 1); REG_PC += 2; return 8;}
int CPU_BIT_1_D (CPU cpu) {CPU_8bitBIT(cpu, &REG_D, 1); REG_PC += 2; return 8;}
int CPU_BIT_1_E (CPU cpu) {CPU_8bitBIT(cpu, &REG_E, 1); REG_PC += 2; return 8;}
int CPU_BIT_1_H (CPU cpu) {CPU_8bitBIT(cpu, &REG_H, 1); REG_PC += 2; return 8;}
int CPU_BIT_1_L (CPU cpu) {CPU_8bitBIT(cpu, &REG_L, 1); REG_PC += 2; return 8;}

int CPU_BIT_1_aHL (CPU cpu) {
   byte byteToTest;
   MMU mmu = GB_getMMU (cpu->gb);
   byteToTest = MMU_readByte (mmu, REG_HL);
   CPU_8bitBIT(cpu, &byteToTest, 1);
   REG_PC += 2; 
   return 16;
}
int CPU_BIT_2_A (CPU cpu) {CPU_8bitBIT(cpu, &REG_A, 2); REG_PC += 2; return 8;}
int CPU_BIT_2_B (CPU cpu) {CPU_8bitBIT(cpu, &REG_B, 2); REG_PC += 2; return 8;}
int CPU_BIT_2_C (CPU cpu) {CPU_8bitBIT(cpu, &REG_C, 2); REG_PC += 2; return 8;}
int CPU_BIT_2_D (CPU cpu) {CPU_8bitBIT(cpu, &REG_D, 2); REG_PC += 2; return 8;}
int CPU_BIT_2_E (CPU cpu) {CPU_8bitBIT(cpu, &REG_E, 2); REG_PC += 2; return 8;}
int CPU_BIT_2_H (CPU cpu) {CPU_8bitBIT(cpu, &REG_H, 2); REG_PC += 2; return 8;}
int CPU_BIT_2_L (CPU cpu) {CPU_8bitBIT(cpu, &REG_L, 2); REG_PC += 2; return 8;}

int CPU_BIT_2_aHL (CPU cpu) {
   byte byteToTest;
   MMU mmu = GB_getMMU (cpu->gb);
   byteToTest = MMU_readByte (mmu, REG_HL);
   CPU_8bitBIT(cpu, &byteToTest, 2);
   REG_PC += 2; 
   return 16;
}
int CPU_BIT_3_A (CPU cpu) {CPU_8bitBIT(cpu, &REG_A, 3); REG_PC += 2; return 8;}
int CPU_BIT_3_B (CPU cpu) {CPU_8bitBIT(cpu, &REG_B, 3); REG_PC += 2; return 8;}
int CPU_BIT_3_C (CPU cpu) {CPU_8bitBIT(cpu, &REG_C, 3); REG_PC += 2; return 8;}
int CPU_BIT_3_D (CPU cpu) {CPU_8bitBIT(cpu, &REG_D, 3); REG_PC += 2; return 8;}
int CPU_BIT_3_E (CPU cpu) {CPU_8bitBIT(cpu, &REG_E, 3); REG_PC += 2; return 8;}
int CPU_BIT_3_H (CPU cpu) {CPU_8bitBIT(cpu, &REG_H, 3); REG_PC += 2; return 8;}
int CPU_BIT_3_L (CPU cpu) {CPU_8bitBIT(cpu, &REG_L, 3); REG_PC += 2; return 8;}

int CPU_BIT_3_aHL (CPU cpu) {
   byte byteToTest;
   MMU mmu = GB_getMMU (cpu->gb);
   byteToTest = MMU_readByte (mmu, REG_HL);
   CPU_8bitBIT(cpu, &byteToTest, 3);
   REG_PC += 2; 
   return 16;
}
int CPU_BIT_4_A (CPU cpu) {CPU_8bitBIT(cpu, &REG_A, 4); REG_PC += 2; return 8;}
int CPU_BIT_4_B (CPU cpu) {CPU_8bitBIT(cpu, &REG_B, 4); REG_PC += 2; return 8;}
int CPU_BIT_4_C (CPU cpu) {CPU_8bitBIT(cpu, &REG_C, 4); REG_PC += 2; return 8;}
int CPU_BIT_4_D (CPU cpu) {CPU_8bitBIT(cpu, &REG_D, 4); REG_PC += 2; return 8;}
int CPU_BIT_4_E (CPU cpu) {CPU_8bitBIT(cpu, &REG_E, 4); REG_PC += 2; return 8;}
int CPU_BIT_4_H (CPU cpu) {CPU_8bitBIT(cpu, &REG_H, 4); REG_PC += 2; return 8;}
int CPU_BIT_4_L (CPU cpu) {CPU_8bitBIT(cpu, &REG_L, 4); REG_PC += 2; return 8;}

int CPU_BIT_4_aHL (CPU cpu) {
   byte byteToTest;
   MMU mmu = GB_getMMU (cpu->gb);
   byteToTest = MMU_readByte (mmu, REG_HL);
   CPU_8bitBIT(cpu, &byteToTest, 4);
   REG_PC += 2; 
   return 16;
}
int CPU_BIT_5_A (CPU cpu) {CPU_8bitBIT(cpu, &REG_A, 5); REG_PC += 2; return 8;}
int CPU_BIT_5_B (CPU cpu) {CPU_8bitBIT(cpu, &REG_B, 5); REG_PC += 2; return 8;}
int CPU_BIT_5_C (CPU cpu) {CPU_8bitBIT(cpu, &REG_C, 5); REG_PC += 2; return 8;}
int CPU_BIT_5_D (CPU cpu) {CPU_8bitBIT(cpu, &REG_D, 5); REG_PC += 2; return 8;}
int CPU_BIT_5_E (CPU cpu) {CPU_8bitBIT(cpu, &REG_E, 5); REG_PC += 2; return 8;}
int CPU_BIT_5_H (CPU cpu) {CPU_8bitBIT(cpu, &REG_H, 5); REG_PC += 2; return 8;}
int CPU_BIT_5_L (CPU cpu) {CPU_8bitBIT(cpu, &REG_L, 5); REG_PC += 2; return 8;}

int CPU_BIT_5_aHL (CPU cpu) {
   byte byteToTest;
   MMU mmu = GB_getMMU (cpu->gb);
   byteToTest = MMU_readByte (mmu, REG_HL);
   CPU_8bitBIT(cpu, &byteToTest, 5);
   REG_PC += 2; 
   return 16;
}
int CPU_BIT_6_A (CPU cpu) {CPU_8bitBIT(cpu, &REG_A, 6); REG_PC += 2; return 8;}
int CPU_BIT_6_B (CPU cpu) {CPU_8bitBIT(cpu, &REG_B, 6); REG_PC += 2; return 8;}
int CPU_BIT_6_C (CPU cpu) {CPU_8bitBIT(cpu, &REG_C, 6); REG_PC += 2; return 8;}
int CPU_BIT_6_D (CPU cpu) {CPU_8bitBIT(cpu, &REG_D, 6); REG_PC += 2; return 8;}
int CPU_BIT_6_E (CPU cpu) {CPU_8bitBIT(cpu, &REG_E, 6); REG_PC += 2; return 8;}
int CPU_BIT_6_H (CPU cpu) {CPU_8bitBIT(cpu, &REG_H, 6); REG_PC += 2; return 8;}
int CPU_BIT_6_L (CPU cpu) {CPU_8bitBIT(cpu, &REG_L, 6); REG_PC += 2; return 8;}

int CPU_BIT_6_aHL (CPU cpu) {
   byte byteToTest;
   MMU mmu = GB_getMMU (cpu->gb);
   byteToTest = MMU_readByte (mmu, REG_HL);
   CPU_8bitBIT(cpu, &byteToTest, 6);
   REG_PC += 2; 
   return 16;
}
int CPU_BIT_7_A (CPU cpu) {CPU_8bitBIT(cpu, &REG_A, 7); REG_PC += 2; return 8;}
int CPU_BIT_7_B (CPU cpu) {CPU_8bitBIT(cpu, &REG_B, 7); REG_PC += 2; return 8;}
int CPU_BIT_7_C (CPU cpu) {CPU_8bitBIT(cpu, &REG_C, 7); REG_PC += 2; return 8;}
int CPU_BIT_7_D (CPU cpu) {CPU_8bitBIT(cpu, &REG_D, 7); REG_PC += 2; return 8;}
int CPU_BIT_7_E (CPU cpu) {CPU_8bitBIT(cpu, &REG_E, 7); REG_PC += 2; return 8;}
int CPU_BIT_7_H (CPU cpu) {CPU_8bitBIT(cpu, &REG_H, 7); REG_PC += 2; return 8;}
int CPU_BIT_7_L (CPU cpu) {CPU_8bitBIT(cpu, &REG_L, 7); REG_PC += 2; return 8;}

int CPU_BIT_7_aHL (CPU cpu) {
   byte byteToTest;
   MMU mmu = GB_getMMU (cpu->gb);
   byteToTest = MMU_readByte (mmu, REG_HL);
   CPU_8bitBIT(cpu, &byteToTest, 7);
   REG_PC += 2; 
   return 16;
}

int CPU_RES_0_A (CPU cpu) {CPU_8bitRES(cpu, &REG_A, 0); REG_PC += 2; return 8;}
int CPU_RES_0_B (CPU cpu) {CPU_8bitRES(cpu, &REG_B, 0); REG_PC += 2; return 8;}
int CPU_RES_0_C (CPU cpu) {CPU_8bitRES(cpu, &REG_C, 0); REG_PC += 2; return 8;}
int CPU_RES_0_D (CPU cpu) {CPU_8bitRES(cpu, &REG_D, 0); REG_PC += 2; return 8;}
int CPU_RES_0_E (CPU cpu) {CPU_8bitRES(cpu, &REG_E, 0); REG_PC += 2; return 8;}
int CPU_RES_0_H (CPU cpu) {CPU_8bitRES(cpu, &REG_H, 0); REG_PC += 2; return 8;}
int CPU_RES_0_L (CPU cpu) {CPU_8bitRES(cpu, &REG_L, 0); REG_PC += 2; return 8;}

int CPU_RES_0_aHL (CPU cpu) {
   byte byteToTest;
   MMU mmu = GB_getMMU (cpu->gb);
   byteToTest = MMU_readByte (mmu, REG_HL);
   CPU_8bitRES(cpu, &byteToTest, 0);
   MMU_writeByte (mmu, REG_HL, byteToTest);
   REG_PC += 2; 
   return 16;
}
int CPU_RES_1_A (CPU cpu) {CPU_8bitRES(cpu, &REG_A, 1); REG_PC += 2; return 8;}
int CPU_RES_1_B (CPU cpu) {CPU_8bitRES(cpu, &REG_B, 1); REG_PC += 2; return 8;}
int CPU_RES_1_C (CPU cpu) {CPU_8bitRES(cpu, &REG_C, 1); REG_PC += 2; return 8;}
int CPU_RES_1_D (CPU cpu) {CPU_8bitRES(cpu, &REG_D, 1); REG_PC += 2; return 8;}
int CPU_RES_1_E (CPU cpu) {CPU_8bitRES(cpu, &REG_E, 1); REG_PC += 2; return 8;}
int CPU_RES_1_H (CPU cpu) {CPU_8bitRES(cpu, &REG_H, 1); REG_PC += 2; return 8;}
int CPU_RES_1_L (CPU cpu) {CPU_8bitRES(cpu, &REG_L, 1); REG_PC += 2; return 8;}

int CPU_RES_1_aHL (CPU cpu) {
   byte byteToTest;
   MMU mmu = GB_getMMU (cpu->gb);
   byteToTest = MMU_readByte (mmu, REG_HL);
   CPU_8bitRES(cpu, &byteToTest, 1);
   MMU_writeByte (mmu, REG_HL, byteToTest);
   REG_PC += 2; 
   return 16;
}
int CPU_RES_2_A (CPU cpu) {CPU_8bitRES(cpu, &REG_A, 2); REG_PC += 2; return 8;}
int CPU_RES_2_B (CPU cpu) {CPU_8bitRES(cpu, &REG_B, 2); REG_PC += 2; return 8;}
int CPU_RES_2_C (CPU cpu) {CPU_8bitRES(cpu, &REG_C, 2); REG_PC += 2; return 8;}
int CPU_RES_2_D (CPU cpu) {CPU_8bitRES(cpu, &REG_D, 2); REG_PC += 2; return 8;}
int CPU_RES_2_E (CPU cpu) {CPU_8bitRES(cpu, &REG_E, 2); REG_PC += 2; return 8;}
int CPU_RES_2_H (CPU cpu) {CPU_8bitRES(cpu, &REG_H, 2); REG_PC += 2; return 8;}
int CPU_RES_2_L (CPU cpu) {CPU_8bitRES(cpu, &REG_L, 2); REG_PC += 2; return 8;}

int CPU_RES_2_aHL (CPU cpu) {
   byte byteToTest;
   MMU mmu = GB_getMMU (cpu->gb);
   byteToTest = MMU_readByte (mmu, REG_HL);
   CPU_8bitRES(cpu, &byteToTest, 2);
   MMU_writeByte (mmu, REG_HL, byteToTest);
   REG_PC += 2; 
   return 16;
}
int CPU_RES_3_A (CPU cpu) {CPU_8bitRES(cpu, &REG_A, 3); REG_PC += 2; return 8;}
int CPU_RES_3_B (CPU cpu) {CPU_8bitRES(cpu, &REG_B, 3); REG_PC += 2; return 8;}
int CPU_RES_3_C (CPU cpu) {CPU_8bitRES(cpu, &REG_C, 3); REG_PC += 2; return 8;}
int CPU_RES_3_D (CPU cpu) {CPU_8bitRES(cpu, &REG_D, 3); REG_PC += 2; return 8;}
int CPU_RES_3_E (CPU cpu) {CPU_8bitRES(cpu, &REG_E, 3); REG_PC += 2; return 8;}
int CPU_RES_3_H (CPU cpu) {CPU_8bitRES(cpu, &REG_H, 3); REG_PC += 2; return 8;}
int CPU_RES_3_L (CPU cpu) {CPU_8bitRES(cpu, &REG_L, 3); REG_PC += 2; return 8;}

int CPU_RES_3_aHL (CPU cpu) {
   byte byteToTest;
   MMU mmu = GB_getMMU (cpu->gb);
   byteToTest = MMU_readByte (mmu, REG_HL);
   CPU_8bitRES(cpu, &byteToTest, 3);
   MMU_writeByte (mmu, REG_HL, byteToTest);
   REG_PC += 2; 
   return 16;
}
int CPU_RES_4_A (CPU cpu) {CPU_8bitRES(cpu, &REG_A, 4); REG_PC += 2; return 8;}
int CPU_RES_4_B (CPU cpu) {CPU_8bitRES(cpu, &REG_B, 4); REG_PC += 2; return 8;}
int CPU_RES_4_C (CPU cpu) {CPU_8bitRES(cpu, &REG_C, 4); REG_PC += 2; return 8;}
int CPU_RES_4_D (CPU cpu) {CPU_8bitRES(cpu, &REG_D, 4); REG_PC += 2; return 8;}
int CPU_RES_4_E (CPU cpu) {CPU_8bitRES(cpu, &REG_E, 4); REG_PC += 2; return 8;}
int CPU_RES_4_H (CPU cpu) {CPU_8bitRES(cpu, &REG_H, 4); REG_PC += 2; return 8;}
int CPU_RES_4_L (CPU cpu) {CPU_8bitRES(cpu, &REG_L, 4); REG_PC += 2; return 8;}

int CPU_RES_4_aHL (CPU cpu) {
   byte byteToTest;
   MMU mmu = GB_getMMU (cpu->gb);
   byteToTest = MMU_readByte (mmu, REG_HL);
   CPU_8bitRES(cpu, &byteToTest, 4);
   MMU_writeByte (mmu, REG_HL, byteToTest);
   REG_PC += 2; 
   return 16;
}
int CPU_RES_5_A (CPU cpu) {CPU_8bitRES(cpu, &REG_A, 5); REG_PC += 2; return 8;}
int CPU_RES_5_B (CPU cpu) {CPU_8bitRES(cpu, &REG_B, 5); REG_PC += 2; return 8;}
int CPU_RES_5_C (CPU cpu) {CPU_8bitRES(cpu, &REG_C, 5); REG_PC += 2; return 8;}
int CPU_RES_5_D (CPU cpu) {CPU_8bitRES(cpu, &REG_D, 5); REG_PC += 2; return 8;}
int CPU_RES_5_E (CPU cpu) {CPU_8bitRES(cpu, &REG_E, 5); REG_PC += 2; return 8;}
int CPU_RES_5_H (CPU cpu) {CPU_8bitRES(cpu, &REG_H, 5); REG_PC += 2; return 8;}
int CPU_RES_5_L (CPU cpu) {CPU_8bitRES(cpu, &REG_L, 5); REG_PC += 2; return 8;}

int CPU_RES_5_aHL (CPU cpu) {
   byte byteToTest;
   MMU mmu = GB_getMMU (cpu->gb);
   byteToTest = MMU_readByte (mmu, REG_HL);
   CPU_8bitRES(cpu, &byteToTest, 5);
   MMU_writeByte (mmu, REG_HL, byteToTest);
   REG_PC += 2; 
   return 16;
}
int CPU_RES_6_A (CPU cpu) {CPU_8bitRES(cpu, &REG_A, 6); REG_PC += 2; return 8;}
int CPU_RES_6_B (CPU cpu) {CPU_8bitRES(cpu, &REG_B, 6); REG_PC += 2; return 8;}
int CPU_RES_6_C (CPU cpu) {CPU_8bitRES(cpu, &REG_C, 6); REG_PC += 2; return 8;}
int CPU_RES_6_D (CPU cpu) {CPU_8bitRES(cpu, &REG_D, 6); REG_PC += 2; return 8;}
int CPU_RES_6_E (CPU cpu) {CPU_8bitRES(cpu, &REG_E, 6); REG_PC += 2; return 8;}
int CPU_RES_6_H (CPU cpu) {CPU_8bitRES(cpu, &REG_H, 6); REG_PC += 2; return 8;}
int CPU_RES_6_L (CPU cpu) {CPU_8bitRES(cpu, &REG_L, 6); REG_PC += 2; return 8;}

int CPU_RES_6_aHL (CPU cpu) {
   byte byteToTest;
   MMU mmu = GB_getMMU (cpu->gb);
   byteToTest = MMU_readByte (mmu, REG_HL);
   CPU_8bitRES(cpu, &byteToTest, 6);
   MMU_writeByte (mmu, REG_HL, byteToTest);
   REG_PC += 2; 
   return 16;
}
int CPU_RES_7_A (CPU cpu) {CPU_8bitRES(cpu, &REG_A, 7); REG_PC += 2; return 8;}
int CPU_RES_7_B (CPU cpu) {CPU_8bitRES(cpu, &REG_B, 7); REG_PC += 2; return 8;}
int CPU_RES_7_C (CPU cpu) {CPU_8bitRES(cpu, &REG_C, 7); REG_PC += 2; return 8;}
int CPU_RES_7_D (CPU cpu) {CPU_8bitRES(cpu, &REG_D, 7); REG_PC += 2; return 8;}
int CPU_RES_7_E (CPU cpu) {CPU_8bitRES(cpu, &REG_E, 7); REG_PC += 2; return 8;}
int CPU_RES_7_H (CPU cpu) {CPU_8bitRES(cpu, &REG_H, 7); REG_PC += 2; return 8;}
int CPU_RES_7_L (CPU cpu) {CPU_8bitRES(cpu, &REG_L, 7); REG_PC += 2; return 8;}

int CPU_RES_7_aHL (CPU cpu) {
   byte byteToTest;
   MMU mmu = GB_getMMU (cpu->gb);
   byteToTest = MMU_readByte (mmu, REG_HL);
   CPU_8bitRES(cpu, &byteToTest, 7);
   MMU_writeByte (mmu, REG_HL, byteToTest);
   REG_PC += 2; 
   return 16;
}

int CPU_SET_0_A (CPU cpu) {CPU_8bitSET(cpu, &REG_A, 0); REG_PC += 2; return 8;}
int CPU_SET_0_B (CPU cpu) {CPU_8bitSET(cpu, &REG_B, 0); REG_PC += 2; return 8;}
int CPU_SET_0_C (CPU cpu) {CPU_8bitSET(cpu, &REG_C, 0); REG_PC += 2; return 8;}
int CPU_SET_0_D (CPU cpu) {CPU_8bitSET(cpu, &REG_D, 0); REG_PC += 2; return 8;}
int CPU_SET_0_E (CPU cpu) {CPU_8bitSET(cpu, &REG_E, 0); REG_PC += 2; return 8;}
int CPU_SET_0_H (CPU cpu) {CPU_8bitSET(cpu, &REG_H, 0); REG_PC += 2; return 8;}
int CPU_SET_0_L (CPU cpu) {CPU_8bitSET(cpu, &REG_L, 0); REG_PC += 2; return 8;}

int CPU_SET_0_aHL (CPU cpu) {
   byte byteToTest;
   MMU mmu = GB_getMMU (cpu->gb);
   byteToTest = MMU_readByte (mmu, REG_HL);
   CPU_8bitSET(cpu, &byteToTest, 0);
   MMU_writeByte (mmu, REG_HL, byteToTest);
   REG_PC += 2; 
   return 16;
}
int CPU_SET_1_A (CPU cpu) {CPU_8bitSET(cpu, &REG_A, 1); REG_PC += 2; return 8;}
int CPU_SET_1_B (CPU cpu) {CPU_8bitSET(cpu, &REG_B, 1); REG_PC += 2; return 8;}
int CPU_SET_1_C (CPU cpu) {CPU_8bitSET(cpu, &REG_C, 1); REG_PC += 2; return 8;}
int CPU_SET_1_D (CPU cpu) {CPU_8bitSET(cpu, &REG_D, 1); REG_PC += 2; return 8;}
int CPU_SET_1_E (CPU cpu) {CPU_8bitSET(cpu, &REG_E, 1); REG_PC += 2; return 8;}
int CPU_SET_1_H (CPU cpu) {CPU_8bitSET(cpu, &REG_H, 1); REG_PC += 2; return 8;}
int CPU_SET_1_L (CPU cpu) {CPU_8bitSET(cpu, &REG_L, 1); REG_PC += 2; return 8;}

int CPU_SET_1_aHL (CPU cpu) {
   byte byteToTest;
   MMU mmu = GB_getMMU (cpu->gb);
   byteToTest = MMU_readByte (mmu, REG_HL);
   CPU_8bitSET(cpu, &byteToTest, 1);
   MMU_writeByte (mmu, REG_HL, byteToTest);
   REG_PC += 2; 
   return 16;
}
int CPU_SET_2_A (CPU cpu) {CPU_8bitSET(cpu, &REG_A, 2); REG_PC += 2; return 8;}
int CPU_SET_2_B (CPU cpu) {CPU_8bitSET(cpu, &REG_B, 2); REG_PC += 2; return 8;}
int CPU_SET_2_C (CPU cpu) {CPU_8bitSET(cpu, &REG_C, 2); REG_PC += 2; return 8;}
int CPU_SET_2_D (CPU cpu) {CPU_8bitSET(cpu, &REG_D, 2); REG_PC += 2; return 8;}
int CPU_SET_2_E (CPU cpu) {CPU_8bitSET(cpu, &REG_E, 2); REG_PC += 2; return 8;}
int CPU_SET_2_H (CPU cpu) {CPU_8bitSET(cpu, &REG_H, 2); REG_PC += 2; return 8;}
int CPU_SET_2_L (CPU cpu) {CPU_8bitSET(cpu, &REG_L, 2); REG_PC += 2; return 8;}

int CPU_SET_2_aHL (CPU cpu) {
   byte byteToTest;
   MMU mmu = GB_getMMU (cpu->gb);
   byteToTest = MMU_readByte (mmu, REG_HL);
   CPU_8bitSET(cpu, &byteToTest, 2);
   MMU_writeByte (mmu, REG_HL, byteToTest);
   REG_PC += 2; 
   return 16;
}
int CPU_SET_3_A (CPU cpu) {CPU_8bitSET(cpu, &REG_A, 3); REG_PC += 2; return 8;}
int CPU_SET_3_B (CPU cpu) {CPU_8bitSET(cpu, &REG_B, 3); REG_PC += 2; return 8;}
int CPU_SET_3_C (CPU cpu) {CPU_8bitSET(cpu, &REG_C, 3); REG_PC += 2; return 8;}
int CPU_SET_3_D (CPU cpu) {CPU_8bitSET(cpu, &REG_D, 3); REG_PC += 2; return 8;}
int CPU_SET_3_E (CPU cpu) {CPU_8bitSET(cpu, &REG_E, 3); REG_PC += 2; return 8;}
int CPU_SET_3_H (CPU cpu) {CPU_8bitSET(cpu, &REG_H, 3); REG_PC += 2; return 8;}
int CPU_SET_3_L (CPU cpu) {CPU_8bitSET(cpu, &REG_L, 3); REG_PC += 2; return 8;}

int CPU_SET_3_aHL (CPU cpu) {
   byte byteToTest;
   MMU mmu = GB_getMMU (cpu->gb);
   byteToTest = MMU_readByte (mmu, REG_HL);
   CPU_8bitSET(cpu, &byteToTest, 3);
   MMU_writeByte (mmu, REG_HL, byteToTest);
   REG_PC += 2; 
   return 16;
}
int CPU_SET_4_A (CPU cpu) {CPU_8bitSET(cpu, &REG_A, 4); REG_PC += 2; return 8;}
int CPU_SET_4_B (CPU cpu) {CPU_8bitSET(cpu, &REG_B, 4); REG_PC += 2; return 8;}
int CPU_SET_4_C (CPU cpu) {CPU_8bitSET(cpu, &REG_C, 4); REG_PC += 2; return 8;}
int CPU_SET_4_D (CPU cpu) {CPU_8bitSET(cpu, &REG_D, 4); REG_PC += 2; return 8;}
int CPU_SET_4_E (CPU cpu) {CPU_8bitSET(cpu, &REG_E, 4); REG_PC += 2; return 8;}
int CPU_SET_4_H (CPU cpu) {CPU_8bitSET(cpu, &REG_H, 4); REG_PC += 2; return 8;}
int CPU_SET_4_L (CPU cpu) {CPU_8bitSET(cpu, &REG_L, 4); REG_PC += 2; return 8;}

int CPU_SET_4_aHL (CPU cpu) {
   byte byteToTest;
   MMU mmu = GB_getMMU (cpu->gb);
   byteToTest = MMU_readByte (mmu, REG_HL);
   CPU_8bitSET(cpu, &byteToTest, 4);
   MMU_writeByte (mmu, REG_HL, byteToTest);
   REG_PC += 2; 
   return 16;
}
int CPU_SET_5_A (CPU cpu) {CPU_8bitSET(cpu, &REG_A, 5); REG_PC += 2; return 8;}
int CPU_SET_5_B (CPU cpu) {CPU_8bitSET(cpu, &REG_B, 5); REG_PC += 2; return 8;}
int CPU_SET_5_C (CPU cpu) {CPU_8bitSET(cpu, &REG_C, 5); REG_PC += 2; return 8;}
int CPU_SET_5_D (CPU cpu) {CPU_8bitSET(cpu, &REG_D, 5); REG_PC += 2; return 8;}
int CPU_SET_5_E (CPU cpu) {CPU_8bitSET(cpu, &REG_E, 5); REG_PC += 2; return 8;}
int CPU_SET_5_H (CPU cpu) {CPU_8bitSET(cpu, &REG_H, 5); REG_PC += 2; return 8;}
int CPU_SET_5_L (CPU cpu) {CPU_8bitSET(cpu, &REG_L, 5); REG_PC += 2; return 8;}

int CPU_SET_5_aHL (CPU cpu) {
   byte byteToTest;
   MMU mmu = GB_getMMU (cpu->gb);
   byteToTest = MMU_readByte (mmu, REG_HL);
   CPU_8bitSET(cpu, &byteToTest, 5);
   MMU_writeByte (mmu, REG_HL, byteToTest);
   REG_PC += 2; 
   return 16;
}
int CPU_SET_6_A (CPU cpu) {CPU_8bitSET(cpu, &REG_A, 6); REG_PC += 2; return 8;}
int CPU_SET_6_B (CPU cpu) {CPU_8bitSET(cpu, &REG_B, 6); REG_PC += 2; return 8;}
int CPU_SET_6_C (CPU cpu) {CPU_8bitSET(cpu, &REG_C, 6); REG_PC += 2; return 8;}
int CPU_SET_6_D (CPU cpu) {CPU_8bitSET(cpu, &REG_D, 6); REG_PC += 2; return 8;}
int CPU_SET_6_E (CPU cpu) {CPU_8bitSET(cpu, &REG_E, 6); REG_PC += 2; return 8;}
int CPU_SET_6_H (CPU cpu) {CPU_8bitSET(cpu, &REG_H, 6); REG_PC += 2; return 8;}
int CPU_SET_6_L (CPU cpu) {CPU_8bitSET(cpu, &REG_L, 6); REG_PC += 2; return 8;}

int CPU_SET_6_aHL (CPU cpu) {
   byte byteToTest;
   MMU mmu = GB_getMMU (cpu->gb);
   byteToTest = MMU_readByte (mmu, REG_HL);
   CPU_8bitSET(cpu, &byteToTest, 6);
   MMU_writeByte (mmu, REG_HL, byteToTest);
   REG_PC += 2; 
   return 16;
}
int CPU_SET_7_A (CPU cpu) {CPU_8bitSET(cpu, &REG_A, 7); REG_PC += 2; return 8;}
int CPU_SET_7_B (CPU cpu) {CPU_8bitSET(cpu, &REG_B, 7); REG_PC += 2; return 8;}
int CPU_SET_7_C (CPU cpu) {CPU_8bitSET(cpu, &REG_C, 7); REG_PC += 2; return 8;}
int CPU_SET_7_D (CPU cpu) {CPU_8bitSET(cpu, &REG_D, 7); REG_PC += 2; return 8;}
int CPU_SET_7_E (CPU cpu) {CPU_8bitSET(cpu, &REG_E, 7); REG_PC += 2; return 8;}
int CPU_SET_7_H (CPU cpu) {CPU_8bitSET(cpu, &REG_H, 7); REG_PC += 2; return 8;}
int CPU_SET_7_L (CPU cpu) {CPU_8bitSET(cpu, &REG_L, 7); REG_PC += 2; return 8;}

int CPU_SET_7_aHL (CPU cpu) {
   byte byteToTest;
   MMU mmu = GB_getMMU (cpu->gb);
   byteToTest = MMU_readByte (mmu, REG_HL);
   CPU_8bitSET(cpu, &byteToTest, 7);
   MMU_writeByte (mmu, REG_HL, byteToTest);
   REG_PC += 2; 
   return 16;
}

