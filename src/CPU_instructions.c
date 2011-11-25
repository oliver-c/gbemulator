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

struct CPU {
   GB gb;

   bool IME;
   reg registers[NUM_REGISTERS];
};

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
