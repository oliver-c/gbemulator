#include <stdlib.h>
#include <assert.h>

#include <SDL.h>

#include "GB.h"
#include "GUI.h"
#include "GPU.h"
#include "MMU.h"
#include "Timer.h"

#include "types.h"
#include "bitOperations.h"

struct GUI {
   GB gb;
   SDL_Surface *screen;
   colour framebuffer[WINDOW_WIDTH * WINDOW_HEIGHT];
   Timer frameTimer;
   bool flippedThisFrame;
   int frameCount;
};

void GUI_handleEvents (GUI gui);
void GUI_updateJoypad (GUI gui);

GUI GUI_init (GB gb) {
   GUI newGUI = (GUI)malloc(sizeof(struct GUI));
   assert (newGUI != NULL);

   newGUI->gb = gb;
   newGUI->flippedThisFrame = FALSE;
   newGUI->frameCount = 0;
   newGUI->frameTimer = Timer_init ();

   Timer_reset (newGUI->frameTimer);
   Timer_start (newGUI->frameTimer);

   SDL_Init (SDL_INIT_VIDEO); 
   
   SDL_WM_SetCaption (WINDOW_CAPTION, NULL); 
   newGUI->screen = SDL_SetVideoMode (WINDOW_WIDTH, WINDOW_HEIGHT, 0, 
                                      SDL_DOUBLEBUF | SDL_SWSURFACE);

   return newGUI;
}

void GUI_free (GUI gui) {
   SDL_FreeSurface (gui->screen);
   Timer_free (gui->frameTimer);

   free (gui);
}

void GUI_update (GUI gui) {
   MMU mmu;
   int currentLine;

   mmu = GB_getMMU (gui->gb);
   currentLine = MMU_readByte (mmu, 0xFF44);

   /* Update screen */

   if (currentLine < NUM_VISIBLE_SCANLINES) {
      gui->flippedThisFrame = FALSE;
   }
   
   if (currentLine >= NUM_VISIBLE_SCANLINES && !gui->flippedThisFrame) {
      /* Entered V-Blank, flip the SDL screen once */
      SDL_Flip (gui->screen); 
      gui->flippedThisFrame = TRUE;
      gui->frameCount++;

      GUI_handleEvents (gui);

      if (Timer_getTicks (gui->frameTimer) >= 5000) {
         printf ("fps = %.2lf\n", (double)(gui->frameCount)/5);
         gui->frameCount = 0;
         Timer_reset (gui->frameTimer);
      }
   }

   GUI_updateJoypad (gui);
}

void GUI_handleEvents (GUI gui) {
   SDL_Event event;

   while (SDL_PollEvent (&event)) {
      if (event.type == SDL_QUIT) {
         GB_setRunning (gui->gb, FALSE);
      }
   }
}

void GUI_updateJoypad (GUI gui) {
   MMU mmu;
   byte joypad;

   mmu = GB_getMMU (gui->gb);
   joypad = MMU_readByte (mmu, 0xFF00);

   setBit (&joypad, 7);
   setBit (&joypad, 6);

   /* Temporary, no buttons pressed */
   setBit (&joypad, 3);
   setBit (&joypad, 2);
   setBit (&joypad, 1);
   setBit (&joypad, 0);

   MMU_writeByte (mmu, 0xFF00, joypad);
}

Uint32 * GUI_getFramebuffer (GUI gui) {
   return gui->screen->pixels;
}

void GUI_updateScanline (GUI gui, int scanlineNumber) {
   int i;
   colour *scanline;
   Uint32 *screenPixels;

   scanline = gui->framebuffer + (scanlineNumber*WINDOW_WIDTH);

   screenPixels = (Uint32 *)(gui->screen->pixels) + (WINDOW_WIDTH*scanlineNumber);

   for (i = 0; i < WINDOW_WIDTH; i++) {
      screenPixels[i] = SDL_MapRGB (gui->screen->format, scanline[i].r, 
                                    scanline[i].g, scanline[i].b);
   }
}
