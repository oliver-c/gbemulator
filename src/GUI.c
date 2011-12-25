#include <stdlib.h>
#include <assert.h>

#include <SDL.h>

#include "GB.h"
#include "GUI.h"
#include "GPU.h"
#include "MMU.h"

#include "types.h"
#include "bitOperations.h"

struct GUI {
   GB gb;
   SDL_Surface *screen;
   colour framebuffer[WINDOW_WIDTH * WINDOW_HEIGHT];
   bool flippedThisFrame;
};

void GUI_handleEvents (GUI gui);
void GUI_updateJoypad (GUI gui);

GUI GUI_init (GB gb) {
   GUI newGUI = (GUI)malloc(sizeof(struct GUI));
   assert (newGUI != NULL);

   newGUI->gb = gb;
   newGUI->flippedThisFrame = FALSE;

   SDL_Init (SDL_INIT_VIDEO); 
   
   SDL_WM_SetCaption (WINDOW_CAPTION, NULL); 
   newGUI->screen = SDL_SetVideoMode (WINDOW_WIDTH, WINDOW_HEIGHT, 0, 
                                      SDL_DOUBLEBUF | SDL_HWPALETTE);

   return newGUI;
}

void GUI_free (GUI gui) {
   SDL_FreeSurface (gui->screen);

   free (gui);
}

void GUI_update (GUI gui) {
   MMU mmu;
   int j;
   int currentLine;
   Uint32 *screenPixels;
   colour currentColour;

   mmu = GB_getMMU (gui->gb);
   currentLine = MMU_readByte (mmu, 0xFF44);

   /* Update screen */
   SDL_LockSurface (gui->screen);

   if (currentLine < NUM_VISIBLE_SCANLINES) {
      gui->flippedThisFrame = FALSE;
      screenPixels = (Uint32 *)(gui->screen->pixels) + (WINDOW_WIDTH*currentLine);

      for (j = 0; j < WINDOW_WIDTH; j++) {
         currentColour = gui->framebuffer[WINDOW_WIDTH*currentLine + j];
         screenPixels[j] = SDL_MapRGB (gui->screen->format, currentColour.r, 
                                       currentColour.g, currentColour.b); 
      }
   }
   
   if (currentLine >= NUM_VISIBLE_SCANLINES && !gui->flippedThisFrame) {
      /* Entered V-Blank, flip the SDL screen once */
      SDL_Flip (gui->screen);
      gui->flippedThisFrame = TRUE;
   }

   SDL_UnlockSurface (gui->screen);
   GUI_handleEvents (gui);
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

colour * GUI_getFramebuffer (GUI gui) {
   return gui->framebuffer;
}
