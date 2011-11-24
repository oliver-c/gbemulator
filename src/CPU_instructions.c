#include "GB.h"
#include "MMU.h"
#include "CPU.h"
#include "CPU_instructions.h"

struct CPU {
   GB gb;

   bool IME;
   reg registers[NUM_REGISTERS];
};

int CPU_NOP (CPU cpu) {
   cpu->registers[PC].value++;
   return 4;
}

int CPU_LD_B_n (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   cpu->registers[BC].bytes.high = MMU_readByte (mmu, cpu->registers[PC].value + 1);
   cpu->registers[PC].value += 2;
   return 8;
}

int CPU_LD_C_n (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   cpu->registers[BC].bytes.low = MMU_readByte (mmu, cpu->registers[PC].value + 1);
   cpu->registers[PC].value += 2;
   return 8;
}

int CPU_LD_D_n (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   cpu->registers[DE].bytes.high = MMU_readByte (mmu, cpu->registers[PC].value + 1);
   cpu->registers[PC].value += 2;
   return 8;
}

int CPU_LD_E_n (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   cpu->registers[DE].bytes.low = MMU_readByte (mmu, cpu->registers[PC].value + 1);
   cpu->registers[PC].value += 2;
   return 8;
}

int CPU_LD_H_n (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   cpu->registers[HL].bytes.high = MMU_readByte (mmu, cpu->registers[PC].value + 1);
   cpu->registers[PC].value += 2;
   return 8;
}

int CPU_LD_L_n (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   cpu->registers[HL].bytes.low = MMU_readByte (mmu, cpu->registers[PC].value + 1);
   cpu->registers[PC].value += 2;
   return 8;
}

int CPU_LD_A_A (CPU cpu) {
   cpu->registers[PC].value++;
   return 4;
}

int CPU_LD_A_B (CPU cpu) {
   cpu->registers[AF].bytes.high = cpu->registers[BC].bytes.high;
   cpu->registers[PC].value++;
   return 4;
}

int CPU_LD_A_C (CPU cpu) {
   cpu->registers[AF].bytes.high = cpu->registers[BC].bytes.low;
   cpu->registers[PC].value++;
   return 4;
}

int CPU_LD_A_D (CPU cpu) {
   cpu->registers[AF].bytes.high = cpu->registers[DE].bytes.high;
   cpu->registers[PC].value++;
   return 4;
}

int CPU_LD_A_E (CPU cpu) {
   cpu->registers[AF].bytes.high = cpu->registers[DE].bytes.low;
   cpu->registers[PC].value++;
   return 4;
}

int CPU_LD_A_H (CPU cpu) {
   cpu->registers[AF].bytes.high = cpu->registers[HL].bytes.high;
   cpu->registers[PC].value++;
   return 4;
}

int CPU_LD_A_L (CPU cpu) {
   cpu->registers[AF].bytes.high = cpu->registers[HL].bytes.low;
   cpu->registers[PC].value++;
   return 4;
}

int CPU_LD_A_aHL (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   cpu->registers[AF].bytes.high = MMU_readByte (mmu, cpu->registers[HL].value);
   cpu->registers[PC].value++;
   return 8;
}

int CPU_LD_B_B (CPU cpu) {
   cpu->registers[PC].value++;
   return 4;
}

int CPU_LD_B_C (CPU cpu) {
   cpu->registers[BC].bytes.high = cpu->registers[BC].bytes.low;
   cpu->registers[PC].value++;
   return 4;
}

int CPU_LD_B_D (CPU cpu) {
   cpu->registers[BC].bytes.high = cpu->registers[DE].bytes.high;
   cpu->registers[PC].value++;
   return 4;
}

int CPU_LD_B_E (CPU cpu) {
   cpu->registers[BC].bytes.high = cpu->registers[DE].bytes.low;
   cpu->registers[PC].value++;
   return 4;

}

int CPU_LD_B_H (CPU cpu) {
   cpu->registers[BC].bytes.high = cpu->registers[HL].bytes.high;
   cpu->registers[PC].value++;
   return 4;
}

int CPU_LD_B_L (CPU cpu) {
   cpu->registers[BC].bytes.high = cpu->registers[HL].bytes.low;
   cpu->registers[PC].value++;
   return 4;
}

int CPU_LD_B_aHL (CPU cpu) {
   MMU mmu = GB_getMMU (cpu->gb);
   cpu->registers[BC].bytes.high = MMU_readByte (mmu, cpu->registers[HL].value);
   cpu->registers[PC].value++;
   return 4;
}

