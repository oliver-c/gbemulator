#include <stdlib.h>
#include <assert.h>

#include <SDL.h>

#include "GB.h"
#include "GUI.h"
#include "GPU.h"
#include "MMU.h"

#include "types.h"

struct GUI {
   GB gb;
   SDL_Surface *screen;
   colour framebuffer[WINDOW_WIDTH * WINDOW_HEIGHT];
   bool flippedThisFrame;
};

void GUI_handleEvents (GUI gui);

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
}

void GUI_handleEvents (GUI gui) {
   SDL_Event event;

   while (SDL_PollEvent (&event)) {
      if (event.type == SDL_QUIT) {
         GB_setRunning (gui->gb, FALSE);
      }
   }
}

colour * GUI_getFramebuffer (GUI gui) {
   return gui->framebuffer;
}
