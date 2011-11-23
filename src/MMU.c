#include <stdlib.h>
#include <assert.h>

#include "GB.h"
#include "MMU.h"

#include "types.h"

struct MMU {
   GB gb;  
   byte memory[MAPPED_MEM_SIZE];
};

MMU MMU_init (GB gb) {
   MMU newMMU = (MMU)malloc(sizeof(struct MMU));
   assert (newMMU != NULL);

   newMMU->gb = gb;

   return newMMU;
}

void MMU_free (MMU mmu) {
   free (mmu);
}

byte MMU_readByte (MMU mmu, int location) {
   byte value;
   
   value = mmu->memory[location];
   
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
   } else {
      mmu->memory[location] = byteToWrite;
   }
}
