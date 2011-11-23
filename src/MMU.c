#include <stdlib.h>
#include <assert.h>

#include "GB.h"
#include "Cartridge.h"
#include "MMU.h"

#include "types.h"

struct MMU {
   GB gb;  

   /* Mapped memory */
   byte memory[MAPPED_MEM_SIZE];

   /* The current switchable ROM bank number */
   int currentROMBank;
};

MMU MMU_init (GB gb) {
   MMU newMMU = (MMU)malloc(sizeof(struct MMU));
   assert (newMMU != NULL);

   newMMU->gb = gb;
   newMMU->currentROMBank = 1;

   return newMMU;
}

void MMU_free (MMU mmu) {
   free (mmu);
}

byte MMU_readByte (MMU mmu, int location) {
   byte *bankData;
   byte value;
   Cartridge cartridge;
   
   if (location >= 0 && location <= 0x3FFF) {
      cartridge = GB_getCartridge (mmu->gb); 
      bankData = Cartridge_getData (cartridge, 0);

      value = bankData[location];

   } else if (location >= 0x4000 && location <= 0x7FFF) {
      cartridge = GB_getCartridge (mmu->gb); 
      bankData = Cartridge_getData (cartridge, mmu->currentROMBank);

      value = bankData[location-0x4000];
   } else {
      value = mmu->memory[location];
   }
   
   return value;
}

void MMU_writeByte (MMU mmu, int location, byte byteToWrite) {
   if (location >= 0xC000 && location <= 0xDE00) {
      /* Echo of RAM */
      mmu->memory[location] = byteToWrite;
      mmu->memory[location+0x2000] = byteToWrite;
   } else if (location >= 0xE000 && location <= 0xFE00) {
      /* Echo of RAM */
      mmu->memory[location] = byteToWrite;
      mmu->memory[location-0x2000] = byteToWrite;
   } else if (location >= 0x2000 && location <= 0x3FFF) {
      /* Change bank number */
   } else {
      mmu->memory[location] = byteToWrite;
   }
}
