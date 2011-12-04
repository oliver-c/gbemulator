#include <stdlib.h>
#include <assert.h>

#include "GB.h"
#include "CPU.h"
#include "MMU.h"
#include "GPU.h"

struct GPU {
   GB gb;
   GPU_mode mode;
};

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
   int currentLine;
   static int scanlineCounter = 0;

   mmu = GB_getMMU (gpu->gb);
   memory = MMU_getMemory (mmu);

   scanlineCounter += cycles;
   currentLine = MMU_readByte (mmu, 0xFF44);

   while (scanlineCounter >= SCANLINE_CYCLES) {
      currentLine++;
      scanlineCounter -= SCANLINE_CYCLES;
   }

   if (currentLine >= NUM_VISIBLE_SCANLINES) {
      GB_requestInterrupt (gpu->gb, INT_VBLANK);
   }

   if (currentLine >= NUM_SCANLINES) {
      currentLine = 0;
   }

   memory[0xFF44] = currentLine;
}
