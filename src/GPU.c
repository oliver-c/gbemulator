#include <stdlib.h>
#include <assert.h>

#include "GB.h"
#include "CPU.h"
#include "MMU.h"
#include "GPU.h"

#include "bitOperations.h"

struct GPU {
   GB gb;
   GPU_mode mode;
};

void GPU_updateLCDStatus (GPU gpu);

GPU GPU_init (GB gb) {
   GPU newGPU = (GPU)malloc(sizeof(struct GPU));
   assert (newGPU != NULL);

   newGPU->gb = gb;

   return newGPU;
}

void GPU_free (GPU gpu) {
   free (gpu);
}

void GPU_update (GPU gpu, int cycles) {
   MMU mmu;
   byte *memory;
   byte currentLine;
   static int scanlineCounter = 0;

   mmu = GB_getMMU (gpu->gb);
   memory = MMU_getMemory (mmu);

   scanlineCounter += cycles;
   currentLine = MMU_readByte (mmu, 0xFF44);

   while (scanlineCounter >= SCANLINE_CYCLES) {
      currentLine++;
      scanlineCounter -= SCANLINE_CYCLES;
   }

   GPU_updateLCDStatus (gpu);

   if (currentLine >= NUM_VISIBLE_SCANLINES) {
      GB_requestInterrupt (gpu->gb, INT_VBLANK);
   }

   if (currentLine >= NUM_SCANLINES) {
      currentLine = 0;
   }

   memory[0xFF44] = currentLine;
}

void GPU_updateLCDStatus (GPU gpu) {
   MMU mmu;
   byte *memory;
   byte status;
   byte currentLine;
   /*byte currentMode, newMode;*/

   mmu = GB_getMMU (gpu->gb);
   status = MMU_readByte (mmu, 0xFF41);
   memory = MMU_getMemory (mmu);

   currentLine = MMU_readByte (mmu, 0xFF44);
   /*currentMode = status & 3;*/
   
   /* Update the LCD status register according to the current
      scanline */
   if (currentLine >= NUM_VISIBLE_SCANLINES) {
      /* V-Blank */    
     clearBit (&status, 1);
     clearBit (&status, 2);
     setBit (&status, 1);
   }

   /*newMode = status & 3;*/
   

   memory[0xFF41] = status;
}
