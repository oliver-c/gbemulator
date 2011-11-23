#include <stdlib.h>
#include <assert.h>

#include "GB.h"
#include "MMU.h"

struct MMU {
   GB gb;  
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
   return 0;
}

void MMU_writeByte (MMU mmu, int location, byte byteToWrite) {

}
