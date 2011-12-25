#include <stdio.h>
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
   int i, address;

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
   } else if (location == 0xFF04) {
      /* Writing to the divider register, which resets it to zero */
      mmu->memory[location]= 0;
   } else if (location == 0xFF44) {
      /* Writing to the scanline register, which resets it to zero */
      mmu->memory[location] = 0;
   } else if (location == 0xFF06) {
      /* DMA transfer */
      address = byteToWrite * 0x100;      
      for (i = 0; i <= 0x9F; i++) {
         mmu->memory[0xFE00+i] = mmu->memory[address+i];
      }
   } else {
      mmu->memory[location] = byteToWrite;
   }
}

word MMU_readWord (MMU mmu, int location) {
   word value;
   value = (MMU_readByte(mmu, location)) | (MMU_readByte(mmu, location+1) << 8);

   return value;
}

void MMU_writeWord (MMU mmu, int location, word wordToWrite) {
   byte MSB, LSB; 
   
   LSB = (wordToWrite & 0xFF);
   wordToWrite >>= 8;
   MSB = (wordToWrite & 0xFF);

   MMU_writeByte (mmu, location, LSB);
   MMU_writeByte (mmu, location+1, MSB);
}

byte * MMU_getMemory (MMU mmu) {
   return mmu->memory;
}
