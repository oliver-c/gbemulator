#include <stdlib.h>
#include <assert.h>

#include "GB.h"
#include "CPU.h"
#include "MMU.h"
#include "GPU.h"

#include "bitOperations.h"
#include "types.h"

struct GPU {
   GB gb;
   int scanlineCounter;
};

/* Updates the LCD status register, also requests interrupts
   if necessary */
void GPU_updateScanline (GPU gpu, int cycles);
void GPU_updateLCDStatus (GPU gpu);

GPU GPU_init (GB gb) {
   GPU newGPU = (GPU)malloc(sizeof(struct GPU));
   assert (newGPU != NULL);

   newGPU->gb = gb;
   newGPU->scanlineCounter = 0;

   return newGPU;
}

void GPU_free (GPU gpu) {
   free (gpu);
}

void GPU_update (GPU gpu, int cycles) {
   GPU_updateScanline (gpu, cycles);
   GPU_updateLCDStatus (gpu);
}

void GPU_updateScanline (GPU gpu, int cycles) {
   MMU mmu;
   byte *memory;
   byte currentLine;

   mmu = GB_getMMU (gpu->gb);
   memory = MMU_getMemory (mmu);

   gpu->scanlineCounter += cycles;
   currentLine = MMU_readByte (mmu, 0xFF44);

   if (gpu->scanlineCounter >= SCANLINE_CYCLES) {
      currentLine++;
      gpu->scanlineCounter -= SCANLINE_CYCLES;
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
   byte lastMode, newMode;
   bool requestInterrupt = FALSE;

   mmu = GB_getMMU (gpu->gb);
   status = MMU_readByte (mmu, 0xFF41);
   memory = MMU_getMemory (mmu);

   currentLine = MMU_readByte (mmu, 0xFF44);
   lastMode = status & 3;
   
   /* Update the LCD status register according to the current
      scanline and scanline cycle counter */
   if (currentLine < NUM_VISIBLE_SCANLINES) {
      if (gpu->scanlineCounter <= 80) {
         /* Mode 2 */   
         clearBit (&status, 0); 
         setBit (&status, 1); 
         requestInterrupt = testBit (status, 5);
      } else if (gpu->scanlineCounter <= 172) {
         /* Mode 3 */    
         setBit (&status, 0);
         setBit (&status, 1);
      } else {
         /* Mode 0, H-Blank */
         clearBit (&status, 0);
         clearBit (&status, 1);
         requestInterrupt = testBit (status, 3);
      }
   } else {
      /* Mode 1, V-Blank */    
      clearBit (&status, 1);
      setBit (&status, 0);
      requestInterrupt = testBit (status, 4);
      GB_requestInterrupt (gpu->gb, INT_VBLANK);
   }

   newMode = status & 3;

   if (newMode != lastMode && requestInterrupt) {
      /* Mode changed */
      GB_requestInterrupt (gpu->gb, INT_LCDSTAT);
   }

   /* Update coincidence flag */
   if (MMU_readByte (mmu, 0xFF44) == MMU_readByte (mmu, 0xFF45)) {
      setBit (&status, 2);
      if (testBit (status, 6)) {
         GB_requestInterrupt (gpu->gb, INT_LCDSTAT);
      }
   } else {
      clearBit (&status, 2);
   }

   memory[0xFF41] = status;
}
