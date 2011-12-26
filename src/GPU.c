#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "GB.h"
#include "CPU.h"
#include "MMU.h"
#include "GPU.h"
#include "GUI.h"

#include "bitOperations.h"
#include "types.h"

typedef enum palette {
   PALETTE_BG,
   PALETTE_OBJ0, 
   PALETTE_OBJ1
} palette;

typedef struct spriteAttribute {
   byte yPos;
   byte xPos;
   byte tileNumber;
   byte flags; 
} spriteAttribute;

struct GPU {
   GB gb;
   int scanlineCounter;
   Uint8 bgPalette[NUM_COLOURS];
   Uint8 objPalette0[NUM_COLOURS];
   Uint8 objPalette1[NUM_COLOURS];
};

/* Updates the scanline */
void GPU_updateScanline (GPU gpu, int cycles);
void GPU_drawScanline (GPU gpu);
void GPU_drawBackground (GPU gpu); 
void GPU_drawSprites (GPU gpu);

void GPU_getPalette (GPU gpu, int type);

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
   byte lcdControl;

   mmu = GB_getMMU (gpu->gb);
   memory = MMU_getMemory (mmu);

   gpu->scanlineCounter += cycles;
   currentLine = MMU_readByte (mmu, 0xFF44);
   lcdControl = MMU_readByte (mmu, 0xFF40);

   if (testBit (lcdControl, 7)) {
      if (gpu->scanlineCounter >= SCANLINE_CYCLES) {
         /* Draw the current scanline before moving on
            to the next */
         if (currentLine < NUM_VISIBLE_SCANLINES)
            GPU_drawScanline (gpu);

         currentLine++;
         gpu->scanlineCounter -= SCANLINE_CYCLES;
      }
      
      if (currentLine >= NUM_VISIBLE_SCANLINES) {
         GB_requestInterrupt (gpu->gb, INT_VBLANK);
      } else if (currentLine >= NUM_SCANLINES) {
         currentLine = 0;
      }

      memory[0xFF44] = currentLine;
   }
}

void GPU_drawScanline (GPU gpu) {
   MMU mmu;
   int currentLine;

   mmu = GB_getMMU (gpu->gb);

   currentLine = MMU_readByte (mmu, 0xFF44);

   GPU_getPalette (gpu, PALETTE_BG);
   GPU_getPalette (gpu, PALETTE_OBJ0);
   GPU_getPalette (gpu, PALETTE_OBJ1);

   GPU_drawBackground (gpu);
   GPU_drawSprites (gpu);
}

void GPU_drawBackground (GPU gpu) {
   GUI gui;
   MMU mmu;
   byte lcdControl;
   Uint8 *pixels;
   int i, j;
   int currentLine;
   int verticalTileIndex;
   int scrollY;
   int tileIndexLocation;
   byte tileIndex;
   signed_byte signedTileIndex;
   int tileVerticalOffset;
   int framebufferIndex;
   int tileDataAddress;
   int colourIndex;
   byte first, second;

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

         if (!testBit (lcdControl, 4)) {
            /* Tile data from 8800-97FF */
            signedTileIndex = (signed_byte)tileIndex;
            tileDataAddress = 0x8800 + (TILE_SIZE_BYTES * (signedTileIndex + 128));     
            assert (tileDataAddress <= 0x97FF);
         } else {
            /* Tile data from 8000-8FFF */
            tileDataAddress = 0x8000 + (TILE_SIZE_BYTES * tileIndex);     
            assert (tileDataAddress <= 0x8FFF);
         }

         /* Read the bytes that contain the pixel data */
         first = MMU_readByte (mmu, tileDataAddress + 2*tileVerticalOffset);
         second = MMU_readByte (mmu, tileDataAddress + 2*tileVerticalOffset + 1);

         for (j = BG_TILE_WIDTH-1; j >= 0; j--) {
            framebufferIndex = currentLine*WINDOW_WIDTH + (BG_TILE_WIDTH*i + 7-j);
            assert (framebufferIndex < (WINDOW_WIDTH*WINDOW_HEIGHT));

            colourIndex = 0;
            if (testBit (first, j)) colourIndex |= 1;
            if (testBit (second, j)) colourIndex |= 2;

            pixels[framebufferIndex] = gpu->bgPalette[colourIndex];
         }
      }
   } else {
      /* Background is white */ 
      for (i = 0; i < WINDOW_WIDTH; i++) {
         pixels[currentLine*WINDOW_WIDTH + i] = COLOUR_WHITE;
      }
   }
}

void GPU_drawSprites (GPU gpu) {
   GUI gui;
   MMU mmu;
   byte lcdControl;
   byte *memory;
   spriteAttribute sprite;
   Uint8 *pixels;
   int i;
   int j;
   int currentAddress;
   int spriteHeight;
   int currentLine;
   int tileVerticalOffset;
   int framebufferIndex;
   int tileDataAddress;
   int colourIndex;
   byte first, second;

   mmu = GB_getMMU (gpu->gb);
   gui = GB_getGUI (gpu->gb);

   memory = MMU_getMemory (mmu);
   pixels = GUI_getFramebuffer (gui);

   lcdControl = MMU_readByte (mmu, 0xFF40);
   currentLine = MMU_readByte (mmu, 0xFF44);

   if (testBit (lcdControl, 1)) {
      /* If sprites are enabled */
      currentAddress = 0xFE00;

      if (testBit (lcdControl, 2)) {
         spriteHeight = 16;
      } else {
         spriteHeight = 8;
      }

      for (i = 0; i < NUM_SPRITES; i++) {
         memcpy (&sprite, memory+currentAddress, sizeof(spriteAttribute));
         sprite.yPos -= 16;
         sprite.xPos -= 8;

         if (sprite.yPos <= currentLine && (sprite.yPos + spriteHeight) > currentLine && 
             sprite.xPos >= 0 && sprite.xPos < WINDOW_WIDTH) {
            /* If this sprite is visible and is part of the current scanline */

            tileVerticalOffset = currentLine % spriteHeight; 
            tileDataAddress = 0x8000 + (TILE_SIZE_BYTES*(spriteHeight/8)*(sprite.tileNumber));

            /* Read the bytes that contain the pixel data */
            first = MMU_readByte (mmu, tileDataAddress + 2*tileVerticalOffset);
            second = MMU_readByte (mmu, tileDataAddress + 2*tileVerticalOffset + 1);

            for (j = BG_TILE_WIDTH-1; j >= 0; j--) {
               framebufferIndex = currentLine*WINDOW_WIDTH + sprite.xPos + (7-j);
               assert (framebufferIndex < (WINDOW_WIDTH*WINDOW_HEIGHT));

               colourIndex = 0;
               if (testBit (first, j)) colourIndex |= 1;
               if (testBit (second, j)) colourIndex |= 2;

               if (colourIndex != 0) {
                  /* Colour index 0 is transparent */
                  if (testBit (sprite.flags, 4)) {
                     pixels[framebufferIndex] = gpu->objPalette1[colourIndex];
                  } else {
                     pixels[framebufferIndex] = gpu->objPalette0[colourIndex];
                  }
               }
            }
         }

         currentAddress += sizeof(spriteAttribute);    
      }
   }
}

void GPU_getPalette (GPU gpu, int type) {
   MMU mmu;
   byte paletteData;
   Uint8 *palette;
   GUI gui;
   int i;
   int curColourIndex;

   mmu = GB_getMMU (gpu->gb);
   gui = GB_getGUI (gpu->gb);
   
   if (type == PALETTE_BG) {
      paletteData = MMU_readByte (mmu, 0xFF47); 
      palette = gpu->bgPalette;
   } else if (type == PALETTE_OBJ0) {
      paletteData = MMU_readByte (mmu, 0xFF48); 
      palette = gpu->objPalette0;
   } else if (type == PALETTE_OBJ1) {
      paletteData = MMU_readByte (mmu, 0xFF49); 
      palette = gpu->objPalette1;
   } else {
      palette = NULL;
   }

   assert (palette != NULL);

   for (i = 0; i < NUM_COLOURS; i++) {
      curColourIndex = 0;
      if (testBit (paletteData, 2*i)) {
         curColourIndex |= 1;
      }

      if (testBit (paletteData, 2*i + 1)) {
         curColourIndex |= 2;
      }

      assert (curColourIndex >= 0 && curColourIndex < 4);

      switch (curColourIndex) {
         case 0:
            palette[i] = COLOUR_WHITE;
            break;
         case 1:
            palette[i] = COLOUR_LIGHTGRAY;
            break;
         case 2:
            palette[i] = COLOUR_DARKGRAY;
            break;
         case 3:
            palette[i] = COLOUR_BLACK;
            break;
         default:
            break;
      }
   }
}

void GPU_updateLCDStatus (GPU gpu) {
   MMU mmu;
   byte *memory;
   byte status;
   byte currentLine;
   byte lastMode, newMode;
   byte lcdControl;
   bool requestInterrupt = FALSE;

   mmu = GB_getMMU (gpu->gb);
   lcdControl = MMU_readByte (mmu, 0xFF40);
   status = MMU_readByte (mmu, 0xFF41);

   if (testBit (lcdControl, 7)) {
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
   } else {
      clearBit (&status, 1);
      setBit (&status, 0);
      MMU_writeByte (mmu, 0xFF44, 0);
      MMU_writeByte (mmu, 0xFF41, status);
      gpu->scanlineCounter = 456;
   }
}
