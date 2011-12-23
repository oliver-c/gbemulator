#include <stdlib.h>
#include <assert.h>

#include "GB.h"
#include "CPU.h"
#include "MMU.h"
#include "GPU.h"
#include "GUI.h"

#include "bitOperations.h"
#include "types.h"

struct GPU {
   GB gb;
   int scanlineCounter;
};

/* Updates the scanline */
void GPU_updateScanline (GPU gpu, int cycles);
void GPU_drawScanline (GPU gpu);
void GPU_drawBackground (GPU gpu); 

/* Updates the LCD status register, also requests interrupts
   if necessary */
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
      /* Draw the current scanline before moving on
         to the next */
      GPU_drawScanline (gpu);

      currentLine++;
      gpu->scanlineCounter -= SCANLINE_CYCLES;
   }

   if (currentLine >= NUM_SCANLINES) {
      currentLine = 0;
   }

   memory[0xFF44] = currentLine;
}

void GPU_drawScanline (GPU gpu) {
   GPU_drawBackground (gpu);
}

void GPU_drawBackground (GPU gpu) {
   GUI gui;
   MMU mmu;
   byte lcdControl;
   colour *pixels;
   colour sourceColour;
   int i, j;
   int currentLine;
   int verticalTileIndex;
   int scrollY;
   int tileIndexLocation;
   int tileIndex;
   int tileVerticalOffset;

   mmu = GB_getMMU (gpu->gb);
   gui = GB_getGUI (gpu->gb);

   lcdControl = MMU_readByte (mmu, 0xFF40);
   currentLine = MMU_readByte (mmu, 0xFF44);

   pixels = GUI_getFramebuffer (gui);

   if (testBit (lcdControl, 0)) {
      /* If background display is enabled */
      scrollY = MMU_readByte (mmu, 0xFF42);
      verticalTileIndex = (scrollY+currentLine)/BG_TILE_HEIGHT;

      assert (verticalTileIndex >= 0 && verticalTileIndex < BG_NUM_VERTICAL_TILES);

      for (i = 0; i < (WINDOW_WIDTH/BG_TILE_WIDTH); i++) {
         if (!testBit (lcdControl, 3)) {
            /* Map data from 9800-9BFF */
            tileIndexLocation = 0x9800+verticalTileIndex*BG_NUM_HORIZONTAL_TILES+i;
         } else {
            /* Map data from 9C00-9FFF */
            tileIndexLocation = 0x9C00+verticalTileIndex*BG_NUM_HORIZONTAL_TILES+i;
         }

         tileIndex = MMU_readByte (mmu, tileIndexLocation);
         tileVerticalOffset = currentLine % BG_TILE_HEIGHT;

         for (j = 0; j < BG_TILE_WIDTH; j++) {
            sourceColour.r = 255;
            sourceColour.g = 255;
            sourceColour.b = 255;
            pixels[currentLine*WINDOW_WIDTH + (BG_TILE_WIDTH*i + j)] = sourceColour;
         }
      }
   } else {
      /* Background is white */ 
      sourceColour.r = 255;
      sourceColour.g = 255;
      sourceColour.b = 255;
      
      for (i = 0; i < WINDOW_WIDTH; i++) {
         pixels[currentLine*WINDOW_WIDTH + i] = sourceColour;
      }
   }
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
